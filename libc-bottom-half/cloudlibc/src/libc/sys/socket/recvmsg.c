#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t recvmsg(int socket, struct msghdr *restrict msg, int flags) {
	errno = EOPNOTSUPP;
  	return -1;
}
