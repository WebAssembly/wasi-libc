#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int pipe2(int fd[2], int flag)
{
	int fd1;
	int fd2;
    __wasi_errno_t error = __wasi_fd_pipe(&fd1, &fd2);
    if (error != 0) {
        errno = error;
        return -1;
    }
	fd[0] = fd1;
	fd[1] = fd2;
    return 0;
}
