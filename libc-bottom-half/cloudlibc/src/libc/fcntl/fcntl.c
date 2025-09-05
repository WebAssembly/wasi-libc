// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

int fcntl(int fildes, int cmd, ...) {
  switch (cmd) {
    case F_GETFD:
      // Act as if the close-on-exec flag is always set.
      return FD_CLOEXEC;
    case F_SETFD:
      // The close-on-exec flag is ignored.
      return 0;
    case F_GETFL: {
#ifdef __wasilibc_use_wasip2
    // Translate the file descriptor to an internal handle
    descriptor_table_entry_t *entry;
    bool ref_exists = descriptor_table_get_ref(fildes, &entry);
    filesystem_borrow_descriptor_t file_handle;
    if (!ref_exists) {
      errno = EBADF;
      return -1;
    }
    if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_STREAM)
      file_handle = entry->stream.file_info.file_handle;
    else if (entry->tag == DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE)
      file_handle = entry->file.file_handle;
    else {
      errno = EINVAL;
      return -1;
    }

    // Get the flags of the descriptor
    filesystem_descriptor_flags_t flags;
    filesystem_error_code_t error_code;
    if (!filesystem_method_descriptor_get_flags(file_handle, &flags, &error_code)) {
      translate_error(error_code);
      return -1;
    }

    int oflags = 0;
    if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_READ) {
      if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_WRITE)
        oflags |= O_RDWR;
      else
        oflags |= O_RDONLY;
    } else if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_WRITE)
        oflags |= O_WRONLY;
    else
        oflags |= O_SEARCH;

    return oflags;
#else
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
#endif
    }
    case F_SETFL: {
      // Set new file descriptor flags.
      va_list ap;
      va_start(ap, cmd);
      int flags = va_arg(ap, int);
      va_end(ap);

#ifdef __wasilibc_use_wasip2
    // Translate the file descriptor to an internal handle
    filesystem_borrow_descriptor_t file_handle;
    if (!fd_to_file_handle_allow_open(fildes, &file_handle)) {
      errno = EBADF;
      return -1;
    }

    // This is a no-op -- not supported in wasip2
#else
      __wasi_fdflags_t fs_flags = flags & 0xfff;
      __wasi_errno_t error =
          __wasi_fd_fdstat_set_flags(fildes, fs_flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
#endif
      return 0;
    }
    default:
      errno = EINVAL;
      return -1;
  }
}
