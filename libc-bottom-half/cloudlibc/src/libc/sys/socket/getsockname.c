#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int getsockname(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len) {
	errno = EOPNOTSUPP;
  	return -1;
}
