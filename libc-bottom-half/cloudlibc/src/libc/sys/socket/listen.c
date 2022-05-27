#include <common/errno.h>
#include <common/net.h>
#include <sys/socket.h>

#include <assert.h>
#include <wasi/api.h>
#include <errno.h>
#include <string.h>

int listen(int socket, int backlog) {
  __wasi_errno_t error = __wasi_sock_listen(socket, backlog);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }

  return 0;
}
