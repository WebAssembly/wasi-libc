// Handle AT_FDCWD and absolute paths for the *at functions.
//
// In the case of an AT_FDCWD file descriptor or an absolute path, call the
// corresponding non-`at` function. This will send it through the libpreopen
// wrappers to convert the path into a directory file descriptor and relative
// path before translating it into the corresponding `__wasilibc_nocwd_*at`
// function, which then calls the appropriate WASI function.

#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wasi/libc.h>

#if !defined(__webp2__)
#include <wasi/libc-nocwd.h>
#else
#include <wasi/report-error.h>
#endif

// If the platform doesn't define O_TMPFILE, we don't need to worry about it.
#ifndef O_TMPFILE
#define O_TMPFILE 0
#endif

int openat(int dirfd, const char *pathname, int flags, ...) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return open(pathname, flags);
  }

  return __wasilibc_nocwd_openat_nomode(dirfd, pathname, flags);
#else
  WEBP2_UNSUPPORTED("openat");
#endif
}

int symlinkat(const char *target, int dirfd, const char *linkpath) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || linkpath[0] == '/') {
    return symlink(target, linkpath);
  }

  return __wasilibc_nocwd_symlinkat(target, dirfd, linkpath);
#else
  WEBP2_UNSUPPORTED("symlinkat");
#endif
}

ssize_t readlinkat(int dirfd, const char *__restrict pathname,
                   char *__restrict buf, size_t bufsiz) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return readlink(pathname, buf, bufsiz);
  }

  return __wasilibc_nocwd_readlinkat(dirfd, pathname, buf, bufsiz);
#else
  WEBP2_UNSUPPORTED("readlinkat");
#endif
}

int mkdirat(int dirfd, const char *pathname, mode_t mode) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return mkdir(pathname, mode);
  }

  return __wasilibc_nocwd_mkdirat_nomode(dirfd, pathname);
#else
  WEBP2_UNSUPPORTED("mkdirat");
#endif
}

DIR *opendirat(int dirfd, const char *path) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || path[0] == '/') {
    return opendir(path);
  }

  return __wasilibc_nocwd_opendirat(dirfd, path);
#else
  WEBP2_UNSUPPORTED("opendirat");
#endif
}

int scandirat(int dirfd, const char *dirp, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **)) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || dirp[0] == '/') {
    return scandir(dirp, namelist, filter, compar);
  }

  return __wasilibc_nocwd_scandirat(dirfd, dirp, namelist, filter, compar);
#else
  WEBP2_UNSUPPORTED("scandirat");
#endif
}

int faccessat(int dirfd, const char *pathname, int mode, int flags) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return __wasilibc_access(pathname, mode, flags);
  }

  return __wasilibc_nocwd_faccessat(dirfd, pathname, mode, flags);
#else
  WEBP2_UNSUPPORTED("faccessat");
#endif
}

int fstatat(int dirfd, const char *__restrict pathname,
            struct stat *__restrict statbuf, int flags) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return __wasilibc_stat(pathname, statbuf, flags);
  }

  return __wasilibc_nocwd_fstatat(dirfd, pathname, statbuf, flags);
#else
  WEBP2_UNSUPPORTED("fstatat");
#endif
}

int utimensat(int dirfd, const char *pathname, const struct timespec times[2],
              int flags) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || pathname[0] == '/') {
    return __wasilibc_utimens(pathname, times, flags);
  }

  return __wasilibc_nocwd_utimensat(dirfd, pathname, times, flags);
#else
  WEBP2_UNSUPPORTED("utimensat");
#endif
}

int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
#if !defined(__webp2__)
  if ((olddirfd == AT_FDCWD || oldpath[0] == '/') &&
      (newdirfd == AT_FDCWD || newpath[0] == '/')) {
    return __wasilibc_link(oldpath, newpath, flags);
  }
  if (olddirfd == AT_FDCWD || oldpath[0] == '/') {
    return __wasilibc_link_newat(oldpath, newdirfd, newpath, flags);
  }
  if (newdirfd == AT_FDCWD || newpath[0] == '/') {
    return __wasilibc_link_oldat(olddirfd, oldpath, newpath, flags);
  }

  return __wasilibc_nocwd_linkat(olddirfd, oldpath, newdirfd, newpath, flags);
#else
  WEBP2_UNSUPPORTED("linkat");
#endif // !webp2
}

int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath) {
#if !defined(__webp2__)
  if ((olddirfd == AT_FDCWD || oldpath[0] == '/') &&
      (newdirfd == AT_FDCWD || newpath[0] == '/')) {
    return rename(oldpath, newpath);
  }
  if (olddirfd == AT_FDCWD || oldpath[0] == '/') {
    return __wasilibc_rename_newat(oldpath, newdirfd, newpath);
  }
  if (newdirfd == AT_FDCWD || newpath[0] == '/') {
    return __wasilibc_rename_oldat(olddirfd, oldpath, newpath);
  }

  return __wasilibc_nocwd_renameat(olddirfd, oldpath, newdirfd, newpath);
#else
  WEBP2_UNSUPPORTED("renameat");
#endif // !webp2
}

int __wasilibc_unlinkat(int dirfd, const char *path) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || path[0] == '/') {
    return unlink(path);
  }

  return __wasilibc_nocwd___wasilibc_unlinkat(dirfd, path);
#else
  WEBP2_UNSUPPORTED("__wasilibc_unlinkat");
#endif
}

int __wasilibc_rmdirat(int dirfd, const char *path) {
#if !defined(__webp2__)
  if (dirfd == AT_FDCWD || path[0] == '/') {
    return rmdir(path);
  }

  return __wasilibc_nocwd___wasilibc_rmdirat(dirfd, path);
#else
  WEBP2_UNSUPPORTED("__wasilibc_rmdirat");
#endif
}
