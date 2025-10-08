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
#include <wasi/libc.h>
#include <wasi/libc-nocwd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "dirent_impl.h"

static int sel_true(const struct dirent *de) {
  return 1;
}

int __wasilibc_nocwd_scandirat(int dirfd, const char *dir, struct dirent ***namelist,
                               int (*sel)(const struct dirent *),
                               int (*compar)(const struct dirent **, const struct dirent **)) {
  struct stat statbuf;

  // Match all files if no select function is provided.
  if (sel == NULL)
    sel = sel_true;

#ifdef __wasilibc_use_wasip2
  // Open the directory.
  int fd = __wasilibc_nocwd_openat_nomode(dirfd, dir, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
  if (fd == -1)
    return -1;
  DIR *dirp = fdopendir(fd);
  if (!dirp)
    return -1;
  fd = dirp->fd;

  // Translate the file descriptor to an internal handle
  filesystem_borrow_directory_entry_stream_t stream;
  filesystem_borrow_descriptor_t parent_file_handle;
  read_directory_state_t state;
  if (!fd_to_directory_stream(fd, &stream, &parent_file_handle, &state)) {
    errno = EBADF;
    return -1;
  }

  // Space for the array to return to the caller.
  struct dirent **dirents = NULL;
  size_t dirents_size = 0;
  size_t dirents_used = 0;

  bool ok = true;
  filesystem_option_directory_entry_t dir_entry_optional;
  filesystem_error_code_t error_code;
  bool handle_dot = false;
  bool handle_dot_dot = false;
  while (true) {
    ok = filesystem_method_directory_entry_stream_read_directory_entry(stream,
                                                                       &dir_entry_optional,
                                                                       &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }
    if (!dir_entry_optional.is_some) {
      // All directory entries have been read; handle . and ..
      if (!handle_dot && !handle_dot_dot) {
        handle_dot = true;
      } else if (handle_dot && !handle_dot_dot) {
        handle_dot_dot = true;
        handle_dot = false;
      } else
        break;
    }

    // Create the new directory entry
    size_t name_len = handle_dot ? 1 : handle_dot_dot ? 2 : dir_entry_optional.val.name.len;
    struct dirent *dirent =
      malloc(offsetof(struct dirent, d_name) + name_len + 1);
    if (dirent == NULL) {
      errno = EINVAL;
      return -1;
    }
    dirent->d_type = (handle_dot || handle_dot_dot) ? DT_DIR : dir_entry_type_to_d_type(dir_entry_optional.val.type);
    if (handle_dot)
      memcpy(dirent->d_name, ".", name_len);
    else if (handle_dot_dot)
      memcpy(dirent->d_name, "..", name_len);
    else
      memcpy(dirent->d_name, dir_entry_optional.val.name.ptr, name_len);
    dirent->d_name[name_len] = '\0';

    if (!(handle_dot || handle_dot_dot)) {
      // Do an `fstatat` to get the inode number.
      if (fstatat(fd, dirent->d_name, &statbuf, AT_SYMLINK_NOFOLLOW) != 0) {
        errno = EBADF;
        return -1;
      }
      // Fill in the inode.
      dirent->d_ino = statbuf.st_ino;

      // In case someone raced with us and replaced the object with this name
      // with another of a different type, update the type too.
      dirent->d_type = __wasilibc_iftodt(statbuf.st_mode & S_IFMT);
    } else {
      dirent->d_ino = -1;
    }

    if (sel(dirent)) {
      // Add the entry to the results
        if (dirents_used == dirents_size) {
          dirents_size = dirents_size < 8 ? 8 : dirents_size * 2;
          struct dirent **new_dirents =
            realloc(dirents, dirents_size * sizeof(*dirents));
          if (new_dirents == NULL) {
            free(dirent);
            free(dirents);
            errno = EBADF;
            return -1;
          }
          dirents = new_dirents;
        }
        dirents[dirents_used++] = dirent;
    } else {
      // Discard the entry.
        free(dirent);
    }
  }

  // Sort results and return them
  filesystem_directory_entry_stream_drop_borrow(stream);
  (qsort)(dirents, dirents_used, sizeof(*dirents),
          (int (*)(const void *, const void *))compar);
  *namelist = dirents;
  return dirents_used;
#else
  // Open the directory.
  int fd = __wasilibc_nocwd_openat_nomode(dirfd, dir, O_RDONLY | O_NONBLOCK | O_DIRECTORY);
  if (fd == -1)
    return -1;

  // Allocate a read buffer for the directory entries.
  size_t buffer_size = DIRENT_DEFAULT_BUFFER_SIZE;
  char *buffer = malloc(buffer_size);
  if (buffer == NULL) {
    close(fd);
    return -1;
  }
  size_t buffer_processed = buffer_size;
  size_t buffer_used = buffer_size;

  // Space for the array to return to the caller.
  struct dirent **dirents = NULL;
  size_t dirents_size = 0;
  size_t dirents_used = 0;

  __wasi_dircookie_t cookie = __WASI_DIRCOOKIE_START;
  for (;;) {
    // Extract the next dirent header.
    size_t buffer_left = buffer_used - buffer_processed;
    if (buffer_left < sizeof(__wasi_dirent_t)) {
      // End-of-file.
      if (buffer_used < buffer_size)
        break;
      goto read_entries;
    }
    __wasi_dirent_t entry;
    memcpy(&entry, buffer + buffer_processed, sizeof(entry));

    size_t entry_size = sizeof(__wasi_dirent_t) + entry.d_namlen;
    if (entry.d_namlen == 0) {
      // Invalid pathname length. Skip the entry.
      buffer_processed += entry_size;
      continue;
    }

    // The entire entry must be present in buffer space. If not, read
    // the entry another time. Ensure that the read buffer is large
    // enough to fit at least this single entry.
    if (buffer_left < entry_size) {
      while (buffer_size < entry_size)
        buffer_size *= 2;
      char *new_buffer = realloc(buffer, buffer_size);
      if (new_buffer == NULL)
        goto bad;
      buffer = new_buffer;
      goto read_entries;
    }

    // Skip entries having null bytes in the filename.
    const char *name = buffer + buffer_processed + sizeof(entry);
    buffer_processed += entry_size;
    if (memchr(name, '\0', entry.d_namlen) != NULL)
      continue;

    // Create the new directory entry.
    struct dirent *dirent =
        malloc(offsetof(struct dirent, d_name) + entry.d_namlen + 1);
    if (dirent == NULL)
      goto bad;
    dirent->d_type = entry.d_type;
    memcpy(dirent->d_name, name, entry.d_namlen);
    dirent->d_name[entry.d_namlen] = '\0';

    // `fd_readdir` implementations may set the inode field to zero if the
    // the inode number is unknown. In that case, do an `fstatat` to get the
    // inode number.
    off_t d_ino = entry.d_ino;
    unsigned char d_type = entry.d_type;
    if (d_ino == 0) {
      if (fstatat(fd, dirent->d_name, &statbuf, AT_SYMLINK_NOFOLLOW) != 0) {
        return -1;
      }

      // Fill in the inode.
      d_ino = statbuf.st_ino;

      // In case someone raced with us and replaced the object with this name
      // with another of a different type, update the type too.
      d_type = __wasilibc_iftodt(statbuf.st_mode & S_IFMT);
    }
    dirent->d_ino = d_ino;
    dirent->d_type = d_type;

    cookie = entry.d_next;

    if (sel(dirent)) {
      // Add the entry to the results.
      if (dirents_used == dirents_size) {
        dirents_size = dirents_size < 8 ? 8 : dirents_size * 2;
        struct dirent **new_dirents =
            realloc(dirents, dirents_size * sizeof(*dirents));
        if (new_dirents == NULL) {
          free(dirent);
          goto bad;
        }
        dirents = new_dirents;
      }
      dirents[dirents_used++] = dirent;
    } else {
      // Discard the entry.
      free(dirent);
    }
    continue;

  read_entries:;
    // Load more directory entries and continue.
    // TODO: Remove the cast on `buffer` once the witx is updated with char8 support.
    __wasi_errno_t error = __wasi_fd_readdir(fd, (uint8_t *)buffer, buffer_size,
                                                       cookie, &buffer_used);
    if (error != 0) {
      errno = error;
      goto bad;
    }
    buffer_processed = 0;
  }

  // Sort results and return them.
  free(buffer);
  close(fd);
  (qsort)(dirents, dirents_used, sizeof(*dirents),
          (int (*)(const void *, const void *))compar);
  *namelist = dirents;
  return dirents_used;

bad:
  // Deallocate partially created results.
  for (size_t i = 0; i < dirents_used; ++i)
    free(dirents[i]);
  free(dirents);
  free(buffer);
  close(fd);
  return -1;
#endif

}
