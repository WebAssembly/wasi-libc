// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#ifndef SYS_STAT_STAT_IMPL_H
#define SYS_STAT_STAT_IMPL_H

#include <common/time.h>

#include <sys/stat.h>

#include <assert.h>
#include <wasi.h>
#include <stdbool.h>

static_assert(S_ISBLK(S_IFBLK), "Value mismatch");
static_assert(S_ISCHR(S_IFCHR), "Value mismatch");
static_assert(S_ISDIR(S_IFDIR), "Value mismatch");
static_assert(S_ISFIFO(S_IFIFO), "Value mismatch");
static_assert(S_ISLNK(S_IFLNK), "Value mismatch");
static_assert(S_ISREG(S_IFREG), "Value mismatch");
static_assert(S_ISSOCK(S_IFSOCK), "Value mismatch");

static inline void to_public_stat(const __wasi_filestat_t *in,
                                  struct stat *out) {
  // Ensure that we don't truncate any values.
  static_assert(sizeof(in->st_dev) == sizeof(out->st_dev), "Size mismatch");
  static_assert(sizeof(in->st_ino) == sizeof(out->st_ino), "Size mismatch");
#ifdef __wasilibc_unmodified_upstream
  static_assert(sizeof(in->st_filetype) == sizeof(out->__st_filetype),
                "Size mismatch");
#else
  /*
   * The non-standard __st_filetype field appears to only be used for shared
   * memory, which we don't currently support.
   */
#endif
#ifdef __wasilibc_unmodified_upstream
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
#ifdef __wasilibc_unmodified_upstream
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
    case __WASI_FILETYPE_BLOCK_DEVICE:
      out->st_mode |= S_IFBLK;
      break;
    case __WASI_FILETYPE_CHARACTER_DEVICE:
      out->st_mode |= S_IFCHR;
      break;
    case __WASI_FILETYPE_DIRECTORY:
      out->st_mode |= S_IFDIR;
      break;
    case __WASI_FILETYPE_REGULAR_FILE:
      out->st_mode |= S_IFREG;
      break;
    case __WASI_FILETYPE_SOCKET_DGRAM:
    case __WASI_FILETYPE_SOCKET_STREAM:
      out->st_mode |= S_IFSOCK;
      break;
    case __WASI_FILETYPE_SYMBOLIC_LINK:
      out->st_mode |= S_IFLNK;
      break;
  }
}

static inline bool utimens_get_timestamps(const struct timespec *times,
#ifdef __wasilibc_unmodified_upstream // fstat
                                          __wasi_filestat_t *fs,
                                          __wasi_fsflags_t *flags) {
#else
                                          __wasi_timestamp_t *st_atim,
                                          __wasi_timestamp_t *st_mtim,
                                          __wasi_fstflags_t *flags) {
#endif
  if (times == NULL) {
    // Update both timestamps.
#ifdef __wasilibc_unmodified_upstream // fstat
    *flags = __WASI_FILESTAT_ATIM_NOW | __WASI_FILESTAT_MTIM_NOW;
#else
    *flags = __WASI_FILE_STAT_SET_ATIM_NOW | __WASI_FILE_STAT_SET_MTIM_NOW;
#endif
  } else {
    // Set individual timestamps.
    *flags = 0;
    switch (times[0].tv_nsec) {
      case UTIME_NOW:
#ifdef __wasilibc_unmodified_upstream // fstat
        *flags |= __WASI_FILESTAT_ATIM_NOW;
#else
        *flags |= __WASI_FILE_STAT_SET_ATIM_NOW;
#endif
        break;
      case UTIME_OMIT:
        break;
      default:
#ifdef __wasilibc_unmodified_upstream // fstat
        *flags |= __WASI_FILESTAT_ATIM;
        if (!timespec_to_timestamp_exact(&times[0], &fs->st_atim))
#else
        *flags |= __WASI_FILE_STAT_SET_ATIM;
        if (!timespec_to_timestamp_exact(&times[0], st_atim))
#endif
          return false;
        break;
    }

    switch (times[1].tv_nsec) {
      case UTIME_NOW:
#ifdef __wasilibc_unmodified_upstream // fstat
        *flags |= __WASI_FILESTAT_MTIM_NOW;
#else
        *flags |= __WASI_FILE_STAT_SET_MTIM_NOW;
#endif
        break;
      case UTIME_OMIT:
        break;
      default:
#ifdef __wasilibc_unmodified_upstream // fstat
        *flags |= __WASI_FILESTAT_MTIM;
        if (!timespec_to_timestamp_exact(&times[1], &fs->st_mtim))
#else
        *flags |= __WASI_FILE_STAT_SET_MTIM;
        if (!timespec_to_timestamp_exact(&times[1], st_mtim))
#endif
          return false;
        break;
    }
  }
  return true;
}

#endif
