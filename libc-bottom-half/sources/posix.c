//! POSIX-like functions supporting absolute path arguments, implemented in
//! terms of `__wasilibc_find_relpath` and `*at`-style functions.

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <utime.h>
#include <wasi/libc.h>

#if !defined(__webp2__)
#include <wasi/libc-find-relpath.h>
#include <wasi/libc-nocwd.h>
#else
#include <wasi/report-error.h>
#endif

#ifndef __webp2__
static int find_relpath2(const char *path, char **relative,
                         size_t *relative_len) {
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
  int fd = find_relpath2(path, &relative_buf, &relative_buf_len);
  // find_relpath2 can update relative_buf, so assign it after the call
  *relative = relative_buf;
  return fd;
}

// same as `find_relpath`, but uses another set of static variables to cache
static int find_relpath_alt(const char *path, char **relative) {
  static __thread char *relative_buf = NULL;
  static __thread size_t relative_buf_len = 0;
  int fd = find_relpath2(path, &relative_buf, &relative_buf_len);
  // find_relpath2 can update relative_buf, so assign it after the call
  *relative = relative_buf;
  return fd;
}
#endif // !webp2

int open(const char *path, int oflag, ...) {
  // WASI libc's `openat` ignores the mode argument, so call a special
  // entrypoint which avoids the varargs calling convention.
  return __wasilibc_open_nomode(path, oflag);
}

// See the documentation in libc.h
int __wasilibc_open_nomode(const char *path, int oflag) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_openat_nomode(dirfd, relative_path, oflag);
#else
  WASILIBC_LOG_UNSUPPORTED("open");
  errno = (oflag & O_CREAT) ? EACCES : ENOENT;
  return -1;
#endif
}

int access(const char *path, int amode) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_faccessat(dirfd, relative_path, amode, 0);
#else
  WASILIBC_LOG_UNSUPPORTED("access");
  errno = ENOENT;
  return -1;
#endif
}

ssize_t readlink(const char *restrict path, char *restrict buf,
                 size_t bufsize) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_readlinkat(dirfd, relative_path, buf, bufsize);
#else
  WASILIBC_LOG_UNSUPPORTED("readlink");
  errno = ENOENT;
  return -1;
#endif
}

int stat(const char *restrict path, struct stat *restrict buf) {
#if !defined(__webp2__)
  if (path[0] == '\0') {
    errno = ENOENT;
    return -1;
  }

  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_fstatat(dirfd, relative_path, buf, 0);
#else
  WASILIBC_LOG_UNSUPPORTED("stat");
  errno = ENOENT;
  return -1;
#endif
}

int lstat(const char *restrict path, struct stat *restrict buf) {
#if !defined(__webp2__)
  if (path[0] == '\0') {
    errno = ENOENT;
    return -1;
  }

  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_fstatat(dirfd, relative_path, buf,
                                  AT_SYMLINK_NOFOLLOW);
#else
  WASILIBC_LOG_UNSUPPORTED("lstat");
  errno = ENOENT;
  return -1;
#endif
}

int utime(const char *path, const struct utimbuf *times) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_utimensat(
      dirfd, relative_path,
      times ? ((struct timespec[2]){{.tv_sec = times->actime},
                                    {.tv_sec = times->modtime}})
            : NULL,
      0);
#else
  WASILIBC_LOG_UNSUPPORTED("utime");
  errno = ENOENT;
  return -1;
#endif
}

int utimes(const char *path, const struct timeval times[2]) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_utimensat(
      dirfd, relative_path,
      times
          ? ((struct timespec[2]){
                {.tv_sec = times[0].tv_sec, .tv_nsec = times[0].tv_usec * 1000},
                {.tv_sec = times[1].tv_sec, .tv_nsec = times[1].tv_usec * 1000},
            })
          : NULL,
      0);
#else
  WASILIBC_LOG_UNSUPPORTED("utimes");
  errno = ENOENT;
  return -1;
#endif
}

int unlink(const char *path) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  // `unlinkat` imports `__wasi_path_remove_directory` even when
  // `AT_REMOVEDIR` isn't passed. Instead, use a specialized function which
  // just imports `__wasi_path_unlink_file`.
  return __wasilibc_nocwd___wasilibc_unlinkat(dirfd, relative_path);
