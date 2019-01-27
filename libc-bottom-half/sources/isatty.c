#include <wasi.h>
#include <__errno.h>
#include <__function___isatty.h>

int __isatty(int fd) {
    wasi_fdstat_t statbuf;
    int r = wasi_fd_stat_get(fd, &statbuf);
    if (r != 0) {
        errno = r;
        return 0;
    }

    // A tty is a character device that we can't seek or tell on.
    if (statbuf.fs_filetype != WASI_FILETYPE_CHARACTER_DEVICE ||
        (statbuf.fs_rights_base & (WASI_RIGHT_FD_SEEK | WASI_RIGHT_FD_TELL)) != 0) {
        errno = WASI_ENOTTY;
        return 0;
    }

    return 1;
}
extern __typeof(__isatty) isatty __attribute__((weak, alias("__isatty")));
