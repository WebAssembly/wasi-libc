#include <sys/socket.h>
#include <__struct_sockaddr_in.h>
#include <__struct_sockaddr_in6.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t sendto(int socket, const void *restrict buf, size_t len, int flags, const struct sockaddr *restrict addr, socklen_t addrlen) {
	errno = EOPNOTSUPP;
  	return -1;
}
