// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <wasi/api.h>
#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef __wasip1__
#include <wasi/file_utils.h>
#include <common/errors.h>
#endif

#include "dirent_impl.h"

#ifdef __wasip1__
static_assert(DT_BLK == __WASI_FILETYPE_BLOCK_DEVICE, "Value mismatch");
static_assert(DT_CHR == __WASI_FILETYPE_CHARACTER_DEVICE, "Value mismatch");
static_assert(DT_DIR == __WASI_FILETYPE_DIRECTORY, "Value mismatch");
static_assert(DT_FIFO == __WASI_FILETYPE_SOCKET_STREAM, "Value mismatch");
static_assert(DT_LNK == __WASI_FILETYPE_SYMBOLIC_LINK, "Value mismatch");
static_assert(DT_REG == __WASI_FILETYPE_REGULAR_FILE, "Value mismatch");
static_assert(DT_UNKNOWN == __WASI_FILETYPE_UNKNOWN, "Value mismatch");
#endif

#ifdef __wasip3__
#include <wasi/wasip3_block.h>
#endif

// Grows a buffer to be large enough to hold a certain amount of data.
#define GROW(buffer, buffer_size, target_size)      \
  do {                                              \
    if ((buffer_size) < (target_size)) {            \
      size_t new_size = (buffer_size);              \
      while (new_size < (target_size))              \
        new_size *= 2;                              \
      void *new_buffer = realloc(buffer, new_size); \
      if (new_buffer == NULL) {                     \
        errno = ENOMEM;                             \
        return NULL;                                \
      }                                             \
      (buffer) = new_buffer;                        \
      (buffer_size) = new_size;                     \
    }                                               \
  } while (0)

#if defined(__wasip1__)
struct dirent *readdir(DIR *dirp) {
  for (;;) {
    // Extract the next dirent header.
    size_t buffer_left = dirp->buffer_used - dirp->buffer_processed;
    if (buffer_left < sizeof(__wasi_dirent_t)) {
      // End-of-file.
      if (dirp->buffer_used < dirp->buffer_size)
        return NULL;
      goto read_entries;
    }
    __wasi_dirent_t entry;
    memcpy(&entry, dirp->buffer + dirp->buffer_processed, sizeof(entry));

    size_t entry_size = sizeof(__wasi_dirent_t) + entry.d_namlen;
    if (entry.d_namlen == 0) {
      // Invalid pathname length. Skip the entry.
      dirp->buffer_processed += entry_size;
      continue;
    }

    // The entire entry must be present in buffer space. If not, read
    // the entry another time. Ensure that the read buffer is large
    // enough to fit at least this single entry.
    if (buffer_left < entry_size) {
      GROW(dirp->buffer, dirp->buffer_size, entry_size);
      goto read_entries;
    }

    // Skip entries having null bytes in the filename.
    const char *name = dirp->buffer + dirp->buffer_processed + sizeof(entry);
    if (memchr(name, '\0', entry.d_namlen) != NULL) {
      dirp->buffer_processed += entry_size;
      continue;
    }

    // Return the next directory entry. Ensure that the dirent is large
    // enough to fit the filename.
    GROW(dirp->dirent, dirp->dirent_size,
         offsetof(struct dirent, d_name) + entry.d_namlen + 1);
    struct dirent *dirent = dirp->dirent;
    dirent->d_type = entry.d_type;
    memcpy(dirent->d_name, name, entry.d_namlen);
    dirent->d_name[entry.d_namlen] = '\0';

    // `fd_readdir` implementations may set the inode field to zero if the
    // the inode number is unknown. In that case, do an `fstatat` to get the
    // inode number.
    off_t d_ino = entry.d_ino;
    unsigned char d_type = entry.d_type;
    if (d_ino == 0 && strcmp(dirent->d_name, "..") != 0) {
      struct stat statbuf;
      if (fstatat(dirp->fd, dirent->d_name, &statbuf, AT_SYMLINK_NOFOLLOW) != 0) {
	if (errno == ENOENT) {
	  // The file disappeared before we could read it, so skip it.
	  dirp->buffer_processed += entry_size;
	  continue;
	}
        return NULL;
      }

      // Fill in the inode.
      d_ino = statbuf.st_ino;

      // In case someone raced with us and replaced the object with this name
      // with another of a different type, update the type too.
      d_type = __wasilibc_iftodt(statbuf.st_mode & S_IFMT);
    }
    dirent->d_ino = d_ino;
    dirent->d_type = d_type;

    dirp->cookie = entry.d_next;
    dirp->buffer_processed += entry_size;
    return dirent;

  read_entries:
    // Discard data currently stored in the input buffer.
    dirp->buffer_used = dirp->buffer_processed = dirp->buffer_size;

    // Load more directory entries and continue.
    __wasi_errno_t error =
        // TODO: Remove the cast on `dirp->buffer` once the witx is updated with char8 support.
        __wasi_fd_readdir(dirp->fd, (uint8_t *)dirp->buffer, dirp->buffer_size,
                                  dirp->cookie, &dirp->buffer_used);
    if (error != 0) {
      errno = error;
      return NULL;
    }
    dirp->buffer_processed = 0;
  }
}

#elif defined(__wasip2__) || defined(__wasip3__)

