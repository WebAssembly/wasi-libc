#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int listen(int socket, int backlog) {
	errno = EOPNOTSUPP;
  	return -1;
}
