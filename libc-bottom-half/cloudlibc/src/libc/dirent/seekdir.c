// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <dirent.h>

#include "dirent_impl.h"

void seekdir(DIR *dirp, long loc) {
#ifdef __wasip2__
  dirp->stream.__handle = 0;
  dirp->skip = loc;
  dirp->offset = 0;
#else
  // Update cookie.
  dirp->cookie = (unsigned long)loc;
  // Mark entire buffer as processed to force a read of new data.
  // TODO(ed): We could prevent a read if the offset is in the buffer.
  dirp->buffer_used = dirp->buffer_processed = dirp->buffer_size;
#endif
}
