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

int main() {
  int socket1 = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(socket1 != -1);
  int socket2 = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(socket2 != -1);

  struct sockaddr_in addr1;
  addr1.sin_family = AF_INET;
  addr1.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr1.sin_port = htons(0);
  TEST(bind(socket1, (struct sockaddr *)&addr1, sizeof(addr1)) == 0);

  socklen_t addrlen = sizeof(addr1);
  TEST(getsockname(socket1, (struct sockaddr *)&addr1, &addrlen) == 0);

  TEST(connect(socket2, (struct sockaddr *)&addr1, sizeof(addr1)) == 0);

  char message[] = "Hello, world!";
  char buffer[256];

  // send/recv work
  TEST(send(socket2, message, sizeof(message), 0) == sizeof(message));
  TEST(recv(socket1, buffer, sizeof(buffer), 0) == sizeof(message));
  TEST(memcmp(message, buffer, sizeof(message)) == 0);

  // read/write work
  TEST(write(socket2, message, sizeof(message)) == sizeof(message));
  TEST(read(socket1, buffer, sizeof(buffer)) == sizeof(message));
  TEST(memcmp(message, buffer, sizeof(message)) == 0);

  // sendto doesn't work (allowed by posix apparently?) but recfrom does
  TEST(sendto(socket2, message, sizeof(message), 0, (struct sockaddr *)&addr1,
              addrlen) == -1 &&
       errno == EISCONN);
  TEST(write(socket2, message, sizeof(message)) == sizeof(message));
  TEST(recvfrom(socket1, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr1,
                &addrlen) == sizeof(message));

  TEST(close(socket1) == 0);
  TEST(close(socket2) == 0);

  return t_status;
}