#else
  WASILIBC_LOG_UNSUPPORTED("unlink");
  errno = ENOENT;
  return -1;
#endif
}

int rmdir(const char *path) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd___wasilibc_rmdirat(dirfd, relative_path);
#else
  WASILIBC_LOG_UNSUPPORTED("rmdir");
  errno = ENOENT;
  return -1;
#endif
}

int remove(const char *path) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  // First try to remove it as a file.
  int r = __wasilibc_nocwd___wasilibc_unlinkat(dirfd, relative_path);
  if (r != 0 && (errno == EISDIR || errno == ENOENT || errno == EACCES ||
                 errno == EPERM)) {
    // That failed, but it might be a directory.
    r = __wasilibc_nocwd___wasilibc_rmdirat(dirfd, relative_path);

    // If it isn't a directory, we lack capabilities to remove it as a file.
    if (errno == ENOTDIR)
      errno = ENOENT;
  }
  return r;
#else
  WASILIBC_LOG_UNSUPPORTED("remove");
  errno = ENOENT;
  return -1;
#endif
}

int mkdir(const char *path, mode_t mode) {
#if !defined(__webp2__)
  (void)mode;
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_mkdirat_nomode(dirfd, relative_path);
#else
  WASILIBC_LOG_UNSUPPORTED("mkdir");
  errno = EACCES;
  return -1;
#endif
}

DIR *opendir(const char *dirname) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(dirname, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return NULL;
  }

  return __wasilibc_nocwd_opendirat(dirfd, relative_path);
#else
  WASILIBC_LOG_UNSUPPORTED("opendir");
  errno = ENOENT;
  return NULL;
#endif
}

int scandir(const char *restrict dir, struct dirent ***restrict namelist,
            int (*filter)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **)) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(dir, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_scandirat(dirfd, relative_path, namelist, filter,
                                    compar);
#else
  WASILIBC_LOG_UNSUPPORTED("scandir");
  errno = ENOENT;
  return -1;
#endif
}

