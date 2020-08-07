//! POSIX-like functions supporting absolute path arguments, implemented in
//! terms of `__wasilibc_find_relpath` and `*at`-style functions.

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#include <wasi/libc.h>
#include <wasi/libc-find-relpath.h>

int __wasilibc_find_relpath_alloc(
    const char *path,
    const char **abs,
    char **relative,
    size_t *relative_len,
    int can_realloc
) __attribute__((weak));

static int find_relpath2(
    const char *path,
    char **relative,
    size_t *relative_len
) {
    // See comments in `preopens.c` for what this trick is doing.
    const char *abs;
    if (__wasilibc_find_relpath_alloc)
        return __wasilibc_find_relpath_alloc(path, &abs, relative, relative_len, 1);
    return __wasilibc_find_relpath(path, &abs, relative, *relative_len);
}

// Helper to call `__wasilibc_find_relpath` and return an already-managed
// pointer for the `relative` path. This function is not reentrant since the
// `relative` pointer will point to static data that cannot be reused until
// `relative` is no longer used.
static int find_relpath(const char *path, char **relative) {
    static __thread char *relative_buf = NULL;
    static __thread size_t relative_buf_len = 0;
    *relative = relative_buf;
    return find_relpath2(path, relative, &relative_buf_len);
}

int open(const char *path, int oflag, ...) {
    // WASI libc's `openat` ignores the mode argument, so call a special
    // entrypoint which avoids the varargs calling convention.
    return __wasilibc_open_nomode(path, oflag);
}

// See the documentation in libc.h
int __wasilibc_open_nomode(const char *path, int oflag) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return __wasilibc_openat_nomode(dirfd, relative_path, oflag);
}

int access(const char *path, int amode) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return faccessat(dirfd, relative_path, amode, 0);
}

ssize_t readlink(
    const char *restrict path,
    char *restrict buf,
    size_t bufsize)
{
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return readlinkat(dirfd, relative_path, buf, bufsize);
}

int stat(const char *restrict path, struct stat *restrict buf) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return fstatat(dirfd, relative_path, buf, 0);
}

int lstat(const char *restrict path, struct stat *restrict buf) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return fstatat(dirfd, relative_path, buf, AT_SYMLINK_NOFOLLOW);
}

int utime(const char *path, const struct utimbuf *times) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return utimensat(dirfd, relative_path,
                     times ? ((struct timespec [2]) {
                                 { .tv_sec = times->actime },
                                 { .tv_sec = times->modtime }
                             })
                           : NULL,
                     0);
}

int unlink(const char *path) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    // `unlinkat` imports `__wasi_path_remove_directory` even when
    // `AT_REMOVEDIR` isn't passed. Instead, use a specialized function which
    // just imports `__wasi_path_unlink_file`.
    return __wasilibc_unlinkat(dirfd, relative_path);
}

int rmdir(const char *path) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return __wasilibc_rmdirat(dirfd, relative_path);
}

int remove(const char *path) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    // First try to remove it as a file.
    int r = __wasilibc_unlinkat(dirfd, relative_path);
    if (r != 0 && (errno == EISDIR || errno == ENOTCAPABLE)) {
        // That failed, but it might be a directory.
        r = __wasilibc_rmdirat(dirfd, relative_path);

        // If it isn't a directory, we lack capabilities to remove it as a file.
        if (errno == ENOTDIR)
            errno = ENOTCAPABLE;
    }
    return r;
}

int mkdir(const char *path, mode_t mode) {
    char *relative_path;
    int dirfd = find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return mkdirat(dirfd, relative_path, mode);
}

DIR *opendir(const char *dirname) {
    char *relative_path;
    int dirfd = find_relpath(dirname, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return NULL;
    }

    return opendirat(dirfd, relative_path);
}

int scandir(
    const char *restrict dir,
    struct dirent ***restrict namelist,
    int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **)
) {
    char *relative_path;
    int dirfd = find_relpath(dir, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return scandirat(dirfd, relative_path, namelist, filter, compar);
}

int symlink(const char *target, const char *linkpath) {
    char *relative_path;
    int dirfd = find_relpath(linkpath, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return symlinkat(target, dirfd, relative_path);
}

int link(const char *old, const char *new) {
    static __thread char *old_relative_buf2 = NULL;
    static __thread size_t old_relative_buf_len2 = 0;
    char *old_relative_path;
    int old_dirfd = find_relpath2(old, &old_relative_path, &old_relative_buf_len2);

    if (old_dirfd != -1) {
        char *new_relative_path;
        int new_dirfd = find_relpath(new, &new_relative_path);

        if (new_dirfd != -1)
            return linkat(old_dirfd, old_relative_path,
                          new_dirfd, new_relative_path, 0);
    }

    // We couldn't find a preopen for it; indicate that we lack capabilities.
    errno = ENOTCAPABLE;
    return -1;
}

int rename(const char *old, const char *new) {
    static __thread char *old_relative_buf2 = NULL;
    static __thread size_t old_relative_buf_len2 = 0;
    char *old_relative_path = old_relative_buf2;
    int old_dirfd = find_relpath2(old, &old_relative_path, &old_relative_buf_len2);

    if (old_dirfd != -1) {
        char *new_relative_path;
        int new_dirfd = find_relpath(new, &new_relative_path);

        if (new_dirfd != -1)
            return renameat(old_dirfd, old_relative_path,
                            new_dirfd, new_relative_path);
    }

    // We couldn't find a preopen for it; indicate that we lack capabilities.
    errno = ENOTCAPABLE;
    return -1;
}
