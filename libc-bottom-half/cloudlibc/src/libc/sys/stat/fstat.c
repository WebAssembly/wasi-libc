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
int fstat(int fildes, struct stat *buf) {
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle_allow_open(fildes, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Get the metadata hash for the file descriptor
  filesystem_metadata_hash_value_t metadata;
  filesystem_error_code_t error;
  if (!filesystem_method_descriptor_metadata_hash(file_handle,
                                                  &metadata,
                                                  &error)) {
    translate_error(error);
    return -1;
  }

  // Get the file metadata
  filesystem_descriptor_stat_t internal_stat;
  bool stat_result  = filesystem_method_descriptor_stat(file_handle,
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
