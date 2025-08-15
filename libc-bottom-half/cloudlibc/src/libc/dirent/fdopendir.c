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
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

#include "dirent_impl.h"

DIR *fdopendir(int fd) {
  // Allocate new directory object and read buffer.
  DIR *dirp = malloc(sizeof(*dirp));
  if (dirp == NULL)
    return NULL;
  dirp->buffer = malloc(DIRENT_DEFAULT_BUFFER_SIZE);
  if (dirp->buffer == NULL) {
    free(dirp);
    return NULL;
  }

#ifdef __wasilibc_use_wasip2
  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fd, &file_handle)) {
    errno = EBADF;
    return NULL;
  }

  // Read the directory
  filesystem_own_directory_entry_stream_t result;
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_read_directory(file_handle,
                                                        &result,
                                                        &error_code);
  if (!ok) {
    free(dirp->buffer);
    free(dirp);
    translate_error(error_code);
    return NULL;
  }

  dirp->fd = fd;
  // Add an internal handle for the buffer
  descriptor_table_entry_t new_entry;
  new_entry.tag = DESCRIPTOR_TABLE_ENTRY_DIRECTORY_STREAM;
  directory_stream_entry_t stream_info;
  stream_info.directory_stream = result;
  stream_info.directory_file_handle = file_handle;
  new_entry.directory_stream_info = stream_info;
  int new_fd = -1;
  descriptor_table_update(dirp->fd, new_entry);

  dirp->cookie = __WASI_DIRCOOKIE_START;
  dirp->buffer_processed = 0;
  dirp->buffer_size = DIRENT_DEFAULT_BUFFER_SIZE;
  dirp->dirent = NULL;
  dirp->dirent_size = 1;
  return dirp;

#else
  // Ensure that this is really a directory by already loading the first
  // chunk of data.
  __wasi_errno_t error =
      // TODO: Remove the cast on `dirp->buffer` once the witx is updated with char8 support.
      __wasi_fd_readdir(fd, (uint8_t *)dirp->buffer, DIRENT_DEFAULT_BUFFER_SIZE,
                                __WASI_DIRCOOKIE_START, &dirp->buffer_used);
  if (error != 0) {
    free(dirp->buffer);
    free(dirp);
    errno = error;
    return NULL;
  }

  // Initialize other members.
  dirp->fd = fd;
  dirp->cookie = __WASI_DIRCOOKIE_START;
  dirp->buffer_processed = 0;
  dirp->buffer_size = DIRENT_DEFAULT_BUFFER_SIZE;
  dirp->dirent = NULL;
  dirp->dirent_size = 1;
  return dirp;
#endif
}
