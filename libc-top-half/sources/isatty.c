#include <sys/stat.h>
#include <errno.h>
#include <__function___isatty.h>

int __isatty(int fd) {
    struct stat statbuf;
    if (fstat(fd, &statbuf) != 0) {
        // errno is already set
        return 0;
    }
    if (!S_ISCHR(statbuf.st_mode)) {
        errno = ENOTTY;
        return 0;
    }
    return 1;
}
weak_alias(__isatty, isatty);
