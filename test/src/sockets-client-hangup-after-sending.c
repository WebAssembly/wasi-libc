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

// See sockets-server.c -- must be running already as a separate executable
void test_tcp_client() {
  // Prepare server socket
  int server_port = 4001;

  // Prepare sockaddr_in for client
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  // Connect from client
  char message[] = "There's gonna be a party when the wolf comes home";
  int len = strlen(message);
  char client_buffer[BUFSIZE];

  int socket_fd = wait_for_server(&sockaddr_in);

  // Client writes a message to server
  TEST(send(socket_fd, message, len, 0) == len);

  // Shut down client
  close(socket_fd);
}

int main(void) {
  test_tcp_client();

  return t_status;
}
