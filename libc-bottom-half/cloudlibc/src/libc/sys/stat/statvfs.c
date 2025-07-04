#include <errno.h>
#include <sys/statvfs.h>

int statvfs(const char *restrict path, struct statvfs *restrict buf)
{
    errno = ENOTSUP;
    return -1;
}

int fstatvfs(int fd, struct statvfs *buf)
{
    errno = ENOTSUP;
    return -1;
}