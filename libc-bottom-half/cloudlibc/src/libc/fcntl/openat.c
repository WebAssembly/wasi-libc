// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

static_assert(O_APPEND == CLOUDABI_FDFLAG_APPEND, "Value mismatch");
static_assert(O_DSYNC == CLOUDABI_FDFLAG_DSYNC, "Value mismatch");
static_assert(O_NONBLOCK == CLOUDABI_FDFLAG_NONBLOCK, "Value mismatch");
static_assert(O_RSYNC == CLOUDABI_FDFLAG_RSYNC, "Value mismatch");
static_assert(O_SYNC == CLOUDABI_FDFLAG_SYNC, "Value mismatch");

static_assert(O_CREAT >> 12 == CLOUDABI_O_CREAT, "Value mismatch");
static_assert(O_DIRECTORY >> 12 == CLOUDABI_O_DIRECTORY, "Value mismatch");
static_assert(O_EXCL >> 12 == CLOUDABI_O_EXCL, "Value mismatch");
static_assert(O_TRUNC >> 12 == CLOUDABI_O_TRUNC, "Value mismatch");

int openat(int fd, const char *path, int oflag, ...) {
  // Compute rights corresponding with the access modes provided.
  // Attempt to obtain all rights, except the ones that contradict the
  // access mode provided to openat().
  cloudabi_rights_t min = 0;
  cloudabi_rights_t max =
      ~(CLOUDABI_RIGHT_FD_DATASYNC | CLOUDABI_RIGHT_FD_READ |
        CLOUDABI_RIGHT_FD_WRITE | CLOUDABI_RIGHT_FILE_ALLOCATE |
        CLOUDABI_RIGHT_FILE_READDIR | CLOUDABI_RIGHT_FILE_STAT_FPUT_SIZE |
        CLOUDABI_RIGHT_MEM_MAP_EXEC);
  switch (oflag & O_ACCMODE) {
    case O_RDONLY:
    case O_RDWR:
    case O_WRONLY:
      if ((oflag & O_RDONLY) != 0) {
        min |= (oflag & O_DIRECTORY) == 0 ? CLOUDABI_RIGHT_FD_READ
                                          : CLOUDABI_RIGHT_FILE_READDIR;
        max |= CLOUDABI_RIGHT_FD_READ | CLOUDABI_RIGHT_FILE_READDIR |
               CLOUDABI_RIGHT_MEM_MAP_EXEC;
      }
      if ((oflag & O_WRONLY) != 0) {
        min |= CLOUDABI_RIGHT_FD_WRITE;
        if ((oflag & O_APPEND) == 0)
          min |= CLOUDABI_RIGHT_FD_SEEK;
        max |= CLOUDABI_RIGHT_FD_DATASYNC | CLOUDABI_RIGHT_FD_WRITE |
               CLOUDABI_RIGHT_FILE_ALLOCATE |
               CLOUDABI_RIGHT_FILE_STAT_FPUT_SIZE;
      }
      break;
    case O_EXEC:
      min |= CLOUDABI_RIGHT_PROC_EXEC;
      break;
    case O_SEARCH:
      break;
    default:
      errno = EINVAL;
      return -1;
  }
  assert((min & max) == min &&
         "Minimal rights should be a subset of the maximum");

  // Ensure that we can actually obtain the minimal rights needed.
  cloudabi_fdstat_t fsb_cur;
  cloudabi_errno_t error = cloudabi_sys_fd_stat_get(fd, &fsb_cur);
  if (error != 0) {
    errno = error;
    return -1;
  }
  if (fsb_cur.fs_filetype != CLOUDABI_FILETYPE_DIRECTORY) {
    errno = ENOTDIR;
    return -1;
  }
  if ((min & fsb_cur.fs_rights_inheriting) != min) {
    errno = ENOTCAPABLE;
    return -1;
  }

  // Path lookup properties.
  cloudabi_lookup_t lookup = {.fd = fd, .flags = 0};
  if ((oflag & O_NOFOLLOW) == 0)
    lookup.flags |= CLOUDABI_LOOKUP_SYMLINK_FOLLOW;

  // Open file with appropriate rights.
  cloudabi_fdstat_t fsb_new = {
      .fs_flags = oflag & 0xfff,
      .fs_rights_base = max & fsb_cur.fs_rights_inheriting,
      .fs_rights_inheriting = fsb_cur.fs_rights_inheriting,
  };
  cloudabi_fd_t newfd;
  error = cloudabi_sys_file_open(lookup, path, strlen(path),
                                 (oflag >> 12) & 0xfff, &fsb_new, &newfd);
  if (error != 0) {
    errno = errno_fixup_directory(lookup.fd, error);
    return -1;
  }
  return newfd;
}
