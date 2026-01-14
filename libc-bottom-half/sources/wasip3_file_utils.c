#include <assert.h>
#include <common/errors.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <wasi/file_utils.h>

#ifdef __wasip3__
ssize_t __wasilibc_write3(int fildes, void const *buf, size_t nbyte) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->write3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  off_t *off;
  waitable_t output_stream;
  wasip3_waitable_status_t status;
  if ((*entry->vtable->write3)(entry->data, buf, nbyte, &output_stream, &status,
                               &off) < 0)
    return -1;
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(output_stream, set);
    wasip3_event_t event;
    wasip3_waitable_set_wait(set, &event);
    assert(event.event == WASIP3_EVENT_STREAM_WRITE);
    assert(event.waitable == output_stream);
    // remove from set
    wasip3_waitable_join(output_stream, 0);
    wasip3_waitable_set_drop(set);
    ssize_t bytes_written = event.code;
    if (off)
      *off += bytes_written;
    return bytes_written;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED) {
    ssize_t bytes_written = WASIP3_WAITABLE_COUNT(status);
    if (off)
      *off += bytes_written;
    return bytes_written;
  } else {
    abort();
  }
}

ssize_t __wasilibc_read3(int fildes, void *buf, size_t nbyte) {
  descriptor_table_entry_t *entry = descriptor_table_get_ref(fildes);
  if (!entry)
    return -1;
  if (!entry->vtable->read3) {
    errno = EOPNOTSUPP;
    return -1;
  }
  off_t *off;
  waitable_t waitable;
  wasip3_waitable_status_t status;
  if ((*entry->vtable->read3)(entry->data, buf, nbyte, &waitable, &status,
                              &off) < 0)
    return -1;
  if (status == WASIP3_WAITABLE_STATUS_BLOCKED) {
    wasip3_waitable_set_t set = wasip3_waitable_set_new();
    wasip3_waitable_join(waitable, set);
    wasip3_event_t event;
    wasip3_waitable_set_wait(set, &event);
    assert(event.event == WASIP3_EVENT_STREAM_READ);
    assert(event.waitable == waitable);
    // remove from set
    wasip3_waitable_join(waitable, 0);
    wasip3_waitable_set_drop(set);
    ssize_t bytes_read = event.code;
    if (off)
      *off += bytes_read;
    return bytes_read;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_COMPLETED) {
    ssize_t bytes_read = WASIP3_WAITABLE_COUNT(status);
    if (off)
      *off += bytes_read;
    return bytes_read;
  } else if (WASIP3_WAITABLE_STATE(status) == WASIP3_WAITABLE_DROPPED) {
    return 0;
  } else {
    abort();
  }
}
#endif // __wasip3__
