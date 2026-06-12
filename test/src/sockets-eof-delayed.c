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

int main() {
  char buf[10];
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
  TEST(listen(listener_fd, 1) != -1);

  // Connect a client to our server
  int client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  TEST(client_fd != -1);
  TEST(connect(client_fd, (struct sockaddr *)&server_address,
               server_address_len) != -1 ||
       errno == EINPROGRESS);

  // Wait for the client to be writable, signaling the connection is there.
  struct pollfd client, listener;
  client.fd = client_fd;
  client.events = POLLWRNORM;
  TEST(poll(&client, 1, -1) == 1);

  // Various ways of seeing that this isn't at EOF yet.
  TEST(read(client_fd, buf, sizeof(buf)) == -1 && errno == EWOULDBLOCK);
  client.events = POLLRDNORM;
  TEST(poll(&client, 1, 0) == 0);
  TEST(read(client_fd, buf, sizeof(buf)) == -1 && errno == EWOULDBLOCK);

  // Accept the socket on the listener, then immediately close it.
  int server_fd;
  listener.fd = listener_fd;
  listener.events = POLLRDNORM;
  TEST(poll(&listener, 1, -1) == 1);
  TEST((server_fd = accept(listener_fd, NULL, NULL)) != -1);
  TEST(close(server_fd) != -1);

  // Test to make sure that the client is now at EOF.
  TEST(poll(&client, 1, -1) == 1);
  TEST(read(client_fd, buf, sizeof(buf)) == 0);
  TEST(read(client_fd, buf, sizeof(buf)) == 0);

  return t_status;
}
