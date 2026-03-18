#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

#define _DEBUG 1

#define DEBUG_PRINT(...)                                                       \
  if (_DEBUG) {                                                                \
    fprintf(stderr, __VA_ARGS__);                                              \
    fflush(stdout);                                                            \
  }

#define BUFSIZE 256
#define MAX_CONNECTIONS 10

enum client_state {
  CLIENT_SENDING,
  CLIENT_RECEIVING,
  CLIENT_DONE,
};

struct client {
  int fd;
  enum client_state state;
  char outgoing[BUFSIZE];
  char incoming[BUFSIZE];
};

// Queue of datagrams the server needs to echo back
struct echo_entry {
  char message[BUFSIZE];
  int msg_len;
  struct sockaddr_in addr;
  socklen_t addrlen;
};

void test_udp_client() {
  // Create non-blocking server socket
  int server_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
  TEST(server_fd != -1);

  struct sockaddr_in server_addr;
  socklen_t server_addr_len = sizeof(server_addr);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = 0;
  TEST(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
       -1);
  TEST(getsockname(server_fd, (struct sockaddr *)&server_addr,
                   &server_addr_len) != -1);
  int server_port = ntohs(server_addr.sin_port);

  // Address clients will send to
  struct sockaddr_in connect_addr;
  connect_addr.sin_family = AF_INET;
  connect_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  connect_addr.sin_port = htons(server_port);

  // Initialize client state machines
  struct client clients[MAX_CONNECTIONS];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    clients[i].fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    TEST(clients[i].fd != -1);

    int32_t number = 0;
    getentropy(&number, sizeof(int32_t));
    sprintf(clients[i].outgoing, "%d", number);
    clients[i].incoming[0] = 0;
    clients[i].state = CLIENT_SENDING;
  }

  // Server echo queue: datagrams received but not yet sent back
  struct echo_entry echo_queue[MAX_CONNECTIONS];
  size_t echo_head = 0;
  size_t echo_tail = 0;

  // Setup the list of fds that we're going to be polling in the event loop
  // below.
  struct pollfd pfds[MAX_CONNECTIONS + 1];
  pfds[0].fd = server_fd;
  pfds[0].events = POLLIN;
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    pfds[i + 1].fd = clients[i].fd;
    pfds[i + 1].events = POLLOUT;
  }

  int clients_done = 0;

  while (t_status == 0 && clients_done < MAX_CONNECTIONS) {
    // Update server poll events based on whether we have queued echoes
    if (echo_head < echo_tail)
      pfds[0].events = POLLIN | POLLOUT;
    else
      pfds[0].events = POLLIN;

    TEST(poll(pfds, MAX_CONNECTIONS + 1, -1) != -1);

    // Handle server: receive datagrams and enqueue for echo
    if (pfds[0].revents & POLLIN) {
      struct sockaddr_in from_addr;
      socklen_t from_len = sizeof(from_addr);
      char buf[BUFSIZE];
      int received;
      TEST((received = recvfrom(server_fd, buf, BUFSIZE, 0,
                                (struct sockaddr *)&from_addr, &from_len)) !=
               -1 ||
           errno == EAGAIN);
      if (received > 0) {
        TEST(echo_tail < MAX_CONNECTIONS);
        memcpy(echo_queue[echo_tail].message, buf, received);
        echo_queue[echo_tail].msg_len = received;
        echo_queue[echo_tail].addr = from_addr;
        echo_queue[echo_tail].addrlen = from_len;
        echo_tail++;
        DEBUG_PRINT("Server received: %.*s (queued: %zu)\n", received, buf,
                    echo_tail - echo_head);
      } else {
        DEBUG_PRINT("Server failed to receive: %d\n", errno);
      }
    }

    // Handle server: send queued echoes back
    if (pfds[0].revents & POLLOUT) {
      TEST(echo_head < echo_tail);
      struct echo_entry *entry = &echo_queue[echo_head];
      ssize_t sent;
      TEST((sent = sendto(server_fd, entry->message, entry->msg_len, 0,
                          (struct sockaddr *)&entry->addr, entry->addrlen)) !=
               -1 ||
           errno == EAGAIN);
      if (sent > 0) {
        TEST(sent == entry->msg_len);
        DEBUG_PRINT("Server echoed: %.*s\n", entry->msg_len, entry->message);
        echo_head++;
      } else {
        DEBUG_PRINT("Server failed to send: %d\n", errno);
      }
    }

    // Handle client state machines
    for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
      int idx = 1 + i;
      if (!pfds[idx].revents)
        continue;

      switch (clients[i].state) {
      case CLIENT_SENDING: {
        socklen_t addr_len = sizeof(connect_addr);
        ssize_t sent;
        TEST((sent = sendto(clients[i].fd, clients[i].outgoing,
                            strlen(clients[i].outgoing) + 1, 0,
                            (struct sockaddr *)&connect_addr, addr_len)) !=
                 -1 ||
             errno == EAGAIN);
        if (sent > 0) {
          TEST(sent == (ssize_t)(strlen(clients[i].outgoing) + 1));
          clients[i].state = CLIENT_RECEIVING;
          pfds[idx].events = POLLIN;
          DEBUG_PRINT("Client %zu sent: %s\n", i, clients[i].outgoing);
        } else {
          DEBUG_PRINT("Client failed to send: %d\n", errno);
        }
        break;
      }

      case CLIENT_RECEIVING: {
        ssize_t received;
        TEST((received = recvfrom(clients[i].fd, clients[i].incoming, BUFSIZE,
                                  0, NULL, NULL)) != -1 ||
             errno == EAGAIN);
        if (received > 0) {
          clients[i].incoming[received] = 0;
          TEST(strcmp(clients[i].outgoing, clients[i].incoming) == 0);
          clients[i].state = CLIENT_DONE;
          pfds[idx].fd = -1;
          clients_done++;
          DEBUG_PRINT("Client %zu done: got '%s' (%d/%d)\n", i,
                      clients[i].incoming, clients_done, MAX_CONNECTIONS);
        } else {
          DEBUG_PRINT("Client failed to recv: %d\n", errno);
        }
        break;
      }

      case CLIENT_DONE:
        break;
      }
    }
  }

  // Close client sockets
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    TEST(clients[i].fd != -1);
    TEST(close(clients[i].fd) == 0);
  }

  // Close server socket
  TEST(close(server_fd) == 0);
}

int main(void) {
  test_udp_client();

  return t_status;
}
