#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int dup2(int fd, int newfd) {
    __wasi_errno_t error = __wasi_fd_renumber(fd, newfd);
    if (error != 0) {
        errno = error;
        return -1;
    }
    return 0;
}
