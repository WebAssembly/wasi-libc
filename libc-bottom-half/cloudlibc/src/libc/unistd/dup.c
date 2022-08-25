#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int dup(int fd) {
    int newfd = 0;
    __wasi_errno_t error = __wasi_fd_dup(fd, &newfd);
    if (error != 0) {
        errno = error;
        return -1;
    }
    return newfd;
}
