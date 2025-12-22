#include "poll.h"
#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
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

int BUFSIZE = 256;
int EXPECTED_CONNECTIONS = 10;
int TIMEOUT = 100;

size_t respond_to_client(int client_fd) {
  char buffer[BUFSIZE];
  ssize_t total_bytes_read = 0, bytes_read = 0;
  while ((bytes_read = recv(client_fd, buffer, BUFSIZE, 0)) > 0) {
    total_bytes_read += bytes_read;
    // Echo back the data received from the client
    send(client_fd, buffer, bytes_read, 0);
  }
  return total_bytes_read;
}

void run_tcp_server() {
  // Prepare server socket
  int server_port = 4001;
  // Use blocking sockets
  int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  // Bind server to socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(server_port);
  TEST(bind(server_socket_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);

  // Listen on socket
  socklen_t client_len = sizeof(struct sockaddr_in);
  int client_socket_fd;
  struct sockaddr_in client_address;
  int32_t bytes_read = 0, total_bytes_read = 0;
  TEST(listen(server_socket_fd, 1) != -1);

  // Server accepts connection
  struct pollfd client_fds[EXPECTED_CONNECTIONS];

  // Wait until a fixed number of clients connect
  for (size_t i = 0; i < EXPECTED_CONNECTIONS; i++) {
    client_socket_fd = accept(server_socket_fd,
                              (struct sockaddr *)&client_address, &client_len);
    TEST(client_socket_fd != -1);
    client_fds[i].fd = client_socket_fd;
    client_fds[i].events &= POLLIN;
  }

  // Poll on the array of client file descriptors and respond to
  // any that are ready
  size_t connections = 0;
  while (poll(client_fds, EXPECTED_CONNECTIONS, TIMEOUT) != 0) {
    for (int i = 0; i < EXPECTED_CONNECTIONS; i++) {
      if (client_fds[i].revents | POLLIN) {
        total_bytes_read += respond_to_client(client_fds[i].fd);
        // If fd is negative, revents will be set to 0 on the next call
        client_fds[i].fd = ~client_fds[i].fd;
      }
    }
  }

  TEST(total_bytes_read > 0);

  for (size_t i = 0; i < EXPECTED_CONNECTIONS; i++) {
    // fd was set to the complement of itself, so
    // it needs to be complemented again
    close(~client_fds[i].fd);
  }
  close(server_socket_fd);
}

int main() {
  run_tcp_server();

  return t_status;
}
