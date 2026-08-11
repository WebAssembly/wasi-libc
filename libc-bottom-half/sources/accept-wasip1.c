// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <wasi/api.h>

int accept(int socket, struct sockaddr *restrict addr,
           socklen_t *restrict addrlen) {
  int ret = -1;

  __wasi_errno_t error = __wasi_sock_accept(socket, 0, &ret);

  if (error != 0) {
    errno = error;
    return -1;
  }

  // POSIX allows both addr and addrlen to be NULL when the peer address is
  // unused. wasip2/p3 accept paths use __wasilibc_sockaddr_validate for the
  // same both-null case.
  if (addr != NULL && addrlen != NULL) {
    // Clear sockaddr to indicate undefined address
    memset(addr, 0, *addrlen);
    // might be AF_UNIX or AF_INET
    addr->sa_family = AF_UNSPEC;
    *addrlen = sizeof(struct sockaddr);
  }

  return ret;
}

int accept4(int socket, struct sockaddr *restrict addr,
            socklen_t *restrict addrlen, int flags) {
  int ret = -1;

  if (flags & ~(SOCK_NONBLOCK | SOCK_CLOEXEC)) {
    errno = EINVAL;
    return -1;
  }

  __wasi_errno_t error = __wasi_sock_accept(
      socket, (flags & SOCK_NONBLOCK) ? __WASI_FDFLAGS_NONBLOCK : 0, &ret);

  if (error != 0) {
    errno = error;
    return -1;
  }

  if (addr != NULL && addrlen != NULL) {
    // Clear sockaddr to indicate undefined address
    memset(addr, 0, *addrlen);
    // might be AF_UNIX or AF_INET
    addr->sa_family = AF_UNSPEC;
    *addrlen = sizeof(struct sockaddr);
  }

  return ret;
}
