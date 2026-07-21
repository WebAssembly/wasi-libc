// Numeric and local-only `getaddrinfo` paths. This test deliberately runs
// WITHOUT any network capabilities granted: numeric literals, NULL-host
// wildcard/loopback synthesis, and flag validation must all work without
// touching the resolver.
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

static size_t count(const struct addrinfo *res) {
  size_t n = 0;
  for (; res; res = res->ai_next)
    n++;
  return n;
}

static in_addr_t v4_of(const struct addrinfo *ai) {
  return ((struct sockaddr_in *)ai->ai_addr)->sin_addr.s_addr;
}

static in_port_t port_of(const struct addrinfo *ai) {
  if (ai->ai_family == AF_INET)
    return ((struct sockaddr_in *)ai->ai_addr)->sin_port;
  return ((struct sockaddr_in6 *)ai->ai_addr)->sin6_port;
}

int main(void) {
  struct addrinfo hints, *res;

  // IPv4 literal, numeric port, stream-only.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  TEST(getaddrinfo("127.0.0.1", "8080", &hints, &res) == 0);
  TEST(count(res) == 1);
  TEST(res->ai_family == AF_INET);
  TEST(res->ai_socktype == SOCK_STREAM);
  TEST(res->ai_protocol == IPPROTO_TCP);
  TEST(res->ai_addrlen == sizeof(struct sockaddr_in));
  TEST(v4_of(res) == htonl(INADDR_LOOPBACK));
  TEST(port_of(res) == htons(8080));
  freeaddrinfo(res);

  // IPv6 literal, dgram-only.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  TEST(getaddrinfo("::1", "53", &hints, &res) == 0);
  TEST(count(res) == 1);
  TEST(res->ai_family == AF_INET6);
  TEST(res->ai_socktype == SOCK_DGRAM);
  TEST(res->ai_protocol == IPPROTO_UDP);
  TEST(res->ai_addrlen == sizeof(struct sockaddr_in6));
  TEST(memcmp(&((struct sockaddr_in6 *)res->ai_addr)->sin6_addr,
              &in6addr_loopback, sizeof(struct in6_addr)) == 0);
  TEST(port_of(res) == htons(53));
  freeaddrinfo(res);

  // Literal of the wrong family is not a match.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == EAI_NONAME);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  TEST(getaddrinfo("::1", "80", &hints, &res) == EAI_NONAME);

  // Unspecified socktype with a numeric port gives one stream and one dgram
  // entry per address.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == 0);
  TEST(count(res) == 2);
  TEST(res->ai_socktype == SOCK_STREAM);
  TEST(res->ai_protocol == IPPROTO_TCP);
  TEST(res->ai_next->ai_socktype == SOCK_DGRAM);
  TEST(res->ai_next->ai_protocol == IPPROTO_UDP);
  TEST(v4_of(res) == v4_of(res->ai_next));
  freeaddrinfo(res);

  // NULL host + AI_PASSIVE = wildcard.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  TEST(getaddrinfo(NULL, "80", &hints, &res) == 0);
  TEST(count(res) == 1);
  TEST(v4_of(res) == htonl(INADDR_ANY));
  TEST(port_of(res) == htons(80));
  freeaddrinfo(res);

  // NULL host without AI_PASSIVE = loopback.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  TEST(getaddrinfo(NULL, "80", &hints, &res) == 0);
  TEST(count(res) == 1);
  TEST(v4_of(res) == htonl(INADDR_LOOPBACK));
  freeaddrinfo(res);

  // NULL host + AF_UNSPEC yields both families.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  TEST(getaddrinfo(NULL, "80", &hints, &res) == 0);
  int saw4 = 0, saw6 = 0;
  for (struct addrinfo *p = res; p; p = p->ai_next) {
    if (p->ai_family == AF_INET)
      saw4 = 1;
    if (p->ai_family == AF_INET6)
      saw6 = 1;
  }
  TEST(saw4 && saw6);
  freeaddrinfo(res);

  // Both host and serv NULL is an error.
  TEST(getaddrinfo(NULL, NULL, NULL, &res) == EAI_NONAME);

  // AI_NUMERICHOST rejects a non-numeric host without consulting the
  // resolver (this test has no name-lookup permission).
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICHOST;
  TEST(getaddrinfo("localhost", "80", &hints, &res) == EAI_NONAME);

  // AI_NUMERICSERV rejects a non-numeric service.
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_NUMERICSERV;
  TEST(getaddrinfo("127.0.0.1", "http", &hints, &res) == EAI_NONAME);

  // Undefined flag bits are rejected.
  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = 0x8000;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == EAI_BADFLAGS);

  // Unsupported family/socktype are rejected.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = 99;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == EAI_FAMILY);
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = 99;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == EAI_SOCKTYPE);

  // AI_CANONNAME fills in the canonical name; freeaddrinfo must free it.
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == 0);
  TEST(res->ai_canonname != NULL);
  TEST(strcmp(res->ai_canonname, "127.0.0.1") == 0);
  freeaddrinfo(res);

  // Common glibc-isms are accepted (and ignored).
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG;
  TEST(getaddrinfo("127.0.0.1", "80", &hints, &res) == 0);
  freeaddrinfo(res);

  return t_status;
}
