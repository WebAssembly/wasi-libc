// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#ifdef __wasilibc_use_wasip2
#include <wasi/wasip2.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <common/errors.h>
#else
#include <wasi/api.h>
#endif
#include <wasi/libc.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

static_assert(O_APPEND == __WASI_FDFLAGS_APPEND, "Value mismatch");
static_assert(O_DSYNC == __WASI_FDFLAGS_DSYNC, "Value mismatch");
static_assert(O_NONBLOCK == __WASI_FDFLAGS_NONBLOCK, "Value mismatch");
static_assert(O_RSYNC == __WASI_FDFLAGS_RSYNC, "Value mismatch");
static_assert(O_SYNC == __WASI_FDFLAGS_SYNC, "Value mismatch");

static_assert(O_CREAT >> 12 == __WASI_OFLAGS_CREAT, "Value mismatch");
static_assert(O_DIRECTORY >> 12 == __WASI_OFLAGS_DIRECTORY, "Value mismatch");
static_assert(O_EXCL >> 12 == __WASI_OFLAGS_EXCL, "Value mismatch");
static_assert(O_TRUNC >> 12 == __WASI_OFLAGS_TRUNC, "Value mismatch");

int __wasilibc_nocwd_openat_nomode(int fd, const char *path, int oflag) {

#ifdef __wasilibc_use_wasip2
  // Set up path flags
  filesystem_path_flags_t lookup_flags = 0;
  if ((oflag & O_NOFOLLOW) == 0)
    lookup_flags |= FILESYSTEM_PATH_FLAGS_SYMLINK_FOLLOW;

  // Set up open flags
  filesystem_open_flags_t open_flags = 0;
  if ((oflag & O_CREAT) != 0)
      open_flags |= FILESYSTEM_OPEN_FLAGS_CREATE;
  if ((oflag & O_DIRECTORY) != 0)
      open_flags |= FILESYSTEM_OPEN_FLAGS_DIRECTORY;
  if ((oflag & O_EXCL) != 0)
      open_flags |= FILESYSTEM_OPEN_FLAGS_EXCLUSIVE;
  if ((oflag & O_TRUNC) != 0)
      open_flags |= FILESYSTEM_OPEN_FLAGS_TRUNCATE;

  // Set up descriptor flags
  filesystem_descriptor_flags_t fs_flags = 0;

  switch (oflag & O_ACCMODE) {
    case O_RDONLY:
    case O_RDWR:
    case O_WRONLY:
      if ((oflag & O_RDONLY) != 0) {
        fs_flags |= FILESYSTEM_DESCRIPTOR_FLAGS_READ;
      }
      if ((oflag & O_WRONLY) != 0) {
        fs_flags |= FILESYSTEM_DESCRIPTOR_FLAGS_WRITE | FILESYSTEM_DESCRIPTOR_FLAGS_MUTATE_DIRECTORY;
      }
      break;
    case O_EXEC:
      break;
    case O_SEARCH:
      break;
    default:
      errno = EINVAL;
      return -1;
  }

  if ((oflag & O_SYNC) != 0)
    fs_flags |= FILESYSTEM_DESCRIPTOR_FLAGS_FILE_INTEGRITY_SYNC;
  if ((oflag & O_DSYNC) != 0)
    fs_flags |= FILESYSTEM_DESCRIPTOR_FLAGS_DATA_INTEGRITY_SYNC;
  if ((oflag & O_RSYNC) != 0)
    fs_flags |= FILESYSTEM_DESCRIPTOR_FLAGS_REQUESTED_WRITE_SYNC;

  // Translate the file descriptor to an internal handle
  filesystem_borrow_descriptor_t file_handle;
  if (!fd_to_file_handle(fd, &file_handle)) {
    errno = EBADF;
    return -1;
  }

  // Construct a WASI string for the path
  wasip2_string_t path2;
  wasip2_string_dup(&path2, path);

  // Open the file, yielding a new handle
  filesystem_own_descriptor_t new_handle;
  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_open_at(file_handle,
                                                 lookup_flags,
                                                 &path2,
                                                 open_flags,
                                                 fs_flags,
                                                 &new_handle,
                                                 &error_code);
  wasip2_string_free(&path2);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }

  // Update the descriptor table with the new handle
  int new_fd = -1;
  descriptor_table_entry_t new_entry;
  new_entry.tag = DESCRIPTOR_TABLE_ENTRY_FILE_HANDLE;
  new_entry.file.readable = ((oflag & O_RDONLY) != 0);
  new_entry.file.writable = ((oflag & O_WRONLY) != 0);
  new_entry.file.file_handle = filesystem_borrow_descriptor(new_handle);
  if (!descriptor_table_insert(new_entry, &new_fd)) {
    errno = ENOMEM;
    return -1;
  }
  // Return the new file descriptor from the table
  return new_fd;
#else
  // Compute rights corresponding with the access modes provided.
  // Attempt to obtain all rights, except the ones that contradict the
  // access mode provided to openat().
  __wasi_rights_t max =
      ~(__WASI_RIGHTS_FD_DATASYNC | __WASI_RIGHTS_FD_READ |
        __WASI_RIGHTS_FD_WRITE | __WASI_RIGHTS_FD_ALLOCATE |
        __WASI_RIGHTS_FD_READDIR | __WASI_RIGHTS_FD_FILESTAT_SET_SIZE);
  switch (oflag & O_ACCMODE) {
    case O_RDONLY:
    case O_RDWR:
    case O_WRONLY:
      if ((oflag & O_RDONLY) != 0) {
        max |= __WASI_RIGHTS_FD_READ | __WASI_RIGHTS_FD_READDIR;
      }
      if ((oflag & O_WRONLY) != 0) {
        max |= __WASI_RIGHTS_FD_DATASYNC | __WASI_RIGHTS_FD_WRITE |
               __WASI_RIGHTS_FD_ALLOCATE |
               __WASI_RIGHTS_FD_FILESTAT_SET_SIZE;
      }
      break;
    case O_EXEC:
      break;
    case O_SEARCH:
      break;
    default:
      errno = EINVAL;
      return -1;
  }

  // Ensure that we can actually obtain the minimal rights needed.
  __wasi_fdstat_t fsb_cur;
  __wasi_errno_t error = __wasi_fd_fdstat_get(fd, &fsb_cur);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Path lookup properties.
  __wasi_lookupflags_t lookup_flags = 0;
  if ((oflag & O_NOFOLLOW) == 0)
    lookup_flags |= __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW;

  // Open file with appropriate rights.
  __wasi_fdflags_t fs_flags = oflag & 0xfff;
  __wasi_rights_t fs_rights_base = max & fsb_cur.fs_rights_inheriting;
  __wasi_rights_t fs_rights_inheriting = fsb_cur.fs_rights_inheriting;
  __wasi_fd_t newfd;
  error = __wasi_path_open(fd, lookup_flags, path,
                                 (oflag >> 12) & 0xfff,
                                 fs_rights_base, fs_rights_inheriting, fs_flags,
                                 &newfd);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return newfd;
#endif

}
