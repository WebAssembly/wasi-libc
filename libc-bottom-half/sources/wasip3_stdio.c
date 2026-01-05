#include <errno.h>
#include <wasi/version.h>

#ifdef __wasip3__
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/wasip3.h>

typedef struct {
  int fd;

  stdin_stream_u8_t input;
  stdin_future_result_void_error_code_t future;

  stdin_stream_u8_writer_t stdout;
  // contents will be filled by host (once stdout has an error)
  stdout_result_void_error_code_t stdout_result;
  wasip3_subtask_t stdout_task;
} stdio3_t;

static void stdio3_free(void *data) {
  stdio3_t *stdio = (stdio3_t *)data;
  stdin_stream_u8_drop_readable(stdio->input);
  stdin_future_result_void_error_code_drop_readable(stdio->future);
  free(stdio);
}

static int stdio3_get_write_stream(void *data, void const *buf, size_t nbyte,
                                   waitable_t *waitable,
                                   wasip3_waitable_status_t *out,
                                   off_t **offs) {
  stdio3_t *stdio = (stdio3_t *)data;
  *waitable = stdio->stdout;
  *out = stdin_stream_u8_write(stdio->stdout, buf, nbyte);
  *offs = NULL;
  return 0;
}

static int stdio3_get_read_stream(void *data, void *buf, size_t nbyte,
                                  waitable_t *waitable,
                                  wasip3_waitable_status_t *out, off_t **offs) {
  stdio3_t *stdio = (stdio3_t *)data;
  *waitable = stdio->input;
  *out = stdin_stream_u8_read(stdio->input, buf, nbyte);
  *offs = NULL;
  return 0;
}

static int stdio3_fstat(void *data, struct stat *buf) {
  memset(buf, 0, sizeof(*buf));
  return 0;
}

static int stdio3_fcntl_getfl(void *data) {
  stdio3_t *stdio = (stdio3_t *)data;
  if (stdio->fd == 0) {
    return O_RDONLY;
  } else {
    return O_WRONLY;
  }
}

static int stdio3_isatty(void *data) { return 1; }

static descriptor_vtable_t stdio3_vtable = {
    .free = stdio3_free,
    .read3 = stdio3_get_read_stream,
    .write3 = stdio3_get_write_stream,
    .fstat = stdio3_fstat,
    .fcntl_getfl = stdio3_fcntl_getfl,
    .isatty = stdio3_isatty,
};

static int stdio_add_input(int fd, stdin_stream_u8_t stream,
                    stdin_future_result_void_error_code_t future) {
  stdio3_t *stdio = calloc(1, sizeof(stdio3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdio->fd = fd;
  stdio->input = stream;
  stdio->future = future;

  descriptor_table_entry_t entry;
  entry.vtable = &stdio3_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

static int stdio3_add_output(int fd, wasip3_subtask_status_t (*func)(
                                  stdin_stream_u8_t data,
                                  stdout_result_void_error_code_t *result)) {
  stdio3_t *stdio = calloc(1, sizeof(stdio3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdin_stream_u8_t read_side = stdin_stream_u8_new(&stdio->stdout);
  stdout_result_void_error_code_t stdout_result;
  wasip3_subtask_status_t res = (*func)(read_side, &stdio->stdout_result);
  if (WASIP3_SUBTASK_STATE(res) != WASIP3_SUBTASK_STARTED) {
    abort();
  }
  stdio->fd = fd;
  stdio->stdout_task = WASIP3_SUBTASK_HANDLE(res);

  descriptor_table_entry_t entry;
  entry.vtable = &stdio3_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

int __wasilibc_init_stdio() {
  stdin_tuple2_stream_u8_future_result_void_error_code_t stdin;
  stdin_read_via_stream(&stdin);
  int fd = stdio_add_input(0, stdin.f0, stdin.f1);
  if (fd != 0) {
    if (fd >= 0) {
      errno = EMFILE;
    }
    stdin_stream_u8_drop_readable(stdin.f0);
    stdin_future_result_void_error_code_drop_readable(stdin.f1);
    return -1;
  }
  fd = stdio3_add_output(1, stdout_write_via_stream);
  if (fd != 1) {
    abort();
  }
  // assuming that these functions are compatible
  fd = stdio3_add_output(
      2, (wasip3_subtask_status_t (*)(
             stdin_stream_u8_t,
             stdout_result_void_error_code_t *))stderr_write_via_stream);
  if (fd != 2) {
    abort();
  }
  return 0;
}
#endif // __wasip3__
