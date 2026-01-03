#include <errno.h>
#include <wasi/api.h>

#ifdef __wasip3__
#include <assert.h>
#include <common/errors.h>
#include <libc/sys/stat/stat_impl.h>
#include <wasi/descriptor_table.h>

typedef struct {
  filesystem_own_descriptor_t file_handle;
  off_t offset;
  int oflag;
} file3_t;

static void file_close_streams(void *data) {
  file3_t *file = (file3_t *)data;
  // nothing yet?
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

static void block_on(wasip3_subtask_status_t status) {
  if (status != WASIP3_SUBTASK_RETURNED) {
    if (WASIP3_SUBTASK_STATE(status) == WASIP3_SUBTASK_STARTED) {
      wasip3_subtask_t handle = WASIP3_SUBTASK_HANDLE(status);
      wasip3_waitable_set_t set = wasip3_waitable_set_new();
      wasip3_waitable_join(handle, set);
      wasip3_event_t event;
      wasip3_waitable_set_wait(set, &event);
      assert(event.event == WASIP3_EVENT_SUBTASK);
      assert(event.code == WASIP3_SUBTASK_RETURNED);
      wasip3_subtask_drop(event.waitable);
      wasip3_waitable_set_drop(set);
    } else {
      abort();
    }
  }
}

static int file_fstat(void *data, struct stat *buf) {
  file3_t *file = (file3_t *)data;
  // Get the metadata hash for the file descriptor
  filesystem_result_metadata_hash_value_error_code_t result;
  wasip3_subtask_status_t status = filesystem_method_descriptor_metadata_hash(
      filesystem_borrow_descriptor(file->file_handle), &result);
  block_on(status);
  if (result.is_err) {
    translate_error(result.val.err);
    return -1;
  }

  // Get the file metadata
  filesystem_result_descriptor_stat_error_code_t stat_result;
  status = filesystem_method_descriptor_stat(
      filesystem_borrow_descriptor(file->file_handle), &stat_result);
  block_on(status);
  if (stat_result.is_err) {
    translate_error(stat_result.val.err);
    return -1;
  }

  // Convert the internal data to an external struct
  to_public_stat(&result.val.ok, &stat_result.val.ok, buf);
  return 0;
}

static int file_seek_end(file3_t *file) { abort(); }

static off_t file_seek(void *data, off_t offset, int whence) { abort(); }

static int file_set_blocking(void *data, bool blocking) { abort(); }

static int file_fcntl_getfl(void *data) { abort(); }

static int file_fcntl_setfl(void *data, int flags) { abort(); }

static int file_read_stream(void *data, filesystem_stream_u8_t *out,
                            off_t **offs) {
  file3_t *file = (file3_t *)data;
  filesystem_tuple2_stream_u8_future_result_void_error_code_t res;
  filesystem_method_descriptor_read_via_stream(
      filesystem_borrow_descriptor(file->file_handle), file->offset, &res);
  *out = res.f0;
  // TODO: I think we need to return f1 as well.
  filesystem_future_result_void_error_code_drop_readable(res.f1);
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
