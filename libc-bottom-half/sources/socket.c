#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/socket.h>

static_assert((SOCK_DGRAM & ~SOCK_NONBLOCK & ~SOCK_CLOEXEC) == SOCK_DGRAM,
              "Socket type flags clash");
static_assert((SOCK_STREAM & ~SOCK_NONBLOCK & ~SOCK_CLOEXEC) == SOCK_STREAM,
              "Socket type flags clash");

static int socketcall(int domain, int type, int protocol) {
    // FIXME: Implement socket by looking up pre-opened network sockets.
    errno = ENOSYS;
    return -1;
}

int socket(int domain, int type, int protocol)
{
    int s = socketcall(domain, type & ~SOCK_CLOEXEC & ~SOCK_NONBLOCK, protocol);
    if (s < 0) return s;
    if (type & SOCK_CLOEXEC)  fcntl(s, F_SETFD, FD_CLOEXEC);
    if (type & SOCK_NONBLOCK) fcntl(s, F_SETFL, O_NONBLOCK);
    return s;
}
