#include <common/errno.h>
#include <wasi/core.h>
#include <errno.h>
#include <string.h>

int __wasilibc_rmdirat(int fd, const char *path) {
    size_t path_len = strlen(path);
    __wasi_errno_t error = __wasi_path_unlink_directory(fd, path, path_len);
    if (error != 0) {
        errno = errno_fixup_directory(fd, error);
        return -1;
    }
    return 0;
}
