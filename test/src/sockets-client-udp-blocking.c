#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
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

// See sockets-server-udp-blocking.c -- must be running already as a separate
// executable
void test_udp_client(int server_port) {

  // Prepare client socket
  // Use blocking sockets
  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(socket_fd != -1);

  // Prepare sockaddr_in for client
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  // Connect from client
  char message[] = "There's gonna be a party when the wolf comes home";
  int len = strlen(message);
  char client_buffer[BUFSIZE];

  TEST(connect(socket_fd, (struct sockaddr *)&sockaddr_in,
               sizeof(sockaddr_in)) != -1);

  // Client writes a message to server
  TEST(send(socket_fd, message, len, 0) == len);

  // Client reads from server
  int32_t bytes_received = recv(socket_fd, client_buffer, len, 0);
  TEST(bytes_received == len);

  // Message received should be the same as message sent
  TEST(strncmp(message, client_buffer, len) == 0);

  // Shut down client
  close(socket_fd);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
    return 1;
  }
  int port;
  if (sscanf(argv[1], "%d", &port) != 1) {
    fprintf(stderr, "Invalid port number: %s\n", argv[1]);
    return 1;
  }
  test_udp_client(port);

  return t_status;
}
