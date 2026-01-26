#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <assert.h>
#include <common/errors.h>
#include <libc/sys/stat/stat_impl.h>
#include <wasi/descriptor_table.h>
#include <wasi/file_utils.h>
#include <wasi/wasip3_block.h>

typedef struct {
  filesystem_own_descriptor_t file_handle;
  off_t offset;
  int oflag;
  filesystem_tuple2_stream_u8_future_result_void_error_code_t read;
  wasip3_write_t write;
} file3_t;

static void file_close_streams(void *data) {
  file3_t *file = (file3_t *)data;
  if (file->read.f0 != 0) {
    filesystem_stream_u8_drop_readable(file->read.f0);
    filesystem_future_result_void_error_code_drop_readable(file->read.f1);
  }
  if (file->write.output != 0) {
    // TODO: This fails
    // if (file->write.subtask)
    //   wasip3_subtask_cancel(file->write.subtask);
    sockets_stream_u8_drop_writable(file->write.output);
  }
}

static void file_free(void *data) {
  file3_t *file = (file3_t *)data;
  file_close_streams(data);
  filesystem_descriptor_drop_own(file->file_handle);
  free(file);
}

static int file_get_file(void *data, filesystem_borrow_descriptor_t *out_file) {
  file3_t *file = (file3_t *)data;
  *out_file = filesystem_borrow_descriptor(file->file_handle);
  return 0;
}

static int file_fstat(void *data, struct stat *buf) {
  file3_t *file = (file3_t *)data;
  // Get the metadata hash for the file descriptor
  filesystem_result_metadata_hash_value_error_code_t result;
  wasip3_subtask_status_t status = filesystem_method_descriptor_metadata_hash(
      filesystem_borrow_descriptor(file->file_handle), &result);
  wasip3_subtask_block_on(status);
  if (result.is_err) {
    translate_error(result.val.err);
    return -1;
  }

  // Get the file metadata
  filesystem_result_descriptor_stat_error_code_t stat_result;
  status = filesystem_method_descriptor_stat(
      filesystem_borrow_descriptor(file->file_handle), &stat_result);
  wasip3_subtask_block_on(status);
  if (stat_result.is_err) {
    translate_error(stat_result.val.err);
    return -1;
  }

  // Convert the internal data to an external struct
  to_public_stat(&result.val.ok, &stat_result.val.ok, buf);
  return 0;
}

static int file_seek_end(file3_t *file) {
  filesystem_result_descriptor_stat_error_code_t result;
  wasip3_subtask_status_t status = filesystem_method_descriptor_stat(
      filesystem_borrow_descriptor(file->file_handle), &result);
  wasip3_subtask_block_on(status);
  if (result.is_err) {
    translate_error(result.val.err);
    return -1;
  }
  file->offset = (off_t)result.val.ok.size;
  return 0;
}

static off_t file_seek(void *data, off_t offset, int whence) {
  file3_t *file = (file3_t *)data;

  // If this file is in append mode, reset our knowledge of the current cursor
  // to the current end of the file.
  if ((file->oflag & O_APPEND) && file_seek_end(file) < 0)
    return -1;

  off_t result;
  switch (whence) {
  case SEEK_SET:
    result = offset;
    break;
  case SEEK_CUR:
    result = file->offset + offset;
    break;
  case SEEK_END: {
    // If we're in append mode we already reset to the end, but if we're not
    // in append mode then do the reset to the end here.
    if (!(file->oflag & O_APPEND) && file_seek_end(file) < 0)
      return -1;
    result = file->offset + offset;
    break;
  }
  default:
    errno = EINVAL;
    return -1;
  }
  if (result < 0) {
    errno = EINVAL;
    return -1;
  }
  file->offset = result;
  file_close_streams(data);
  return file->offset;
}

static int file_set_blocking(void *data, bool blocking) { abort(); }

static int file_fcntl_getfl(void *data) { abort(); }

static int file_fcntl_setfl(void *data, int flags) { abort(); }

static int file_read_stream(
    void *data,
    filesystem_tuple2_stream_u8_future_result_void_error_code_t **out,
    off_t **offs) {
  file3_t *file = (file3_t *)data;
  if (file->read.f0 == 0) {
    filesystem_method_descriptor_read_via_stream(
        filesystem_borrow_descriptor(file->file_handle), file->offset,
        &file->read);
  }
  *out = &file->read;
  *offs = &file->offset;
  return 0;
}

static int file_write_stream(void *data, wasip3_write_t **out, off_t **offs) {
  file3_t *file = (file3_t *)data;
  if (file->write.output == 0) {
    filesystem_stream_u8_t write_read =
        filesystem_stream_u8_new(&file->write.output);
    file->write.subtask = filesystem_method_descriptor_write_via_stream(
        filesystem_borrow_descriptor(file->file_handle), write_read,
        file->offset, &file->write.pending_result);
  }
  *out = &file->write;
  *offs = &file->offset;
  return 0;
}

static descriptor_vtable_t file_vtable = {
    .free = file_free,
    .get_file = file_get_file,
    .set_blocking = file_set_blocking,
    .fstat = file_fstat,
    .seek = file_seek,
    .close_streams = file_close_streams,
    .fcntl_getfl = file_fcntl_getfl,
    .fcntl_setfl = file_fcntl_setfl,
    .get_read_stream3 = file_read_stream,
    .get_write_stream3 = file_write_stream,
};

int __wasilibc_add_file(filesystem_own_descriptor_t file_handle, int oflag) {
  file3_t *file = calloc(1, sizeof(file3_t));
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

#endif // __wasip3__
