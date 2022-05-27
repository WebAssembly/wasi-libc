#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int connect(int socket, const struct sockaddr *restrict address, socklen_t len) {
	errno = EOPNOTSUPP;
  	return -1;
}
