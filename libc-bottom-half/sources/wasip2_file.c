#ifdef __wasilibc_use_wasip2

#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <fcntl.h>
#include <wasi/descriptor_table.h>
#include <wasi/file.h>
#include <wasi/wasip2.h>

#include "libc/sys/stat/stat_impl.h"

typedef struct {
  filesystem_own_descriptor_t file_handle;
  // Lazily initialized read/write streams which track the current position
  // in the file. Seeking will close these streams and cause them to be
  // reopened on the next operation, for example.
  streams_own_input_stream_t read_stream;
  streams_own_output_stream_t write_stream;
  // Current position in stream, relative to the beginning of the
  // *file_handle*, measured in bytes
  off_t offset;
  // Used for checking readiness to read/write to stream. Lazily initialized
  // and initially set to 0.
  streams_own_pollable_t read_pollable;
  streams_own_pollable_t write_pollable;
  int oflag;
} file_t;

static void file_close_streams(void *data) {
  file_t *file = (file_t *)data;
  if (file->read_pollable.__handle != 0) {
    poll_pollable_drop_own(file->read_pollable);
    file->read_pollable.__handle = 0;
  }
  if (file->write_pollable.__handle != 0) {
    poll_pollable_drop_own(file->write_pollable);
    file->write_pollable.__handle = 0;
  }
  if (file->read_stream.__handle != 0) {
    streams_input_stream_drop_own(file->read_stream);
    file->read_stream.__handle = 0;
  }
  if (file->write_stream.__handle != 0) {
    streams_output_stream_drop_own(file->write_stream);
    file->write_stream.__handle = 0;
  }
}

static void file_free(void *data) {
  file_t *file = (file_t *)data;
  file_close_streams(data);
  filesystem_descriptor_drop_own(file->file_handle);
  free(file);
}

static int file_get_read_stream(void *data,
                                streams_borrow_input_stream_t *out_stream,
                                off_t **out_offset,
                                poll_own_pollable_t **out_pollable) {
  file_t *file = (file_t *)data;
  if (file->read_stream.__handle == 0) {
    filesystem_error_code_t error_code;
    bool ok = filesystem_method_descriptor_read_via_stream(
        filesystem_borrow_descriptor(file->file_handle),
        file->offset,
        &file->read_stream,
        &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }
  }
  *out_stream = streams_borrow_input_stream(file->read_stream);
  if (out_offset)
    *out_offset = &file->offset;
  if (out_pollable)
    *out_pollable = &file->read_pollable;
  return 0;
}

static int file_get_write_stream(void *data,
                                 streams_borrow_output_stream_t *out_stream,
                                 off_t **out_offset,
                                 poll_own_pollable_t **out_pollable) {
  file_t *file = (file_t *)data;
  if (file->write_stream.__handle == 0) {
    filesystem_error_code_t error_code;
    bool ok = filesystem_method_descriptor_write_via_stream(
        filesystem_borrow_descriptor(file->file_handle),
        file->offset,
        &file->write_stream,
        &error_code);
    if (!ok) {
      translate_error(error_code);
      return -1;
    }
  }
  *out_stream = streams_borrow_output_stream(file->write_stream);
  if (out_offset)
    *out_offset = &file->offset;
  if (out_pollable)
    *out_pollable = &file->write_pollable;
  return 0;
}

static int file_get_file(void *data,
                         filesystem_borrow_descriptor_t *out_file) {
  file_t *file = (file_t *)data;
  *out_file = filesystem_borrow_descriptor(file->file_handle);
  return 0;
}

static int file_fstat(void *data, struct stat *buf) {
  file_t *file = (file_t *)data;
  // Get the metadata hash for the file descriptor
  filesystem_metadata_hash_value_t metadata;
  filesystem_error_code_t error;
  if (!filesystem_method_descriptor_metadata_hash(filesystem_borrow_descriptor(file->file_handle),
                                                  &metadata,
                                                  &error)) {
    translate_error(error);
    return -1;
  }

  // Get the file metadata
  filesystem_descriptor_stat_t internal_stat;
  bool stat_result = filesystem_method_descriptor_stat(filesystem_borrow_descriptor(file->file_handle),
                                                       &internal_stat,
                                                       &error);
  if (!stat_result) {
    translate_error(error);
    return -1;
  }

  // Convert the internal data to an external struct
  to_public_stat(&metadata, &internal_stat, buf);
  return 0;
}

static off_t file_seek(void *data, off_t offset, int whence) {
  file_t *file = (file_t *)data;
  switch (whence) {
    case SEEK_SET:
      file->offset = offset;
      break;
    case SEEK_CUR:
      file->offset += offset;
      break;
    case SEEK_END: {
      // Get the file metadata
      filesystem_descriptor_stat_t stat;
      filesystem_error_code_t error;
      bool ok = filesystem_method_descriptor_stat(filesystem_borrow_descriptor(file->file_handle),
                                                  &stat,
                                                  &error);
      if (!ok) {
        translate_error(error);
        return -1;
      }
      file->offset = ((off_t) stat.size) + offset;
      break;
    }
    default:
      errno = EINVAL;
      return -1;
  }
  file_close_streams(data);
  return file->offset;
}

static int file_set_blocking(void *data, bool blocking) {
  file_t *file = (file_t *)data;
  if (blocking)
    file->oflag &= ~O_NONBLOCK;
  else
    file->oflag |= O_NONBLOCK;
  return 0;
}

static int file_fcntl_getfl(void *data) {
  file_t *file = (file_t *)data;

  // Get the flags of the descriptor
  filesystem_descriptor_flags_t flags;
  filesystem_error_code_t error_code;

  filesystem_borrow_descriptor_t file_handle = filesystem_borrow_descriptor(file->file_handle);
  if (!filesystem_method_descriptor_get_flags(file_handle, &flags, &error_code)) {
    translate_error(error_code);
    return -1;
  }

  int oflags = file->oflag & (O_NONBLOCK | O_APPEND);
  if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_READ) {
    if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_WRITE)
      oflags |= O_RDWR;
    else
      oflags |= O_RDONLY;
  } else if (flags & FILESYSTEM_DESCRIPTOR_FLAGS_WRITE) {
      oflags |= O_WRONLY;
  } else {
      oflags |= O_SEARCH;
  }
  return oflags;
}

static int file_fcntl_setfl(void *data, int flags) {
  file_t *file = (file_t *)data;
  flags &= O_NONBLOCK | O_APPEND;
  file->oflag = (file->oflag & ~(O_NONBLOCK | O_APPEND)) | flags;
  return 0;
}

static descriptor_vtable_t file_vtable = {
  .free = file_free,
  .get_read_stream = file_get_read_stream,
  .get_write_stream = file_get_write_stream,
  .get_file = file_get_file,
  .set_blocking = file_set_blocking,
  .fstat = file_fstat,
  .seek = file_seek,
  .close_streams = file_close_streams,
  .fcntl_getfl = file_fcntl_getfl,
  .fcntl_setfl = file_fcntl_setfl,
};

int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag) {
  file_t *file = calloc(1, sizeof(file_t));
  if (!file) {
    filesystem_descriptor_drop_own(file_handle);
    errno = ENOMEM;
    return -1;
  }
  assert(file_handle.__handle != 0);
  file->file_handle = file_handle;
  file->oflag = oflag;

  descriptor_table_entry_t entry;
  entry.vtable = &file_vtable;
  entry.data = file;
  return descriptor_table_insert(entry);
}

#endif // __wasilibc_use_wasip2
