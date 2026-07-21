// Resolver-path `getaddrinfo` behavior: socktype filtering, service names,
// and AI_CANONNAME for resolved (non-numeric) hosts. Only "localhost" is
// resolved so no external DNS is required.
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include "test.h"

#define TEST(c)                                                                \
  do {                                                                         \
    if (!(c))                                                                  \
      t_error("%s failed\n", #c);                                              \
  } while (0)

static in_port_t port_of(const struct addrinfo *ai) {
  if (ai->ai_family == AF_INET)
    return ((struct sockaddr_in *)ai->ai_addr)->sin_port;
  return ((struct sockaddr_in6 *)ai->ai_addr)->sin6_port;
}

int main(void) {
  struct addrinfo hints, *res;

  // Baseline resolution with AI_CANONNAME: every entry has the port, the
  // first entry carries the canonical name.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  TEST(getaddrinfo("localhost", "8080", &hints, &res) == 0);
  TEST(res != NULL);
  TEST(res->ai_canonname != NULL);
  TEST(strcmp(res->ai_canonname, "localhost") == 0);
  for (struct addrinfo *p = res; p; p = p->ai_next) {
    TEST(p->ai_socktype == SOCK_STREAM);
    TEST(p->ai_protocol == IPPROTO_TCP);
    TEST(port_of(p) == htons(8080));
  }
  freeaddrinfo(res);

  // SOCK_DGRAM hint filters to datagram entries only.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  TEST(getaddrinfo("localhost", "53", &hints, &res) == 0);
  for (struct addrinfo *p = res; p; p = p->ai_next) {
    TEST(p->ai_socktype == SOCK_DGRAM);
    TEST(p->ai_protocol == IPPROTO_UDP);
  }
  freeaddrinfo(res);

  // Unspecified socktype with a numeric port yields both stream and dgram
  // entries.
  memset(&hints, 0, sizeof(hints));
  TEST(getaddrinfo("localhost", "80", &hints, &res) == 0);
  int stream = 0, dgram = 0;
  for (struct addrinfo *p = res; p; p = p->ai_next) {
    if (p->ai_socktype == SOCK_STREAM)
      stream = 1;
    if (p->ai_socktype == SOCK_DGRAM)
      dgram = 1;
  }
  TEST(stream && dgram);
  freeaddrinfo(res);

  // A TCP-only service name filtered to SOCK_DGRAM has no valid entries.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_DGRAM;
  TEST(getaddrinfo("localhost", "ftp", &hints, &res) == EAI_SERVICE);

  // A service name resolves to its port.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  TEST(getaddrinfo("localhost", "ftp", &hints, &res) == 0);
  for (struct addrinfo *p = res; p; p = p->ai_next) {
    TEST(p->ai_socktype == SOCK_STREAM);
    TEST(port_of(p) == htons(21));
  }
  freeaddrinfo(res);

  return t_status;
}
