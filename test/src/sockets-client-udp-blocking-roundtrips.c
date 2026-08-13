#include "test.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
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

// Pair with sockets-server-udp-blocking-roundtrips.c. Many blocking send/recv
// cycles exercise wasip3 subtask completion on both directions. On wasip3 CI
// this runs under -Smax-resources=128 so missing subtask drops fail the
// resource table while correct drops stay green (see server file comment).
#define NROUNDS 200
#define BUFSIZE 256

void test_udp_client(int server_port) {
  int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(socket_fd != -1);
  if (socket_fd == -1)
    return;

  struct sockaddr_in sockaddr_in;
  sockaddr_in.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  sockaddr_in.sin_family = AF_INET;
  sockaddr_in.sin_port = htons(server_port);

  TEST(connect(socket_fd, (struct sockaddr *)&sockaddr_in,
               sizeof(sockaddr_in)) != -1);

  for (int i = 0; i < NROUNDS; i++) {
    char message[64];
    int len = snprintf(message, sizeof(message), "roundtrip-%d", i);
    TEST(len > 0 && len < (int)sizeof(message));

    char client_buffer[BUFSIZE];
    TEST(send(socket_fd, message, len, 0) == len);
    int bytes_received = recv(socket_fd, client_buffer, BUFSIZE, 0);
    TEST(bytes_received == len);
    if (bytes_received == len)
      TEST(memcmp(message, client_buffer, len) == 0);
  }

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
  test_udp_client(port);
  return t_status;
}
