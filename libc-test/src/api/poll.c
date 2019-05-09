#include <poll.h>
#define T(t) (t*)0;
#define F(t,n) {t *y = &x.n;}
#define C(n) switch(n){case n:;}
static void f()
{
T(nfds_t)
{
struct pollfd x;
F(int, fd)
F(short, events)
F(short, revents)
}
C(POLLIN)
C(POLLRDNORM)
#ifdef __wasilibc_unmodified_upstream // WASI does not support out-of-band data on sockets
C(POLLRDBAND)
C(POLLPRI)
#endif
C(POLLOUT)
C(POLLWRNORM)
#ifdef __wasilibc_unmodified_upstream // WASI does not support out-of-band data on sockets
C(POLLWRBAND)
#endif
C(POLLERR)
C(POLLHUP)
C(POLLNVAL)
{int(*p)(struct pollfd[],nfds_t,int) = poll;}
}
