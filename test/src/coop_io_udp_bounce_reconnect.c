#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
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
static _Atomic int got_datagram;
static _Atomic int got_error;

static void *receiver(void *arg) {
  (void)arg;
  char buf[32];
  errno = 0;
  ssize_t n = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
  if (n == 2) {
    got_datagram++;
  } else if (n == -1 && (errno == EOPNOTSUPP || errno == EINVAL)) {
    // The socket was reconfigured (or left streamless by a failed
    // reconnect) while this thread waited its turn.
    got_error++;
  } else {
    t_error("unexpected recvfrom result %d (errno = %d)\n", (int)n, errno);
  }
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

  // A separate sender socket; `fd` gets connected to this sender's address so
  // that datagrams keep flowing after the reconnect filter kicks in.
  int sender = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(sender >= 0);
  struct sockaddr_in sender_addr;
  socklen_t sender_addr_len = sizeof(sender_addr);
  memset(&sender_addr, 0, sizeof(sender_addr));
  sender_addr.sin_family = AF_INET;
  sender_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sender_addr.sin_port = 0;
  TEST(bind(sender, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) == 0);
  TEST(getsockname(sender, (struct sockaddr *)&sender_addr, &sender_addr_len) ==
       0);

#define SEND()                                                                 \
  TEST(sendto(sender, "hi", 2, 0, (struct sockaddr *)&addr, sizeof(addr)) == 2)

  for (int iter = 0; iter < 20; iter++) {
    int readers_before = got_datagram + got_error;

    // Drain any datagrams left over from the previous iteration
    char drain[32];
    while (recvfrom(fd, drain, sizeof(drain), MSG_DONTWAIT, NULL, NULL) >= 0) {
      // ..
    }

    // Race two readers
    pthread_t r1, r2;
    TEST(pthread_create(&r1, NULL, receiver, NULL) == 0);
    for (int i = 0; i < 20; i++)
      TEST(sched_yield() == 0);
    TEST(pthread_create(&r2, NULL, receiver, NULL) == 0);
    for (int i = 0; i < 20; i++)
      sched_yield();

    // Complete a reader, and then race a reconnect against the other reader.
    SEND();
    struct sockaddr_in target = sender_addr;
    if (iter % 2)
      target.sin_port = 0;
    int rc;
    do {
      errno = 0;
      rc = connect(fd, (struct sockaddr *)&target, sizeof(target));
      if (rc == -1 && errno != EOPNOTSUPP && iter % 2 == 0)
        t_error("unexpected connect failure (errno = %d)\n", errno);
      TEST(sched_yield() == 0);
    } while (rc == -1 && errno == EOPNOTSUPP);

    // Unblock both readers now.
    for (int i = 0; i < 1000 && got_datagram + got_error < readers_before + 2;
         i++) {
      SEND();
      for (int j = 0; j < 100 && got_datagram + got_error < readers_before + 2;
           j++)
        sched_yield();
    }

    TEST(pthread_join(r1, NULL) == 0);
    TEST(pthread_join(r2, NULL) == 0);
    TEST(got_datagram + got_error == readers_before + 2);
  }

  TEST(close(sender) == 0);
  TEST(close(fd) == 0);
  return t_status;
}