int symlink(const char *target, const char *linkpath) {
#if !defined(__webp2__)
  char *relative_path;
  int dirfd = find_relpath(linkpath, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_symlinkat(target, dirfd, relative_path);
#else
  WASILIBC_LOG_UNSUPPORTED("symlink");
  errno = EACCES;
  return -1;
#endif
}

int link(const char *old, const char *new) {
#if !defined(__webp2__)
  char *old_relative_path;
  int old_dirfd = find_relpath_alt(old, &old_relative_path);

  if (old_dirfd != -1) {
    char *new_relative_path;
    int new_dirfd = find_relpath(new, &new_relative_path);

    if (new_dirfd != -1)
      return __wasilibc_nocwd_linkat(old_dirfd, old_relative_path, new_dirfd,
                                     new_relative_path, 0);
  }

  // We couldn't find a preopen for it; fail as if we can't find the path.
  errno = ENOENT;
  return -1;
#else
  WASILIBC_LOG_UNSUPPORTED("link");
  errno = ENOENT;
  return -1;
#endif
}

int rename(const char *old, const char *new) {
#if !defined(__webp2__)
  char *old_relative_path;
  int old_dirfd = find_relpath_alt(old, &old_relative_path);

  if (old_dirfd != -1) {
    char *new_relative_path;
    int new_dirfd = find_relpath(new, &new_relative_path);

    if (new_dirfd != -1)
      return __wasilibc_nocwd_renameat(old_dirfd, old_relative_path, new_dirfd,
                                       new_relative_path);
  }

  // We couldn't find a preopen for it; fail as if we can't find the path.
  errno = ENOENT;
  return -1;
#else
  WASILIBC_LOG_UNSUPPORTED("rename");
  errno = ENOENT;
  return -1;
#endif
}

int chmod(const char *path, mode_t mode) {
  (void)path;
  (void)mode;
  // TODO: We plan to support this eventually in WASI, but not yet.
  // Meanwhile, we provide a stub so that libc++'s `<filesystem>`
  // implementation will build unmodified.
  WASILIBC_LOG_UNSUPPORTED("chmod");
  errno = ENOSYS;
  return -1;
}

int fchmod(int fd, mode_t mode) {
  (void)fd;
  (void)mode;
  // TODO: We plan to support this eventually in WASI, but not yet.
  // Meanwhile, we provide a stub so that libc++'s `<filesystem>`
  // implementation will build unmodified.
  WASILIBC_LOG_UNSUPPORTED("fchmod");
  errno = ENOSYS;
  return -1;
}

int fchmodat(int fd, const char *path, mode_t mode, int flag) {
  (void)fd;
  (void)path;
  (void)mode;
  (void)flag;
  // TODO: We plan to support this eventually in WASI, but not yet.
  // Meanwhile, we provide a stub so that libc++'s `<filesystem>`
  // implementation will build unmodified.
  WASILIBC_LOG_UNSUPPORTED("fchmodat");
  errno = ENOSYS;
  return -1;
}

int statvfs(const char *__restrict path, struct statvfs *__restrict buf) {
  (void)path;
  (void)buf;
  // TODO: We plan to support this eventually in WASI, but not yet.
  // Meanwhile, we provide a stub so that libc++'s `<filesystem>`
  // implementation will build unmodified.
  WASILIBC_LOG_UNSUPPORTED("statvfs");
  errno = ENOSYS;
  return -1;
}

int fstatvfs(int fd, struct statvfs *buf) {
  (void)fd;
  (void)buf;
  // TODO: We plan to support this eventually in WASI, but not yet.
  // Meanwhile, we provide a stub so that libc++'s `<filesystem>`
  // implementation will build unmodified.
  WASILIBC_LOG_UNSUPPORTED("fstatvfs");
  errno = ENOSYS;
  return -1;
}

#if !defined(__webp2__)
// Like `access`, but with `faccessat`'s flags argument.
int __wasilibc_access(const char *path, int mode, int flags) {
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_faccessat(dirfd, relative_path, mode, flags);
}

// Like `utimensat`, but without the `at` part.
int __wasilibc_utimens(const char *path, const struct timespec times[2],
                       int flags) {
  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_utimensat(dirfd, relative_path, times, flags);
}

// Like `stat`, but with `fstatat`'s flags argument.
int __wasilibc_stat(const char *__restrict path, struct stat *__restrict st,
                    int flags) {
  if (path[0] == '\0') {
    errno = ENOENT;
    return -1;
  }

  char *relative_path;
  int dirfd = find_relpath(path, &relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_fstatat(dirfd, relative_path, st, flags);
}

// Like `link`, but with `linkat`'s flags argument.
int __wasilibc_link(const char *oldpath, const char *newpath, int flags) {
  char *old_relative_path;
  char *new_relative_path;
  int old_dirfd = find_relpath(oldpath, &old_relative_path);
  int new_dirfd = find_relpath_alt(newpath, &new_relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (old_dirfd == -1 || new_dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_linkat(old_dirfd, old_relative_path, new_dirfd,
                                 new_relative_path, flags);
}

// Like `__wasilibc_link`, but oldpath is relative to olddirfd.
int __wasilibc_link_oldat(int olddirfd, const char *oldpath,
                          const char *newpath, int flags) {
  char *new_relative_path;
  int new_dirfd = find_relpath(newpath, &new_relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (new_dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_linkat(olddirfd, oldpath, new_dirfd,
                                 new_relative_path, flags);
}

// Like `__wasilibc_link`, but newpath is relative to newdirfd.
int __wasilibc_link_newat(const char *oldpath, int newdirfd,
                          const char *newpath, int flags) {
  char *old_relative_path;
  int old_dirfd = find_relpath(oldpath, &old_relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (old_dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_linkat(old_dirfd, old_relative_path, newdirfd,
                                 newpath, flags);
}

// Like `rename`, but from is relative to fromdirfd.
int __wasilibc_rename_oldat(int fromdirfd, const char *from, const char *to) {
  char *to_relative_path;
  int to_dirfd = find_relpath(to, &to_relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (to_dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_renameat(fromdirfd, from, to_dirfd, to_relative_path);
}

// Like `rename`, but to is relative to todirfd.
int __wasilibc_rename_newat(const char *from, int todirfd, const char *to) {
  char *from_relative_path;
  int from_dirfd = find_relpath(from, &from_relative_path);

  // If we can't find a preopen for it, fail as if we can't find the path.
  if (from_dirfd == -1) {
    errno = ENOENT;
    return -1;
  }

  return __wasilibc_nocwd_renameat(from_dirfd, from_relative_path, todirfd, to);
}
#endif // !webp2
