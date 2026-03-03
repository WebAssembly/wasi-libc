// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <unistd.h>

#ifndef __wasip1__
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#include <string.h>
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

ssize_t read(int fildes, void *buf, size_t nbyte) {
#if defined(__wasip1__)
  __wasi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  __wasi_errno_t error = __wasi_fd_read(fildes, &iov, 1, &bytes_read);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_read;
#else
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (entry->vtable->get_read_stream) {
    wasi_read_t read;
    if (entry->vtable->get_read_stream(entry->data, &read) < 0)
      return -1;
    return __wasilibc_read(&read, buf, nbyte);
  }
  if (entry->vtable->recvfrom)
    return entry->vtable->recvfrom(entry->data, buf, nbyte, 0, NULL, NULL);
  errno = EOPNOTSUPP;
  return -1;
#endif
}
