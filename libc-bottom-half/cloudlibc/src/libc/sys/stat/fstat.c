// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>

#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <errno.h>

#include "stat_impl.h"

#ifdef __wasilibc_use_wasip2
static int fstat_file(filesystem_borrow_descriptor_t file, struct stat *buf) {
  // Get the metadata hash for the file descriptor
  filesystem_metadata_hash_value_t metadata;
  filesystem_error_code_t error;
  if (!filesystem_method_descriptor_metadata_hash(file,
                                                  &metadata,
                                                  &error)) {
    translate_error(error);
    return -1;
  }

  // Get the file metadata
  filesystem_descriptor_stat_t internal_stat;
  bool stat_result  = filesystem_method_descriptor_stat(file,
                                                        &internal_stat,
                                                        &error);
  if (!stat_result) {
    translate_error(error);
    return -1;
  }

  // Convert the internal data to an external struct
  to_public_stat(&metadata, &internal_stat, buf);
  return 0;
}

int fstat(int fildes, struct stat *buf) {
  // Translate the file descriptor to an internal handle
  descriptor_table_entry_t *entry;
  if (!descriptor_table_get_ref(fildes, &entry)) {
    errno = EBADF;
    return -1;
  }

  int st_mode = 0;

  switch (entry->tag) {
    case DESCRIPTOR_TABLE_ENTRY_FILE:
      if (entry->file.file_handle.__handle != 0)
        return fstat_file(filesystem_borrow_descriptor(entry->file.file_handle), buf);
      break;
    case DESCRIPTOR_TABLE_ENTRY_TCP_SOCKET:
    case DESCRIPTOR_TABLE_ENTRY_UDP_SOCKET:
      st_mode = S_IFSOCK;
      break;
    default:
      errno = EBADF;
      return -1;
  }

  memset(buf, 0, sizeof(*buf));
  buf->st_mode = st_mode;
  return 0;
}
#else
int fstat(int fildes, struct stat *buf) {
  __wasi_filestat_t internal_stat;
  __wasi_errno_t error = __wasi_fd_filestat_get(fildes, &internal_stat);
  if (error != 0) {
    errno = error;
    return -1;
  }
  to_public_stat(&internal_stat, buf);
  return 0;
}
#endif
