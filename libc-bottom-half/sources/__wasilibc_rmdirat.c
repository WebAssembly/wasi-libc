#include <common/errno.h>
#include <wasix/api.h>
#include <wasix/libc.h>
#include <errno.h>

int __wasilibc_nocwd___wasilibc_rmdirat(int fd, const char *path) {
    __wasi_errno_t error = __wasi_path_remove_directory(fd, path);
    if (error != 0) {
        errno = errno_fixup_directory(fd, error);
        return -1;
    }
    return 0;
}
