// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

#ifndef __wasip1__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

int fcntl(int fildes, int cmd, ...) {
#if defined(__wasip2__) || defined(__wasip3__)
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (entry == NULL)
    return -1;
#endif

  switch (cmd) {
    case F_GETFD:
      // Act as if the close-on-exec flag is always set.
      return FD_CLOEXEC;
    case F_SETFD:
      // The close-on-exec flag is ignored.
      return 0;
    case F_GETFL: {

#if defined(__wasip1__)
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
#elif defined(__wasip2__) || defined(__wasip3__) 
      if (!entry->vtable->fcntl_getfl) {
        errno = EINVAL;
        return -1;
      }
      return entry->vtable->fcntl_getfl(entry->data);
#else
# error "Unknown WASI version"
#endif
    }
    case F_SETFL: {
      // Set new file descriptor flags.
      va_list ap;
      va_start(ap, cmd);
      int flags = va_arg(ap, int);
      va_end(ap);

#if defined(__wasip1__)
      __wasi_fdflags_t fs_flags = flags & 0xfff;
      __wasi_errno_t error =
          __wasi_fd_fdstat_set_flags(fildes, fs_flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
#elif defined(__wasip2__) || defined(__wasip3__)
      if (!entry->vtable->fcntl_setfl) {
        errno = EINVAL;
        return -1;
      }
      return entry->vtable->fcntl_setfl(entry->data, flags);
#else
# error "Unknown WASI version"
#endif
      return 0;
    }
    default:
      errno = EINVAL;
      return -1;
  }
}
