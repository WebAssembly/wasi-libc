// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <wasi.h>
#include <errno.h>
#include <unistd.h>

static_assert(SEEK_CUR == WASI_WHENCE_CUR, "Value mismatch");
static_assert(SEEK_END == WASI_WHENCE_END, "Value mismatch");
static_assert(SEEK_SET == WASI_WHENCE_SET, "Value mismatch");

off_t lseek(int fildes, off_t offset, int whence) {
  wasi_filesize_t new_offset;
  wasi_errno_t error =
      wasi_fd_seek(fildes, offset, whence, &new_offset);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return new_offset;
}