static int ensure_has_directory_stream(DIR *dirp, filesystem_borrow_descriptor_t *handle) {
  if (fd_to_file_handle(dirp->fd, handle) < 0)
    return -1;

#ifdef __wasip2__
  if (dirp->stream.__handle != 0)
    return 0;

  filesystem_error_code_t error_code;
  bool ok = filesystem_method_descriptor_read_directory(*handle,
                                                        &dirp->stream,
                                                        &error_code);
  if (!ok) {
    translate_error(error_code);
    return -1;
  }
#elif defined(__wasip3__)
  if (dirp->stream.f0 == 0) {
    wasip3_subtask_status_t status = filesystem_method_descriptor_read_directory(*handle, &dirp->stream);
    wasip3_subtask_block_on(status);
  }
#endif
  return 0;
}

static struct dirent *readdir_next(DIR *dirp) {
  filesystem_metadata_hash_value_t metadata;
  filesystem_error_code_t error_code;
  filesystem_borrow_descriptor_t dir_handle;

  if (ensure_has_directory_stream(dirp, &dir_handle) < 0)
    return NULL;

  // Yield '.' first if the offset is 0. Note that `d_ino` is from the metadata
  // hash of the directory itself.
  if (dirp->offset == 0) {
    dirp->offset += 1;
    GROW(dirp->dirent, dirp->dirent_size, offsetof(struct dirent, d_name) + 2);
#ifdef __wasip2__
    bool ok = filesystem_method_descriptor_metadata_hash(dir_handle,
                                                         &metadata,
                                                         &error_code);
    if (!ok) {
      translate_error(error_code);
      return NULL;
    }
#else
    filesystem_result_metadata_hash_value_error_code_t result;
    wasip3_subtask_status_t status = filesystem_method_descriptor_metadata_hash(dir_handle, &result);
    wasip3_subtask_block_on(status);
    if (result.is_err) {
      translate_error(result.val.err);
      return NULL;
    }
    metadata = result.val.ok;
#endif
    dirp->dirent->d_ino = metadata.lower;
    dirp->dirent->d_type = DT_DIR;
    dirp->dirent->d_name[0] = '.';
    dirp->dirent->d_name[1] = 0;
    return dirp->dirent;
  }

  // Yield '..' next if the offset is 1. Note that `d_ino` is set to 0 to
  // avoid opening the parent directory here.
  if (dirp->offset == 1) {
    dirp->offset += 1;
    GROW(dirp->dirent, dirp->dirent_size, offsetof(struct dirent, d_name) + 3);
    dirp->dirent->d_ino = 0;
    dirp->dirent->d_type = DT_DIR;
    dirp->dirent->d_name[0] = '.';
    dirp->dirent->d_name[1] = '.';
    dirp->dirent->d_name[2] = 0;
    return dirp->dirent;
  }

#ifdef __wasip2__
  filesystem_borrow_directory_entry_stream_t stream = filesystem_borrow_directory_entry_stream(dirp->stream);
  filesystem_option_directory_entry_t dir_entry_optional;
  bool ok = filesystem_method_directory_entry_stream_read_directory_entry(stream,
                                                                          &dir_entry_optional,
                                                                          &error_code);
  if (!ok) {
    translate_error(error_code);
    return NULL;
  }

  // Reached end-of-directory? Return null.
  if (!dir_entry_optional.is_some)
    return NULL;

  filesystem_directory_entry_t dir_entry = dir_entry_optional.val;

#elif defined(__wasip3__)
  filesystem_directory_entry_t dir_entry;
  wasip3_waitable_status_t status = filesystem_stream_directory_entry_read(dirp->stream.f0, &dir_entry, 1);
  ssize_t amount = wasip3_waitable_block_on(status, dirp->stream.f0);
  if (amount<1)
    return NULL;
#endif

  // Ensure that the dirent is large enough to fit the filename
  size_t the_size = offsetof(struct dirent, d_name);
  GROW(dirp->dirent, dirp->dirent_size, the_size + dir_entry.name.len + 1);

  // Fill out `d_type` and `d_name`
  dirp->dirent->d_type = dir_entry_type_to_d_type(dir_entry.type);
  memcpy(dirp->dirent->d_name, dir_entry.name.ptr, dir_entry.name.len);
  dirp->dirent->d_name[dir_entry.name.len] = '\0';

  // Fill out `d_ino` with the metadata hash.
  filesystem_path_flags_t path_flags = 0; // Don't follow symlinks
#ifdef __wasip2__
  ok = filesystem_method_descriptor_metadata_hash_at(dir_handle,
                                                     path_flags,
                                                     &dir_entry.name,
                                                     &metadata,
                                                     &error_code);
  wasip2_string_free(&dir_entry.name);
  if (!ok) {
    translate_error(error_code);
    return NULL;
  }
#else
  filesystem_result_metadata_hash_value_error_code_t result;
  status = filesystem_method_descriptor_metadata_hash_at(dir_handle, path_flags, dir_entry.name, &result);
  wasip3_subtask_block_on(status);
  //wasip3_string_free(&dir_entry.name);
  if (result.is_err) {
    translate_error(result.val.err);
    return NULL;
  }
  metadata = result.val.ok;
#endif
  dirp->dirent->d_ino = metadata.lower;
  dirp->offset += 1;

  return dirp->dirent;
}

struct dirent *readdir(DIR *dirp) {
  struct dirent *result = readdir_next(dirp);
  while (result != NULL && dirp->skip > 0) {
    dirp->skip -= 1;
    result = readdir_next(dirp);
  }
  return result;
}
#else
# error "Unknown WASI version"
#endif
