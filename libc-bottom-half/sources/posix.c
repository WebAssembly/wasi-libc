//! POSIX-like functions supporting absolute path arguments, implemented in
//! terms of `__wasilibc_find_relpath` and `*at`-style functions.

#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
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
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = __wasilibc_openat_nomode(dirfd, relative_path, oflag);
    free(relative_path);
    return ret;
}

int access(const char *path, int amode) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = faccessat(dirfd, relative_path, amode, 0);
    free(relative_path);
    return ret;
}

ssize_t readlink(
    const char *restrict path,
    char *restrict buf,
    size_t bufsize)
{
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    ssize_t ret = readlinkat(dirfd, relative_path, buf, bufsize);
    free(relative_path);
    return ret;
}

int stat(const char *restrict path, struct stat *restrict buf) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = fstatat(dirfd, relative_path, buf, 0);
    free(relative_path);
    return ret;
}

int lstat(const char *restrict path, struct stat *restrict buf) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = fstatat(dirfd, relative_path, buf, AT_SYMLINK_NOFOLLOW);
    free(relative_path);
    return ret;
}

int utime(const char *path, const struct utimbuf *times) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = utimensat(dirfd, relative_path,
                     times ? ((struct timespec [2]) {
                                 { .tv_sec = times->actime },
                                 { .tv_sec = times->modtime }
                             })
                           : NULL,
                     0);
    free(relative_path);
    return ret;
}

int unlink(const char *path) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    // `unlinkat` imports `__wasi_path_remove_directory` even when
    // `AT_REMOVEDIR` isn't passed. Instead, use a specialized function which
    // just imports `__wasi_path_unlink_file`.
    int ret = __wasilibc_unlinkat(dirfd, relative_path);
    free(relative_path);
    return ret;
}

int rmdir(const char *path) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = __wasilibc_rmdirat(dirfd, relative_path);
    free(relative_path);
    return ret;
}

int remove(const char *path) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

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
    free(relative_path);
    return r;
}

int mkdir(const char *path, mode_t mode) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(path, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = mkdirat(dirfd, relative_path, mode);
    free(relative_path);
    return ret;
}

DIR *opendir(const char *dirname) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(dirname, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return NULL;
    }

    DIR *ret = opendirat(dirfd, relative_path);
    free(relative_path);
    return ret;
}

int scandir(
    const char *restrict dir,
    struct dirent ***restrict namelist,
    int (*filter)(const struct dirent *),
    int (*compar)(const struct dirent **, const struct dirent **)
) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(dir, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = scandirat(dirfd, relative_path, namelist, filter, compar);
    free(relative_path);
    return ret;
}

int symlink(const char *target, const char *linkpath) {
    const char *abs;
    char *relative_path;
    int dirfd = __wasilibc_find_relpath(linkpath, &abs, &relative_path);

    // If we can't find a preopen for it, indicate that we lack capabilities.
    if (dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = symlinkat(target, dirfd, relative_path);
    free(relative_path);
    return ret;
}

int link(const char *old, const char *new) {
    const char *abs;
    char *old_relative_path;
    char *new_relative_path;

    int old_dirfd = __wasilibc_find_relpath(old, &abs, &old_relative_path);
    // We couldn't find a preopen for it; indicate that we lack capabilities.
    if (old_dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }
    int new_dirfd = __wasilibc_find_relpath(new, &abs, &new_relative_path);
    if (new_dirfd == -1) {
        free(old_relative_path);
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = linkat(old_dirfd, old_relative_path, new_dirfd, new_relative_path, 0);
    free(old_relative_path);
    free(new_relative_path);
    return ret;
}

int rename(const char *old, const char *new) {
    const char *abs;
    char *old_relative_path;
    char *new_relative_path;

    int old_dirfd = __wasilibc_find_relpath(old, &abs, &old_relative_path);
    // We couldn't find a preopen for it; indicate that we lack capabilities.
    if (old_dirfd == -1) {
        errno = ENOTCAPABLE;
        return -1;
    }
    int new_dirfd = __wasilibc_find_relpath(new, &abs, &new_relative_path);
    if (new_dirfd == -1) {
        free(old_relative_path);
        errno = ENOTCAPABLE;
        return -1;
    }

    int ret = renameat(old_dirfd, old_relative_path, new_dirfd, new_relative_path);
    free(old_relative_path);
    free(new_relative_path);
    return ret;
}
