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

// Server must be running already as a separate executable
// (except when testing the case where connect() fails)
void test_tcp_client(int server_port) {

  // Prepare sockaddr_in for client
  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  TEST(socket_fd != -1);
  TEST(connect(socket_fd, (struct sockaddr *)&sockaddr_in,
               sizeof(sockaddr_in)) == 0);

  // Hang up immediately
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
  test_tcp_client(port);

  return t_status;
}
