#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
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

// #define _DEBUG 1

#ifdef _DEBUG
#define DEBUG_PRINT(...)                                                       \
  fprintf(stderr, __VA_ARGS__);                                                \
  fflush(stdout)
#else
#define DEBUG_PRINT(...)                                                       \
  do {                                                                         \
  } while (0)
#endif

#define BUFSIZE 256
size_t MAX_CONNECTIONS = 10;

struct response {
  int recipient_fd;
  char message[BUFSIZE];
};

struct request {
  bool sent;
  char message[BUFSIZE];
};

// We are done if client_incoming == client_outgoing
bool done(struct request out[MAX_CONNECTIONS],
          char in[MAX_CONNECTIONS][BUFSIZE]) {
  DEBUG_PRINT("Checking if we're done\n");
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    if (out[i].message[0] == 0) {
      return false;
    }
    if (strcmp(out[i].message, in[i]) != 0) {
      DEBUG_PRINT("%zu does not match: (%s) (%s)\n", i, out[i].message, in[i]);
      return false;
    }
    DEBUG_PRINT("%zu matches\n", i);
  }
  for (size_t i = 0; i < MAX_CONNECTIONS; i++)
    DEBUG_PRINT("out = %s in = %s\n", out[i].message, in[i]);
  return true;
}

