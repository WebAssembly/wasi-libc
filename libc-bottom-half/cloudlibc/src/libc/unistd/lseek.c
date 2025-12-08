// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>
#include <unistd.h>

static_assert(SEEK_CUR == __WASI_WHENCE_CUR, "Value mismatch");
static_assert(SEEK_END == __WASI_WHENCE_END, "Value mismatch");
static_assert(SEEK_SET == __WASI_WHENCE_SET, "Value mismatch");

off_t __lseek(int fildes, off_t offset, int whence) {
#ifdef __wasilibc_use_wasip2
  // Look up a stream for fildes
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->seek) {
    errno = EINVAL;
    return -1;
  }
  return entry->vtable->seek(entry->data, offset, whence);
#else
  __wasi_filesize_t new_offset;
  __wasi_errno_t error =
      __wasi_fd_seek(fildes, offset, whence, &new_offset);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return new_offset;
#endif
}

weak_alias(__lseek, lseek);
