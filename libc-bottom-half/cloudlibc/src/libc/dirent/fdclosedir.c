// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>
#include <stdlib.h>

#include "dirent_impl.h"

int fdclosedir(DIR *dirp) {
  int fd = dirp->fd;
#if defined(__wasip1__)
  free(dirp->buffer);
#elif defined(__wasip2__)
  if (dirp->stream.__handle != 0)
    filesystem_directory_entry_stream_drop_own(dirp->stream);
#elif defined(__wasip3__)
  if (dirp->stream != 0)
    filesystem_stream_directory_entry_drop_readable(dirp->stream);
  if (dirp->future != 0)
    filesystem_future_result_void_error_code_drop_readable(dirp->future);
#else
# error "Unsupported WASI version"
#endif
  free(dirp->dirent);
  free(dirp);
  return fd;
}
