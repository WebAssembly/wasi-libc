#define _GNU_SOURCE
#include <netdb.h>
#include <errno.h>

struct servent *getservbyport(int port, const char *prots)
{
#ifdef __wasilibc_unmodified_upstream
	static struct servent se;
	static long buf[32/sizeof(long)];
	struct servent *res;
	if (getservbyport_r(port, prots, &se, (void *)buf, sizeof buf, &res))
		return 0;
	return &se;
#else
  errno = ENOTSUP;
  return 0;
#endif
}
