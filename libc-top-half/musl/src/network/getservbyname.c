#define _GNU_SOURCE
#include <errno.h>
#include <netdb.h>

struct servent *getservbyname(const char *name, const char *prots)
{
#ifdef __wasilibc_unmodified_upstream
	static struct servent se;
	static char *buf[2];
	struct servent *res;
	if (getservbyname_r(name, prots, &se, (void *)buf, sizeof buf, &res))
		return 0;
	return &se;
#else
  errno = ENOTSUP;
  return 0;
#endif
}
