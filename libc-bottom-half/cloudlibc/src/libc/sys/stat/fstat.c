// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/descriptor_table.h>
#else
#include <wasi/api.h>
#include "stat_impl.h"
#endif
#include <errno.h>

int fstat(int fildes, struct stat *buf) {
#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  return entry->vtable->fstat(entry->data, buf);
#else
  __wasi_filestat_t internal_stat;
  __wasi_errno_t error = __wasi_fd_filestat_get(fildes, &internal_stat);
  if (error != 0) {
    errno = error;
    return -1;
  }
  to_public_stat(&internal_stat, buf);
  return 0;
#endif
}
