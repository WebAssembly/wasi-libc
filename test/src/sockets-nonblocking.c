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

int BUFSIZE = 256;

// See sockets-server.c -- must be running already as a separate executable
void test_tcp_client() {
  // Use non-blocking sockets
  int server_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  // Bind server to socket
  struct sockaddr_in server_address;
  socklen_t server_address_len = sizeof(server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = 0;
  TEST(bind(server_socket_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);

  TEST(getsockname(server_socket_fd, (struct sockaddr *)&server_address,
                   &server_address_len) != -1);
  int server_port = ntohs(server_address.sin_port);

  // Listen on socket
  char buffer[BUFSIZE];
  socklen_t client_len = sizeof(struct sockaddr_in);
  int client_socket_fd = -1;
  struct sockaddr_in client_address;
  int32_t bytes_read = 0;
  TEST(listen(server_socket_fd, 1) != -1);

  // Prepare client socket
  // Use non-blocking sockets
  int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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
               sizeof(sockaddr_in)) != -1 ||
       errno == EINPROGRESS);

  // Accept a client, turning the loop while it's not ready.
  while (!t_status) {
    TEST((client_socket_fd =
              accept4(server_socket_fd, (struct sockaddr *)&client_address,
                      &client_len, SOCK_NONBLOCK)) != -1 ||
         errno == EAGAIN);
    if (client_socket_fd != -1)
      break;
    struct pollfd poll_fd = {
        .fd = server_socket_fd, .events = POLLRDNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

  // client => server (write)
  while (!t_status) {
    ssize_t ret;
    TEST((ret = send(socket_fd, message, len, 0)) == len || errno == EAGAIN ||
         errno == ENOTCONN);
    if (ret > 0)
      break;
    struct pollfd poll_fd = {
        .fd = socket_fd, .events = POLLWRNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

  // client => server (read)
  while (!t_status) {
    TEST((bytes_read = recv(client_socket_fd, buffer, BUFSIZE, 0)) > 0 ||
         errno == EAGAIN);
    if (bytes_read > 0) {
      TEST(bytes_read == len);
      break;
    }
    struct pollfd poll_fd = {
        .fd = client_socket_fd, .events = POLLRDNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

  // server => client (write)
  while (!t_status) {
    ssize_t ret;
    TEST((ret = send(client_socket_fd, buffer, bytes_read, 0)) == bytes_read ||
         errno == EAGAIN);
    if (ret > 0) {
      TEST(ret == bytes_read);
      break;
    }
    struct pollfd poll_fd = {
        .fd = client_socket_fd, .events = POLLWRNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

  // server => client (read)
  while (!t_status) {
    TEST((bytes_read = recv(socket_fd, client_buffer, BUFSIZE, 0)) > 0 ||
         errno == EAGAIN);
    if (bytes_read > 0) {
      TEST(bytes_read == len);
      break;
    }
    struct pollfd poll_fd = {
        .fd = socket_fd, .events = POLLRDNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

  // Message received should be the same as message sent
  TEST(strcmp(message, client_buffer) == 0);

  // Server closes connection with client
  TEST(close(client_socket_fd) == 0);

  // Shut down client
  TEST(close(socket_fd) == 0);

  // Shut down server
  TEST(close(server_socket_fd) == 0);
}

int main(void) {
  test_tcp_client();

  return t_status;
}
