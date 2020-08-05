#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/libc-find-relpath.h>
#include <wasi/libc.h>

extern char *__wasilibc_cwd;
static int __wasilibc_cwd_mallocd = 0;

int chdir(const char *path)
{
    static char *relative_buf = NULL;
    static size_t relative_buf_len = 0;

    // Find a preopen'd directory as well as a relative path we're anchored
    // from which we're changing directories to.
    const char *abs;
    int parent_fd;
    while (1) {
        parent_fd = __wasilibc_find_relpath(path, &abs, relative_buf, relative_buf_len);
        if (parent_fd != -1)
            break;
        if (errno != ERANGE)
            return -1;
        size_t new_len = relative_buf_len == 0 ? 16 : 2 * relative_buf_len;
        relative_buf = realloc(relative_buf, new_len);
        if (relative_buf == NULL) {
            errno = ENOMEM;
            return -1;
        }
        relative_buf_len = new_len;
    }

    // Make sure that this directory we're accessing is indeed a directory.
    struct stat dirinfo;
    int ret = fstatat(parent_fd, relative_buf, &dirinfo, 0);
    if (ret == -1) {
        return -1;
    }
    if (!S_ISDIR(dirinfo.st_mode)) {
        errno = ENOTDIR;
        return -1;
    }

    // Copy over our new absolute path into `__wasilibc_cwd`. Only copy over
    // the relative portion of the path if it's not equal to `.`
    size_t len = strlen(abs);
    int copy_relative = strcmp(relative_buf, ".") != 0;
    char *new_cwd = malloc(len + (copy_relative ? strlen(relative_buf) : 0));
    if (new_cwd == NULL) {
        errno = ENOMEM;
        return -1;
    }
    strcpy(new_cwd, abs);
    if (copy_relative)
        strcpy(new_cwd + strlen(abs), relative_buf);

    // And set our new malloc'd buffer into the global cwd, freeing the
    // previous one if necessary.
    char *prev_cwd = __wasilibc_cwd;
    __wasilibc_cwd = new_cwd;
    if (__wasilibc_cwd_mallocd)
        free(prev_cwd);
    __wasilibc_cwd_mallocd = 1;
    return 0;
}
