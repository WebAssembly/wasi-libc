#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int socketpair(int domain, int ty, int protocol, int *restrict socket_vector) {
	errno = EOPNOTSUPP;
  	return -1;
}
