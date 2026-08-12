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

// Many blocking UDP recv/send cycles. On wasip3, each blocking wait that
// completes a pending subtask must drop that subtask handle. Clearing the
// handle without drop leaks component-model resources across iterations.
// The server blocks in recvfrom before each client send so the receive
// subtask is often still outstanding when the wait completes.
#define NROUNDS 64
#define BUFSIZE 256

void run_udp_server(void) {
  int server_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  TEST(server_socket_fd != -1);
  if (server_socket_fd == -1)
    return;

  struct sockaddr_in server_address;
  socklen_t server_address_len = sizeof(server_address);
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = 0;
  TEST(bind(server_socket_fd, (struct sockaddr *)&server_address,
            sizeof(server_address)) != -1);
  if (t_status != 0)
    return;

  TEST(getsockname(server_socket_fd, (struct sockaddr *)&server_address,
                   &server_address_len) != -1);

  printf("%d\n", ntohs(server_address.sin_port));
  fflush(stdout);

  for (int i = 0; i < NROUNDS; i++) {
    struct sockaddr_in client_address;
    socklen_t address_len = sizeof(client_address);
    char buffer[BUFSIZE];
    int bytes_read = recvfrom(server_socket_fd, buffer, BUFSIZE, 0,
                              (struct sockaddr *)&client_address, &address_len);
    TEST(bytes_read > 0);
    if (bytes_read <= 0)
      break;
    TEST(sendto(server_socket_fd, buffer, bytes_read, 0,
                (struct sockaddr *)&client_address, address_len) == bytes_read);
  }

  close(server_socket_fd);
}

int main(void) {
  run_udp_server();
  return t_status;
}
