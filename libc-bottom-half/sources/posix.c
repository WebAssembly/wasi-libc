//! POSIX-like functions supporting absolute path arguments, implemented in
//! terms of `__wasilibc_find_relpath` and `*at`-style functions.

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <sys/stat.h>
#include <wasi/libc.h>
#include <wasi/libc-find-relpath.h>

int open(const char *path, int oflag, ...) {
    // WASI libc's `openat` ignores the mode argument, so call a special
    // entrypoint which avoids the varargs calling convention.
    return __wasilibc_open_nomode(path, oflag);
}

// See the documentation in libc.h
int __wasilibc_open_nomode(const char *path, int oflag) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return __wasilibc_openat_nomode(dirfd, relative_path, oflag);
}

int access(const char *path, int amode) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

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
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return readlinkat(dirfd, relative_path, buf, bufsize);
}

int stat(const char *restrict path, struct stat *restrict buf) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return fstatat(dirfd, relative_path, buf, 0);
}

int lstat(const char *restrict path, struct stat *restrict buf) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return fstatat(dirfd, relative_path, buf, AT_SYMLINK_NOFOLLOW);
}

int utime(const char *path, const struct utimbuf *times) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return utimensat(fd, relative_path, times ? ((struct timespec [2]) {
                                                    { .tv_sec = times->actime },
                                                    { .tv_sec = times->modtime }
                                                })
                                              : NULL,
                     0);
}

int unlink(const char *path) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

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
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return __wasilibc_rmdirat(dirfd, relative_path);
}

int remove(const char *path) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

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
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return mkdirat(dirfd, relative_path, mode);
}

DIR *opendir(const char *dirname) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(dirname, &relative_path);

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
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(dir, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return scandirat(dirfd, relative_path, namelist, filter, compar);
}

int symlink(const char *target, const char *linkpath) {
    const char *relative_path;
    int dirfd = __wasilibc_find_relpath(linkpath, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    return symlinkat(target, dirfd, relative_path);
}

int link(const char *old, const char *new) {
    const char *old_relative_path;
    int old_dirfd = __wasilibc_find_relpath(old, &old_relative_path);

    if (old_dirfd != -1) {
        const char *new_relative_path;
        int new_dirfd = __wasilibc_find_relpath(new, &new_relative_path);

        if (new_dirfd != -1)
            return linkat(old_dirfd, old_relative_path,
                          new_dirfd, new_relative_path, 0);
    }

    // We couldn't find a preopen for it; indicate that we lack capabilities.
    errno = ENOTCAPABLE;
    return -1;
}

int rename(const char *old, const char *new) {
    const char *old_relative_path;
    int old_dirfd = __wasilibc_find_relpath(old, &old_relative_path);

    if (old_dirfd != -1) {
        const char *new_relative_path;
        int new_dirfd = __wasilibc_find_relpath(new, &new_relative_path);

        if (new_dirfd != -1)
            return renameat(old_dirfd, old_relative_path,
                            new_dirfd, new_relative_path);
    }

    // We couldn't find a preopen for it; indicate that we lack capabilities.
    errno = ENOTCAPABLE;
    return -1;
}
