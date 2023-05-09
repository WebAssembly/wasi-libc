#define _GNU_SOURCE

#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>

struct hostent *gethostbyname(const char *name)
{
#ifdef __wasilibc_unmodified_upstream
  return gethostbyname2(name, AF_INET);
#else
  errno = ENOTSUP;
  return 0;
#endif
}
