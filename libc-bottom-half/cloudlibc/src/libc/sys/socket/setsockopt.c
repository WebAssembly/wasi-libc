#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int setsockopt(int socket, int level, int name, const void *restrict value, socklen_t option_len) {
	errno = EOPNOTSUPP;
  	return -1;
}
