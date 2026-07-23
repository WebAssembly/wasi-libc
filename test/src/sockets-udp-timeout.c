#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static void set_timeout_ms(int fd, int optname, long ms) {
  struct timeval tv = {.tv_sec = ms / 1000, .tv_usec = (ms % 1000) * 1000};
  TEST(setsockopt(fd, SOL_SOCKET, optname, &tv, sizeof(tv)) == 0);
}

static struct timeval get_timeout(int fd, int optname) {
  struct timeval tv;
  memset(&tv, 0xff, sizeof(tv));
  socklen_t len = sizeof(tv);
  TEST(getsockopt(fd, SOL_SOCKET, optname, &tv, &len) == 0);
  TEST(len == sizeof(tv));
  return tv;
}

static int bind_loopback(struct sockaddr_in *addr) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(fd != -1);

  struct sockaddr_in any = {
      .sin_family = AF_INET,
      .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
      .sin_port = htons(0),
  };
  TEST(bind(fd, (struct sockaddr *)&any, sizeof(any)) == 0);

  socklen_t addrlen = sizeof(*addr);
  TEST(getsockname(fd, (struct sockaddr *)addr, &addrlen) == 0);
  return fd;
}

static void test_getsockopt_setsockopt(void) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(fd != -1);

  struct timeval tv = get_timeout(fd, SO_RCVTIMEO);
  TEST(tv.tv_sec == 0 && tv.tv_usec == 0);
  tv = get_timeout(fd, SO_SNDTIMEO);
  TEST(tv.tv_sec == 0 && tv.tv_usec == 0);

  set_timeout_ms(fd, SO_RCVTIMEO, 1500);
  tv = get_timeout(fd, SO_RCVTIMEO);
  TEST(tv.tv_sec == 1 && tv.tv_usec == 500000);
  tv = get_timeout(fd, SO_SNDTIMEO);
  TEST(tv.tv_sec == 0 && tv.tv_usec == 0);

  set_timeout_ms(fd, SO_SNDTIMEO, 3000);
  tv = get_timeout(fd, SO_SNDTIMEO);
  TEST(tv.tv_sec == 3 && tv.tv_usec == 0);
  tv = get_timeout(fd, SO_RCVTIMEO);
  TEST(tv.tv_sec == 1 && tv.tv_usec == 500000);

  TEST(close(fd) == 0);
}

static void test_setsockopt_invalid(void) {
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(fd != -1);

  struct timeval tv = {.tv_sec = 1, .tv_usec = 1000000};
  TEST(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1 &&
       errno == EINVAL);
  tv.tv_usec = -1;
  TEST(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1 &&
       errno == EINVAL);

  tv.tv_usec = 0;
  TEST(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv) - 1) == -1 &&
       errno == EINVAL);
  TEST(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv) - 1) == -1 &&
       errno == EINVAL);

  tv = get_timeout(fd, SO_RCVTIMEO);
  TEST(tv.tv_sec == 0 && tv.tv_usec == 0);
  tv = get_timeout(fd, SO_SNDTIMEO);
  TEST(tv.tv_sec == 0 && tv.tv_usec == 0);

  TEST(close(fd) == 0);
}

static void test_recv_timeout(void) {
  struct sockaddr_in addr;
  int fd = bind_loopback(&addr);
  set_timeout_ms(fd, SO_RCVTIMEO, 1);

  char buffer[256];

  for (int i = 0; i < 2; i++)
    TEST(recv(fd, buffer, sizeof(buffer), 0) == -1 && errno == EWOULDBLOCK);

  struct sockaddr_in peer_addr, from;
  int peer = bind_loopback(&peer_addr);
  const char message[] = "still works";
  TEST(sendto(peer, message, strlen(message) + 1, 0,
              (const struct sockaddr *)&addr,
              sizeof(addr)) == (ssize_t)strlen(message) + 1);

  socklen_t fromlen = sizeof(from);
  TEST(recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from,
                &fromlen) == sizeof(message));
  TEST(memcmp(buffer, message, sizeof(message)) == 0);
  TEST(from.sin_port == peer_addr.sin_port);

  TEST(close(peer) == 0);
  TEST(close(fd) == 0);
}

