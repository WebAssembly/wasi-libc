#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int truncate(const char *path, off_t length)
{
    int fd = open(path, O_WRONLY | O_CLOEXEC | O_NOCTTY);
    if (fd < 0)
        return -1;

    int result = ftruncate(fd, length);
    if (result < 0) {
        int save_errno = errno;
        (void)close(fd);
        errno = save_errno;
        return -1;
    }

    if (close(fd) < 0)
        return -1;

    return 0;
}
