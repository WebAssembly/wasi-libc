#ifdef __wasilibc_use_wasip2

#include <errno.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/stdio.h>
#include <wasi/wasip2.h>

typedef struct {
  int fd;
  // Lazily initialized streams. Depending on `fd` these may never be
  // initialized, for example stdin can never have an output stream.
  streams_own_input_stream_t input;
  streams_own_output_stream_t output;
  // Lazily initialized pollables derived from input/output.
  poll_own_pollable_t input_pollable;
  poll_own_pollable_t output_pollable;
} stdio_t;

static void stdio_free(void *data) {
  stdio_t *stdio = (stdio_t *)data;
  if (stdio->input_pollable.__handle != 0) {
    poll_pollable_drop_own(stdio->input_pollable);
  }
  if (stdio->output_pollable.__handle != 0) {
    poll_pollable_drop_own(stdio->output_pollable);
  }
  if (stdio->input.__handle != 0) {
    streams_input_stream_drop_own(stdio->input);
  }
  if (stdio->output.__handle != 0) {
    streams_output_stream_drop_own(stdio->output);
  }
  free(stdio);
}

static int stdio_get_read_stream(void *data,
                                 streams_borrow_input_stream_t *out_stream,
                                 off_t **out_offset,
                                 poll_own_pollable_t **out_pollable) {
  stdio_t *stdio = (stdio_t *)data;
  if (stdio->fd != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }
  if (stdio->input.__handle == 0)
    stdio->input = stdin_get_stdin();
  *out_stream = streams_borrow_input_stream(stdio->input);
  if (out_offset)
    *out_offset = NULL;
  if (out_pollable)
    *out_pollable = &stdio->input_pollable;
  return 0;
}

static int stdio_get_write_stream(void *data,
                                  streams_borrow_output_stream_t *out_stream,
                                  off_t **out_offset,
                                  poll_own_pollable_t **out_pollable) {
  stdio_t *stdio = (stdio_t *)data;
  if (stdio->output.__handle == 0) {
    if (stdio->fd == 1) {
      stdio->output = stdout_get_stdout();
    } else if (stdio->fd == 2) {
      stdio->output = stderr_get_stderr();
    } else {
      errno = EOPNOTSUPP;
      return -1;
    }
  }
  *out_stream = streams_borrow_output_stream(stdio->output);
  if (out_offset)
    *out_offset = NULL;
  if (out_pollable)
    *out_pollable = &stdio->output_pollable;
  return 0;
}

static int stdio_fstat(void *data, struct stat *buf) {
  memset(buf, 0, sizeof(*buf));
  return 0;
}

static descriptor_vtable_t stdio_vtable = {
  .free = stdio_free,
  .get_read_stream = stdio_get_read_stream,
  .get_write_stream = stdio_get_write_stream,
  .fstat = stdio_fstat,
};

static int stdio_add(int fd) {
  stdio_t *stdio = calloc(1, sizeof(stdio_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdio->fd = fd;

  descriptor_table_entry_t entry;
  entry.vtable = &stdio_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

int __wasilibc_init_stdio() {
  if (stdio_add(0) < 0)
    return -1;
  if (stdio_add(1) < 0)
    return -1;
  if (stdio_add(2) < 0)
    return -1;
  return 0;
}
#endif // __wasilibc_use_wasip2
