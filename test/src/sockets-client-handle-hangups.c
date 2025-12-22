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
  return -1;
}

// Server must be running already as a separate executable
// (except when testing the case where connect() fails)
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
  if (socket_fd == -1)
    return;

  // Client writes a message to server
  // This should succeed even in the presence of hangups,
  // since send() only fails if there's a local error
  TEST(send(socket_fd, message, len, 0) == len);

  // Set timeout for recv
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 500000; // 500 milliseconds
  setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));

  // Client reads from server
  int32_t bytes_received = recv(socket_fd, client_buffer, len, 0);
  if (bytes_received == len) {
    // Message received should be the same as message sent
    TEST(strcmp(message, client_buffer) == 0);
  }

  // Shut down client
  close(socket_fd);
}

int main(void) {
  test_tcp_client();

  return t_status;
}
