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

void run_tcp_server() {
  // Prepare server socket
  int server_port = 4001;
  // Use blocking sockets
  int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(server_socket_fd != -1);
  if (server_socket_fd == -1)
    return;

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
  int client_socket_fd;
  struct sockaddr_in client_address;
  int32_t bytes_read = 0, total_bytes_read = 0;
  TEST(listen(server_socket_fd, 1) != -1);

  // Server accepts connection
  client_socket_fd =
      accept(server_socket_fd, (struct sockaddr *)&client_address, &client_len);
  TEST(client_socket_fd != -1);

  // Server waits for input and echoes message back to client
  // The server shuts down after the client closes the connection
  while ((bytes_read = recv(client_socket_fd, buffer, BUFSIZE, 0)) > 0) {
    total_bytes_read += bytes_read;
    // Echo back the data received from the client
    send(client_socket_fd, buffer, bytes_read, 0);
  }

  TEST(total_bytes_read > 0);

  close(client_socket_fd);
  close(server_socket_fd);
}

int main() {
  run_tcp_server();

  return t_status;
}
