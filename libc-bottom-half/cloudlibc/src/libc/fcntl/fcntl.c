// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

int fcntl(int fildes, int cmd, ...) {
  switch (cmd) {
    case F_GETFD: {
      __wasi_fdflagsext_t flags;
      __wasi_errno_t error = __wasi_fd_fdflags_get(fildes, &flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return flags & __WASI_FDFLAGSEXT_CLOEXEC ? FD_CLOEXEC : 0;
    }
    case F_SETFD: {
      va_list ap;
      va_start(ap, cmd);
      int flags = va_arg(ap, int);
      va_end(ap);

      __wasi_fdflagsext_t fd_flags = flags | FD_CLOEXEC ? __WASI_FDFLAGSEXT_CLOEXEC : 0;
      __wasi_errno_t error =
          __wasi_fd_fdflags_set(fildes, fd_flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case F_GETFL: {
      // Obtain the flags and the rights of the descriptor.
      __wasi_fdstat_t fds;
      __wasi_errno_t error = __wasi_fd_fdstat_get(fildes, &fds);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Roughly approximate the access mode by converting the rights.
      int oflags = fds.fs_flags;
      if ((fds.fs_rights_base &
           (__WASI_RIGHTS_FD_READ | __WASI_RIGHTS_FD_READDIR)) != 0) {
        if ((fds.fs_rights_base & __WASI_RIGHTS_FD_WRITE) != 0)
          oflags |= O_RDWR;
        else
          oflags |= O_RDONLY;
      } else if ((fds.fs_rights_base & __WASI_RIGHTS_FD_WRITE) != 0) {
        oflags |= O_WRONLY;
      } else {
        oflags |= O_SEARCH;
      }
      return oflags;
    }
    case F_SETFL: {
      // Set new file descriptor flags.
      va_list ap;
      va_start(ap, cmd);
      int flags = va_arg(ap, int);
      va_end(ap);

      __wasi_fdflags_t fs_flags = flags & 0xfff;
      __wasi_errno_t error =
          __wasi_fd_fdstat_set_flags(fildes, fs_flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case F_DUPFD:
    case F_DUPFD_CLOEXEC: {
      va_list ap;
      va_start(ap, cmd);
      int min_res_fd = va_arg(ap, int);
      va_end(ap);

      int fd;
      __wasi_bool_t cloexec = cmd == F_DUPFD_CLOEXEC;
      __wasi_errno_t error = __wasi_fd_dup2(fildes, min_res_fd, cloexec, &fd);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return fd;
    }
    default:
      errno = EINVAL;
      return -1;
  }
}
