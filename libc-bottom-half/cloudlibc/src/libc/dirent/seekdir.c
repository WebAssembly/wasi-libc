// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>

#include "dirent_impl.h"

void seekdir(DIR *dirp, long loc) {
#if defined(__wasip1__)
  // Update cookie.
  dirp->cookie = (unsigned long)loc;
  // Mark entire buffer as processed to force a read of new data.
  // TODO(ed): We could prevent a read if the offset is in the buffer.
  dirp->buffer_used = dirp->buffer_processed = dirp->buffer_size;
#elif defined(__wasip2__)
  dirp->stream.__handle = 0;
  dirp->skip = loc;
  dirp->offset = 0;
#elif defined(__wasip3__)
  if (dirp->stream.f0 != 0) {
    filesystem_stream_directory_entry_drop_readable(dirp->stream.f0);
    dirp->stream.f0 = 0;
  }
  if (dirp->stream.f1 != 0) {
    filesystem_future_result_void_error_code_drop_readable(dirp->stream.f1);
    dirp->stream.f1 = 0;
  }
  dirp->skip = loc;
  dirp->offset = 0;
#else
# error "Unsupported WASI version"
#endif
}
