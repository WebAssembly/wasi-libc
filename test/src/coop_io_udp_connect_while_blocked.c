#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static int fd;

static void *receiver(void *arg) {
  (void)arg;
  char buf[32];
  ssize_t n = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
  TEST(n == 2);
  TEST(memcmp(buf, "hi", 2) == 0);
  return NULL;
}

int main(void) {
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(fd >= 0);

  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  TEST(bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(getsockname(fd, (struct sockaddr *)&addr, &addr_len) == 0);

  pthread_t rt;
  TEST(pthread_create(&rt, NULL, receiver, NULL) == 0);
  for (int i = 0; i < 10; i++)
    TEST(sched_yield() == 0);

  // Can't concurrently do a nonblocking receive.
  char buf[32];
  TEST(recvfrom(fd, buf, sizeof(buf), MSG_DONTWAIT, NULL, NULL) == -1 &&
       errno == EOPNOTSUPP);

  // Can't concurrently reconnect.
  TEST(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1 &&
       errno == EOPNOTSUPP);

  int sender = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(sender >= 0);
  TEST(sendto(sender, "hi", 2, 0, (struct sockaddr *)&addr, sizeof(addr)) == 2);
  TEST(pthread_join(rt, NULL) == 0);

  // Connect should not work.
  TEST(connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == 0);
  TEST(close(sender) == 0);
  TEST(close(fd) == 0);
  return t_status;
}
