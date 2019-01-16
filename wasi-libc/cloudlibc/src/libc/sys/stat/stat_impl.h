// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef SYS_STAT_STAT_IMPL_H
#define SYS_STAT_STAT_IMPL_H

#include <common/time.h>

#include <sys/stat.h>

#include <assert.h>
#include <cloudabi_types.h>
#include <stdbool.h>

static_assert(S_ISBLK(S_IFBLK), "Value mismatch");
static_assert(S_ISCHR(S_IFCHR), "Value mismatch");
static_assert(S_ISDIR(S_IFDIR), "Value mismatch");
static_assert(S_ISFIFO(S_IFIFO), "Value mismatch");
static_assert(S_ISLNK(S_IFLNK), "Value mismatch");
static_assert(S_ISREG(S_IFREG), "Value mismatch");
static_assert(S_ISSOCK(S_IFSOCK), "Value mismatch");

static inline void to_public_stat(const cloudabi_filestat_t *in,
                                  struct stat *out) {
  // Ensure that we don't truncate any values.
  static_assert(sizeof(in->st_dev) == sizeof(out->st_dev), "Size mismatch");
  static_assert(sizeof(in->st_ino) == sizeof(out->st_ino), "Size mismatch");
#ifdef __wasilibc_unmodified_upstream__
  static_assert(sizeof(in->st_filetype) == sizeof(out->__st_filetype),
                "Size mismatch");
#else
  /*
   * The -standard __st_filetype field appears to only be used for shared
   * memory, which we don't currently support.
   */
#endif
#ifdef __wasilibc_unmodified_upstream__
  static_assert(sizeof(in->st_nlink) == sizeof(out->st_nlink), "Size mismatch");
#else
  /* nlink_t is 64-bit on wasm32, following the x32 ABI. */
  static_assert(sizeof(in->st_nlink) <= sizeof(out->st_nlink), "Size shortfall");
#endif
  static_assert(sizeof(in->st_size) == sizeof(out->st_size), "Size mismatch");

  *out = (struct stat){
#define COPY_FIELD(field) .field = in->field
      COPY_FIELD(st_dev),
      COPY_FIELD(st_ino),
#ifdef __wasilibc_unmodified_upstream__
      .__st_filetype = in->st_filetype,
#endif
      COPY_FIELD(st_nlink),
      COPY_FIELD(st_size),
#undef COPY_FIELD
#define COPY_TIMESPEC(field) .field = timestamp_to_timespec(in->field)
      COPY_TIMESPEC(st_atim),
      COPY_TIMESPEC(st_mtim),
      COPY_TIMESPEC(st_ctim),
#undef COPY_TIMESPEC
  };

  // Convert file type to legacy types encoded in st_mode.
  switch (in->st_filetype) {
    case CLOUDABI_FILETYPE_BLOCK_DEVICE:
      out->st_mode |= S_IFBLK;
      break;
    case CLOUDABI_FILETYPE_CHARACTER_DEVICE:
      out->st_mode |= S_IFCHR;
      break;
    case CLOUDABI_FILETYPE_DIRECTORY:
      out->st_mode |= S_IFDIR;
      break;
    case CLOUDABI_FILETYPE_REGULAR_FILE:
      out->st_mode |= S_IFREG;
      break;
    case CLOUDABI_FILETYPE_SOCKET_DGRAM:
    case CLOUDABI_FILETYPE_SOCKET_STREAM:
      out->st_mode |= S_IFSOCK;
      break;
    case CLOUDABI_FILETYPE_SYMBOLIC_LINK:
      out->st_mode |= S_IFLNK;
      break;
  }
}

static inline bool utimens_get_timestamps(const struct timespec *times,
                                          cloudabi_filestat_t *fs,
                                          cloudabi_fsflags_t *flags) {
  if (times == NULL) {
    // Update both timestamps.
    *flags = CLOUDABI_FILESTAT_ATIM_NOW | CLOUDABI_FILESTAT_MTIM_NOW;
  } else {
    // Set individual timestamps.
    *flags = 0;
    switch (times[0].tv_nsec) {
      case UTIME_NOW:
        *flags |= CLOUDABI_FILESTAT_ATIM_NOW;
        break;
      case UTIME_OMIT:
        break;
      default:
        *flags |= CLOUDABI_FILESTAT_ATIM;
        if (!timespec_to_timestamp_exact(&times[0], &fs->st_atim))
          return false;
        break;
    }

    switch (times[1].tv_nsec) {
      case UTIME_NOW:
        *flags |= CLOUDABI_FILESTAT_MTIM_NOW;
        break;
      case UTIME_OMIT:
        break;
      default:
        *flags |= CLOUDABI_FILESTAT_MTIM;
        if (!timespec_to_timestamp_exact(&times[1], &fs->st_mtim))
          return false;
        break;
    }
  }
  return true;
}

#endif
