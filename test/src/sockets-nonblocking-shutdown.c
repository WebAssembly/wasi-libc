#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
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

#define N 10

int main() {
  int listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  // Setup a listener bound to port 0 to have the OS assign us one.
  struct sockaddr_in server_address;
  socklen_t server_address_len = sizeof(server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server_address.sin_port = 0;
  TEST(bind(listener_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);
  TEST(getsockname(listener_fd, (struct sockaddr *)&server_address,
                   &server_address_len) != -1);
  TEST(listen(listener_fd, N) != -1);

  int client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  TEST(client_fd != -1);
  int rc;
  TEST((rc = connect(client_fd, (struct sockaddr *)&server_address,
                     server_address_len)) != -1 ||
       errno == EINPROGRESS);
  if (rc == -1) {
    struct pollfd pfd = {.fd = client_fd, .events = POLLWRNORM};
    TEST(poll(&pfd, 1, -1) != -1);
  }

  TEST(shutdown(client_fd, SHUT_RDWR) != -1);

  char buf[10];
  TEST(recv(client_fd, buf, sizeof(buf), 0) == 0);
  TEST(send(client_fd, buf, sizeof(buf), 0) == -1 && errno == EPIPE);

  return t_status;
}
