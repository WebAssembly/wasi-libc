// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <errno.h>
#include <unistd.h>
#include <wasi/api.h>

#ifndef __wasip1__
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <time.h>
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

ssize_t write(int fildes, const void *buf, size_t nbyte) {
#if defined(__wasip1__)
  __wasi_ciovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_written;
  __wasi_errno_t error =
      __wasi_fd_write(fildes, &iov, 1, &bytes_written);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_written;
#else
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (entry->vtable->get_write_stream) {
    wasi_write_t write;
    if (entry->vtable->get_write_stream(entry->data, &write) < 0)
      return -1;
    return __wasilibc_write(&write, buf, nbyte);
  }
  if (entry->vtable->sendto)
    return entry->vtable->sendto(entry->data, buf, nbyte, 0, NULL, 0);
  errno = EOPNOTSUPP;
  return -1;
#endif
}
