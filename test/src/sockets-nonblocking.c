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

  // Wait for server to be ready to accept a client. Also test that `poll`
  // works.
  {
    struct pollfd poll_fd = {
        .fd = server_socket_fd, .events = POLLRDNORM, .revents = 0};
    TEST(poll(&poll_fd, 1, -1) != -1);
  }

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

  TEST(read(client_socket_fd, NULL, 0) == 0);
  TEST(write(client_socket_fd, NULL, 0) == 0);
  TEST(read(socket_fd, NULL, 0) == 0);
  TEST(write(socket_fd, NULL, 0) == 0);

  // Message received should be the same as message sent
  TEST(strcmp(message, client_buffer) == 0);

  // Server closes connection with client
  TEST(close(client_socket_fd) == 0);

  // Shut down client
  TEST(close(socket_fd) == 0);

  // Shut down server
  TEST(close(server_socket_fd) == 0);
}

static void test_poll_events() {
  int listener_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

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

  int client_fd;
  TEST((client_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) != -1);
  TEST(connect(client_fd, (struct sockaddr *)&server_address,
               server_address_len) != -1 ||
       errno == EINPROGRESS);

  struct pollfd pfds[3];
  pfds[0].fd = listener_fd;
  pfds[0].events = POLLRDNORM;
  pfds[0].revents = 0;
  pfds[1].fd = client_fd;
  pfds[1].events = POLLRDNORM;
  pfds[1].revents = 0;

  while (!(pfds[0].revents & POLLRDNORM)) {
    TEST(poll(pfds, 2, -1) != -1);
    TEST(pfds[1].revents == 0);
  }
  TEST(pfds[0].revents == POLLRDNORM);

  int server_fd;
  struct pollfd pfd;
  TEST((server_fd = accept4(listener_fd, NULL, NULL, SOCK_NONBLOCK)) != -1);
  pfd.fd = server_fd;
  pfd.events = POLLWRNORM;
  pfd.revents = 0;
  while (!(pfd.revents & POLLWRNORM))
    TEST(poll(&pfd, 1, -1) != -1);
  TEST(write(server_fd, "x", 1) == 1);
  TEST(close(server_fd) == 0);

  while (!(pfds[1].revents & POLLRDNORM)) {
    TEST(poll(pfds, 2, -1) != -1);
    TEST(pfds[0].revents == 0);
  }
  TEST(pfds[1].revents == POLLRDNORM);

  TEST(close(client_fd) == 0);
  TEST(close(listener_fd) == 0);
}

int main(void) {
  test_tcp_client();
  test_poll_events();

  return t_status;
}