static void test_recv_no_timeout(void) {
  struct sockaddr_in addr;
  int fd = bind_loopback(&addr);
  struct sockaddr_in peer_addr;
  int peer = bind_loopback(&peer_addr);

  char buffer[256];
  const char message[] = "hello";

  set_timeout_ms(fd, SO_RCVTIMEO, 10000);
  TEST(sendto(peer, message, strlen(message) + 1, 0,
              (const struct sockaddr *)&addr,
              sizeof(addr)) == (ssize_t)strlen(message) + 1);
  TEST(recv(fd, buffer, sizeof(buffer), 0) == sizeof(message));
  TEST(memcmp(buffer, message, sizeof(message)) == 0);

  set_timeout_ms(fd, SO_RCVTIMEO, 0);
  TEST(sendto(peer, message, strlen(message) + 1, 0,
              (const struct sockaddr *)&addr,
              sizeof(addr)) == (ssize_t)strlen(message) + 1);
  TEST(recv(fd, buffer, sizeof(buffer), 0) == sizeof(message));
  TEST(memcmp(buffer, message, sizeof(message)) == 0);

  TEST(close(peer) == 0);
  TEST(close(fd) == 0);
}

static void test_recv_timeout_nonblocking(void) {
  struct sockaddr_in addr;
  int fd = bind_loopback(&addr);
  set_timeout_ms(fd, SO_RCVTIMEO, 10000);

  char buffer[256];

  TEST(recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT) == -1 &&
       errno == EWOULDBLOCK);
  TEST(fcntl(fd, F_SETFL, O_NONBLOCK) == 0);
  TEST(recv(fd, buffer, sizeof(buffer), 0) == -1 && errno == EWOULDBLOCK);

  TEST(close(fd) == 0);
}

static void test_recv_timeout_connected(void) {
  struct sockaddr_in server_addr;
  int server = bind_loopback(&server_addr);

  int client = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(client != -1);
  set_timeout_ms(client, SO_RCVTIMEO, 5);

  TEST(connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
       0);
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(client_addr);
  TEST(getsockname(client, (struct sockaddr *)&client_addr, &addrlen) == 0);

  char buffer[256];
  TEST(recv(client, buffer, sizeof(buffer), 0) == -1 && errno == EWOULDBLOCK);

  const char message[] = "connected";
  TEST(sendto(server, message, strlen(message) + 1, 0,
              (const struct sockaddr *)&client_addr,
              sizeof(client_addr)) == (ssize_t)strlen(message) + 1);
  TEST(recv(client, buffer, sizeof(buffer), 0) == sizeof(message));
  TEST(memcmp(buffer, message, sizeof(message)) == 0);

  TEST(connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
       0);
  TEST(recv(client, buffer, sizeof(buffer), 0) == -1 && errno == EWOULDBLOCK);

  TEST(close(client) == 0);
  TEST(close(server) == 0);
}

static void test_recv_timeout_after_poll(void) {
  struct sockaddr_in addr;
  int fd = bind_loopback(&addr);
  struct sockaddr_in peer_addr;
  int peer = bind_loopback(&peer_addr);
  set_timeout_ms(fd, SO_RCVTIMEO, 5);

  struct pollfd pfd = {.fd = fd, .events = POLLIN};
  TEST(poll(&pfd, 1, 2) == 0);

  char buffer[256];
  TEST(recv(fd, buffer, sizeof(buffer), 0) == -1 && errno == EWOULDBLOCK);

  TEST(poll(&pfd, 1, 2) == 0);
  const char message[] = "after poll";
  TEST(sendto(peer, message, strlen(message) + 1, 0,
              (const struct sockaddr *)&addr,
              sizeof(addr)) == (ssize_t)strlen(message) + 1);
  TEST(recv(fd, buffer, sizeof(buffer), 0) == sizeof(message));
  TEST(memcmp(buffer, message, sizeof(message)) == 0);

  TEST(close(peer) == 0);
  TEST(close(fd) == 0);
}

// Sends don't seem to ever time out so there's not a whole lot to test here...
static void test_send_timeout(void) {
  struct sockaddr_in addr;
  int fd = bind_loopback(&addr);
  struct sockaddr_in peer_addr;
  int peer = bind_loopback(&peer_addr);
  set_timeout_ms(peer, SO_SNDTIMEO, 5);

  char buffer[256];
  const char message[] = "sent with a timeout";

  for (int i = 0; i < 4; i++) {
    TEST(sendto(peer, message, sizeof(message), 0, (struct sockaddr *)&addr,
                sizeof(addr)) == sizeof(message));
    TEST(recv(fd, buffer, sizeof(buffer), 0) == sizeof(message));
    TEST(memcmp(buffer, message, sizeof(message)) == 0);
  }

  TEST(close(peer) == 0);
  TEST(close(fd) == 0);
}

int main(void) {
  test_getsockopt_setsockopt();
  test_setsockopt_invalid();
  test_recv_timeout();
  test_recv_no_timeout();
  test_recv_timeout_nonblocking();
  test_recv_timeout_connected();
  test_recv_timeout_after_poll();
  test_send_timeout();

  return t_status;
}
