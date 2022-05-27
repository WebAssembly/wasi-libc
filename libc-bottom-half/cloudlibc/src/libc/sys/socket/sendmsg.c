#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

ssize_t sendmsg(int socket, const struct msghdr* msg, int flags) {
	errno = EOPNOTSUPP;
  	return -1;
}
