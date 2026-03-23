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

#define _DEBUG 0

#define DEBUG_PRINT(...)                                                       \
  if (_DEBUG) {                                                                \
    fprintf(stderr, __VA_ARGS__);                                              \
    fflush(stdout);                                                            \
  }

#define BUFSIZE 256
#define MAX_CONNECTIONS 10

enum client_state {
  CLIENT_CONNECTING,
  CLIENT_SENDING,
  CLIENT_RECEIVING,
  CLIENT_DONE,
};

enum server_conn_state {
  SERVER_UNUSED,
  SERVER_READING,
  SERVER_WRITING,
  SERVER_DONE,
};

struct client {
  int fd;
  enum client_state state;
  char outgoing[BUFSIZE];
  char incoming[BUFSIZE];
};

struct server_conn {
  int fd;
  enum server_conn_state state;
  char buf[BUFSIZE];
  int buf_len;
};

void test_tcp_client() {
  // Create non-blocking server socket
  int server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
  TEST(listen(server_fd, MAX_CONNECTIONS) != -1);

  // Address clients will connect to
  struct sockaddr_in connect_addr;
  connect_addr.sin_family = AF_INET;
  connect_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  connect_addr.sin_port = htons(server_port);

  // Initialize client state machines
  struct client clients[MAX_CONNECTIONS];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    TEST((clients[i].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) !=
         -1);

    int32_t number = 0;
    getentropy(&number, sizeof(int32_t));
    sprintf(clients[i].outgoing, "%d", number);
    clients[i].incoming[0] = 0;

    int ret;
    TEST((ret = connect(clients[i].fd, (struct sockaddr *)&connect_addr,
                        sizeof(connect_addr))) != -1 ||
         errno == EINPROGRESS);
    clients[i].state = ret == 0 ? CLIENT_SENDING : CLIENT_CONNECTING;
  }

  // Initialize server-side connection state machines
  struct server_conn server_conns[MAX_CONNECTIONS];
  int num_server_conns = 0;
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    server_conns[i].fd = -1;
    server_conns[i].state = SERVER_UNUSED;
    server_conns[i].buf[0] = 0;
    server_conns[i].buf_len = 0;
  }

  // Initialize poll array once; entries are updated in place on state changes
  struct pollfd pfds[1 + MAX_CONNECTIONS * 2];

  int nfds = 0;

  // Server listener
  pfds[nfds].fd = server_fd;
  pfds[nfds].events = POLLIN;
  nfds++;

  // Client sockets: connecting/sending need POLLOUT
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    pfds[nfds].fd = clients[i].fd;
    pfds[nfds].events = POLLOUT;
    nfds++;
  }

  int clients_done = 0;

  // Turn this event loop until all clients have been handled.
  while (t_status == 0 && clients_done < MAX_CONNECTIONS) {
    // Wait for something to be ready, we'll iterate over the results below.
    TEST(poll(pfds, nfds, -1) != -1);

    for (int i = 0; i < (int)nfds; i++) {
      // Skip this pfd if it doesn't have any events ready.
      if (!pfds[i].revents)
        continue;

      // Handle server listener: accept new connections
      if (i == 0) {
        TEST(pfds[i].revents & POLLIN);
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int accepted;
        TEST((accepted = accept4(server_fd, (struct sockaddr *)&client_addr,
                                 &client_len, SOCK_NONBLOCK)) != -1 ||
             errno == EAGAIN);
        if (accepted != -1) {
          TEST(num_server_conns < MAX_CONNECTIONS);
          server_conns[num_server_conns].fd = accepted;
          server_conns[num_server_conns].state = SERVER_READING;
          pfds[nfds].fd = accepted;
          pfds[nfds].events = POLLIN;
          nfds++;
          num_server_conns++;
          DEBUG_PRINT("Server accepted connection %d (total: %d)\n", accepted,
                      num_server_conns);
        }
        continue;
      }

      // Handle client connections
      if (i < MAX_CONNECTIONS + 1) {
        size_t ci = i - 1;

        switch (clients[ci].state) {
        case CLIENT_CONNECTING: {
          int error = 0;
          socklen_t len = sizeof(error);
          TEST(getsockopt(clients[ci].fd, SOL_SOCKET, SO_ERROR, &error, &len) ==
               0);
          TEST(error == 0);
          clients[ci].state = CLIENT_SENDING;
          DEBUG_PRINT("Client %zu connected\n", ci);
          // fall through to `CLIENT_SENDING` below.
        }

        case CLIENT_SENDING: {
          ssize_t sent;
          TEST((sent = send(clients[ci].fd, clients[ci].outgoing,
                            strlen(clients[ci].outgoing), 0)) > 0 ||
               errno == EAGAIN);
          if (sent > 0) {
            TEST((size_t)sent == strlen(clients[ci].outgoing));
            clients[ci].state = CLIENT_RECEIVING;
            pfds[i].events = POLLIN;
            DEBUG_PRINT("Client %zu sent: %s\n", ci, clients[ci].outgoing);
          } else {
            continue;
          }
          // fall through to `CLIENT_RECEIVING`
        }

        case CLIENT_RECEIVING: {
          ssize_t received;
          TEST((received = recv(clients[ci].fd, clients[ci].incoming,
                                BUFSIZE - 1, 0)) != -1 ||
               errno == EAGAIN);
          if (received > 0) {
            TEST((size_t)received == strlen(clients[ci].outgoing));
            clients[ci].incoming[received] = 0;
            TEST(strcmp(clients[ci].outgoing, clients[ci].incoming) == 0);
            clients[ci].state = CLIENT_DONE;
            pfds[i].fd = -1;
            clients_done++;
            DEBUG_PRINT("Client %zu done: got '%s' (%d/%d)\n", ci,
                        clients[ci].incoming, clients_done, MAX_CONNECTIONS);
          } else {
            continue;
          }
          break;
        }

        case CLIENT_DONE:
          break;

        default:
          abort();
          break;
        }
        continue;
      }

      // Handle server-side connection state machines
      size_t si = i - MAX_CONNECTIONS - 1;

      switch (server_conns[si].state) {
      case SERVER_READING: {
        ssize_t received;
        TEST((received = recv(server_conns[si].fd, server_conns[si].buf,
                              BUFSIZE - 1, 0)) != -1 ||
             errno == EAGAIN);
        if (received > 0) {
          server_conns[si].buf_len = received;
          server_conns[si].buf[received] = 0;
          server_conns[si].state = SERVER_WRITING;
          pfds[i].events = POLLOUT;
          DEBUG_PRINT("Server conn %zu received: %s\n", si,
                      server_conns[si].buf);
        } else {
          continue;
        }
        // fall through to `SERVER_WRITING`.
      }

      case SERVER_WRITING: {
        ssize_t sent;
        TEST((sent = send(server_conns[si].fd, server_conns[si].buf,
                          server_conns[si].buf_len, 0)) != -1 ||
             errno == EAGAIN);
        if (sent > 0) {
          TEST(sent == server_conns[si].buf_len);
          server_conns[si].state = SERVER_DONE;
          pfds[i].fd = -1;
          DEBUG_PRINT("Server conn %zu echoed back\n", si);
        } else {
          continue;
        }
        break;
      }

      default:
        abort();
        break;
      }
    }
  }

  // Close client sockets
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    TEST(clients[i].fd != -1);
    TEST(close(clients[i].fd) == 0);
  }

  // Close server-side accepted sockets
  for (size_t i = 0; i < (size_t)num_server_conns; i++) {
    TEST(server_conns[i].fd != -1);
    TEST(close(server_conns[i].fd) == 0);
  }

  // Close server listener
  TEST(close(server_fd) == 0);
}

int main(void) {
  test_tcp_client();

  return t_status;
}