// See sockets-server.c -- must be running already as a separate executable
void test_tcp_client() {
  // Prepare server socket
  int server_port = 4001;
  // Use non-blocking sockets
  int server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  // Bind server to socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(server_port);
  TEST(bind(server_socket_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);

  // Listen on socket
  char buffer[BUFSIZE];
  socklen_t client_len = sizeof(struct sockaddr_in);
  int client_socket_fd = -1;
  struct sockaddr_in client_address;
  int32_t bytes_read = 0, total_bytes_read = 0;
  TEST(listen(server_socket_fd, 1) != -1);

  // Prepare client sockets
  int client_sockets[MAX_CONNECTIONS];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    // Use non-blocking sockets
    client_sockets[i] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    TEST(client_sockets[i] != -1);
  }

  // Prepare sockaddr_in for client
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  // Connect from client
  struct request client_outgoing[MAX_CONNECTIONS];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    int32_t number = 0;
    getentropy(&number, sizeof(int32_t));
    sprintf(client_outgoing[i].message, "%d", number);
    client_outgoing[i].sent = false;
  }
  char client_incoming[MAX_CONNECTIONS][BUFSIZE];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    client_incoming[i][0] = 0;
  }

  int server_sockets[MAX_CONNECTIONS];
  int next_server_socket = 0;
  struct pollfd poll_fd_server[MAX_CONNECTIONS + 1];
  struct pollfd poll_fd_client[MAX_CONNECTIONS];
  char server_incoming[MAX_CONNECTIONS + 1][BUFSIZE];
  struct response server_outgoing[MAX_CONNECTIONS + 1];
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    server_outgoing[i].message[0] = 0;
  }
  poll_fd_server[0].fd = server_socket_fd;
  poll_fd_server[0].events = POLLRDNORM;
  poll_fd_server[0].revents = 0;
  int next_client = 0;
  bool failure = false;

  while (!done(client_outgoing, client_incoming) &&
         (next_client < MAX_CONNECTIONS * 2)) {

    DEBUG_PRINT("============== next_client = %d ================\n",
                next_client);

    // One client tries to connect
    if (next_client < MAX_CONNECTIONS) {
      int connect_result =
          connect(client_sockets[next_client], (struct sockaddr *)&sockaddr_in,
                  sizeof(sockaddr_in));
      if (connect_result == 0 || errno == EINPROGRESS || errno == EISCONN) {
        int32_t error = -1;
        socklen_t len = -1;

        struct pollfd poll_fd = {.fd = client_sockets[next_client],
                                 .events = POLLWRNORM,
                                 .revents = 0};
        int getsockopt_result = getsockopt(client_sockets[next_client],
                                           SOL_SOCKET, SO_ERROR, &error, &len);
        poll(&poll_fd, 1, 100);
        // Check if socket has become writable
        TEST(getsockopt(client_sockets[next_client], SOL_SOCKET, SO_ERROR,
                        &error, &len) == 0);
        if (error == 0) {
          next_client++;
          poll_fd_server[next_client].events = POLLRDNORM;
          poll_fd_server[next_client].revents = 0;
          poll_fd_client[next_client - 1].fd = client_sockets[next_client - 1];
          poll_fd_client[next_client - 1].events = POLLWRNORM;
          poll_fd_server[next_client - 1].revents = 0;
          DEBUG_PRINT("Client [#%d] = %d connected\n", next_client - 1,
                      client_sockets[next_client - 1]);
        }
      } else {
        failure = true;
        DEBUG_PRINT("FAILURE: client %d failed to connect: %s\n",
                    client_sockets[next_client], strerror(errno));
        break;
      }
      errno = 0;
    } else
      DEBUG_PRINT("No longer accepting connections\n");

    // Server polls for new connections and activity on existing connections,
    // simultaneously
    poll(poll_fd_server, next_server_socket + 1, 100);
    int to_poll = next_server_socket + 1;
    bool recv_failed = false;
    DEBUG_PRINT("Server looping through poll events errno = %s\n",
                strerror(errno));
    for (size_t i = 0; i < to_poll; i++) {
      if (i == 0) {
        int accept_result =
            accept4(server_socket_fd, (struct sockaddr *)&client_address,
                    &client_len, SOCK_NONBLOCK);
        if (accept_result != -1) {
          server_sockets[next_server_socket] = accept_result;
          DEBUG_PRINT("Server accepted new connection: %d, storing in %d\n",
                      server_sockets[next_server_socket], next_server_socket);
          poll_fd_server[next_client].fd = server_sockets[next_server_socket];
          next_server_socket++;
        }
        errno = 0;
      } else {
        DEBUG_PRINT("poll_fd_server[%zu].revents = %d (fd = %d) errno = %d\n",
                    i, poll_fd_server[i].revents, poll_fd_server[i].fd, errno);
        if (poll_fd_server[i].revents & POLLRDNORM) {
          int recv_result =
              recv(poll_fd_server[i].fd, &server_incoming[i], BUFSIZE, 0);
          if (recv_result == -1 && (errno == EAGAIN)) {
            // This shouldn't happen -- poll() reported that there was data
            DEBUG_PRINT("Shouldn't happen -- poll() said there was data but "
                        "recv() returned EAGAIN\n");
            errno = 0;
          } else if (recv_result == -1) {
            recv_failed = true;
          } else {
            server_outgoing[i].recipient_fd = poll_fd_server[i].fd;
            memcpy(server_outgoing[i].message, server_incoming[i], recv_result);
            server_outgoing[i].message[recv_result] = 0;
            DEBUG_PRINT("Received message: %s from %d (%d bytes)\n",
                        server_outgoing[i].message, poll_fd_server[i].fd,
                        recv_result);
          }
        }
      }
    }

    if (recv_failed) {
      DEBUG_PRINT("FAILURE: server reading from client failed\n");
      failure = true;
      break;
    }

    // All connected clients try to send
    bool send_failed = false;
    for (int32_t i = 0; i < next_client; i++) {
      DEBUG_PRINT("client_outgoing[%d] %s\n", i,
                  client_outgoing[i].sent ? "sent" : "not sent");
      if (!client_outgoing[i].sent) {
        client_outgoing[i].sent = true;
        int send_result = send(client_sockets[i], client_outgoing[i].message,
                               strlen(client_outgoing[i].message), 0);
        DEBUG_PRINT("Client %d sent %s with result %d and error = %s\n",
                    client_sockets[i], client_outgoing[i].message, send_result,
                    strerror(errno));
        if (errno == EAGAIN) {
          struct pollfd poll_fd = {
              .fd = client_sockets[i], .events = POLLWRNORM, .revents = 0};
          poll(&poll_fd, 1, 100);
          // Check if socket has become writable
          int32_t error = -1;
          socklen_t len = -1;
          TEST(getsockopt(client_sockets[i], SOL_SOCKET, SO_ERROR, &error,
                          &len) == 0);
          if (error != 0) {
            send_failed = true;
            break;
          }
          errno = 0;
        } else if (send_result == -1) {
          send_failed = true;
          break;
        }
      }
    }

    if (send_failed) {
      DEBUG_PRINT("FAILURE: client sending to server failed\n");
      failure = true;
      break;
    }

    // All connected clients try to receive
    if (next_client > 0) {
      for (int32_t i = 0; i < next_client; i++)
        poll_fd_client[i].events = POLLRDNORM;
      poll(poll_fd_client, next_client, 100);
    }
    DEBUG_PRINT("Looping through poll events\n");
    for (int32_t i = 0; i < next_client; i++) {
      DEBUG_PRINT("poll_fd_client[%d].revents = %d\n", i,
                  poll_fd_client[i].revents);
      if (poll_fd_client[i].revents & POLLRDNORM) {
        int bytes_received =
            recv(poll_fd_client[i].fd, client_incoming[i], BUFSIZE, 0);
        DEBUG_PRINT("Client %d received message %s, expected %s [received %d "
                    "bytes, length of incoming = %lu]\n",
                    poll_fd_client[i].fd, client_incoming[i],
                    client_outgoing[i].message, bytes_received,
                    strlen(client_incoming[i]));
        TEST(strcmp(client_outgoing[i].message, client_incoming[i]) == 0);
      }
    }
    DEBUG_PRINT("Looped through poll events\n");

    // Server replies to any pending messages
    for (size_t i = 0; i < MAX_CONNECTIONS + 1; i++) {
      if (server_outgoing[i].message[0]) {
        int send_result =
            send(server_outgoing[i].recipient_fd, server_outgoing[i].message,
                 strlen(server_outgoing[i].message) + 1, 0);
        DEBUG_PRINT("Server sent %s to %d (sent: %d bytes)\n",
                    server_outgoing[i].message, poll_fd_server[i].fd,
                    send_result);
        server_outgoing[i].message[0] = 0;
      }
    }
  }

  TEST(failure == false);
  TEST(next_client == MAX_CONNECTIONS);

  // Clients shut themselves down
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    TEST(client_sockets[i] != -1);
    close(client_sockets[i]);
  }

  // Server shuts down all connections
  for (size_t i = 0; i < MAX_CONNECTIONS; i++) {
    TEST(server_sockets[i] != -1);
    close(server_sockets[i]);
  }

  // Server shuts down
  close(server_socket_fd);
}

int main(void) {
  test_tcp_client();

  return t_status;
}
