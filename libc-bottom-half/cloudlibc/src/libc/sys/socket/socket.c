#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int socket(int domain, int ty, int protocol) {
	errno = EOPNOTSUPP;
  	return -1;
}
