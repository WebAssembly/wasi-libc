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

int BUFSIZE = 256;

void run_udp_server() {
  // Use blocking sockets
  int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(server_socket_fd != -1);
  if (server_socket_fd == -1)
    return;

  // Bind server to socket
  struct sockaddr_in server_address;
  socklen_t server_address_len = sizeof(server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = 0;
  TEST(bind(server_socket_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);
  if (t_status != 0)
    return;

  TEST(getsockname(server_socket_fd, (struct sockaddr *)&server_address,
                   &server_address_len) != -1);

  printf("%d\n", ntohs(server_address.sin_port));

  struct sockaddr_in client_address;
  socklen_t address_len = sizeof(client_address);

  // Server waits for input and echoes message back to client
  // The server shuts down after the client closes the connection
  int bytes_read = 0;
  char buffer[BUFSIZE];

  while (bytes_read <= 0) {
    bytes_read = recvfrom(server_socket_fd, buffer, BUFSIZE, 0,
                          (struct sockaddr *)&client_address, &address_len);
    if (bytes_read > 0)
      sendto(server_socket_fd, buffer, bytes_read, 0,
             (struct sockaddr *)&client_address, address_len);
  }

  TEST(bytes_read > 0);

  close(server_socket_fd);
}

int main() {
  run_udp_server();

  return t_status;
}
