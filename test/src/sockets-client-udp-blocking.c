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

static int wait_for_server(struct sockaddr_in *addr) {
  for (int attempt = 0; attempt < 200; attempt++) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    TEST(fd != -1);
    if (connect(fd, (struct sockaddr *)addr, sizeof(*addr)) != -1)
      return fd;
    close(fd);
    usleep(5000); // sleep for 5ms
  }
  t_error("server didn't come online within 1 second (errno = %d)\n", errno);
  return -1;
}

// See sockets-server-udp-blocking.c -- must be running already as a separate
// executable
void test_udp_client() {
  // Prepare server socket
  int server_port = 4001;

  // Prepare client socket
  // Use blocking sockets
  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(socket_fd != -1);

  // Prepare sockaddr_in for client
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  // Synchronize the UDP port being bound by waiting for a TCP connection to
  // go through.
  close(wait_for_server(&sockaddr_in));

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

int main(void) {
  test_udp_client();

  return t_status;
}
