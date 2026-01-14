#include <errno.h>
#include <wasi/version.h>

#ifdef __wasip3__
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/wasip3.h>

typedef struct {
  stdin_stream_u8_t input;
  stdin_future_result_void_error_code_t future;
  terminal_input_own_terminal_input_t terminal_in;

  stdin_stream_u8_writer_t stdout;
  // contents will be filled by host (once stdout has an error)
  stdout_result_void_error_code_t stdout_result;
  wasip3_subtask_t stdout_task;
  terminal_output_own_terminal_output_t terminal_out;
} stdio3_t;

static void stdio3_free(void *data) {
  stdio3_t *stdio = (stdio3_t *)data;
  if (stdio->terminal_in.__handle)
    terminal_input_terminal_input_drop_own(stdio->terminal_in);
  if (stdio->future)
    stdin_future_result_void_error_code_drop_readable(stdio->future);
  if (stdio->input)
    stdin_stream_u8_drop_readable(stdio->input);

  if (stdio->terminal_out.__handle)
    terminal_output_terminal_output_drop_own(stdio->terminal_out);
  if (stdio->stdout_task)
    wasip3_subtask_cancel(stdio->stdout_task);
  if (stdio->stdout)
    stdin_stream_u8_drop_writable(stdio->stdout);
  free(stdio);
}

static int stdio3_write(void *data, void const *buf, size_t nbyte,
                        waitable_t *waitable, wasip3_waitable_status_t *out,
                        off_t **offs) {
  stdio3_t *stdio = (stdio3_t *)data;
  if (!stdio->stdout) {
    errno = EBADF;
    return -1;
  }
  *waitable = stdio->stdout;
  *out = stdin_stream_u8_write(stdio->stdout, buf, nbyte);
  *offs = NULL;
  return 0;
}

static int stdio3_read(void *data, void *buf, size_t nbyte,
                       waitable_t *waitable, wasip3_waitable_status_t *out,
                       off_t **offs) {
  stdio3_t *stdio = (stdio3_t *)data;
  if (!stdio->input) {
    errno = EBADF;
    return -1;
  }
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
  if (stdio->stdout == 0) {
    return O_RDONLY;
  } else {
    return O_WRONLY;
  }
}

static int stdio3_isatty(void *data) {
  stdio3_t *stdio = (stdio3_t *)data;
  return stdio->terminal_in.__handle != 0 || stdio->terminal_out.__handle != 0;
}

static descriptor_vtable_t stdio3_vtable = {
    .free = stdio3_free,
    .read3 = stdio3_read,
    .write3 = stdio3_write,
    .fstat = stdio3_fstat,
    .fcntl_getfl = stdio3_fcntl_getfl,
    .isatty = stdio3_isatty,
};

static int stdio_add_input() {
  stdio3_t *stdio = calloc(1, sizeof(stdio3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdin_tuple2_stream_u8_future_result_void_error_code_t stdin;
  stdin_read_via_stream(&stdin);

  if (!terminal_stdin_get_terminal_stdin(&stdio->terminal_in))
    stdio->terminal_in.__handle = 0;

  stdio->input = stdin.f0;
  stdio->future = stdin.f1;

  descriptor_table_entry_t entry;
  entry.vtable = &stdio3_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

static int stdio3_add_output(
    // int fd,
    wasip3_subtask_status_t (*func)(stdin_stream_u8_t data,
                                    stdout_result_void_error_code_t *result),
    bool (*terminal)(terminal_stdout_own_terminal_output_t *ret)) {
  stdio3_t *stdio = calloc(1, sizeof(stdio3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdin_stream_u8_t read_side = stdin_stream_u8_new(&stdio->stdout);
  wasip3_subtask_status_t res = (*func)(read_side, &stdio->stdout_result);
  stdio->stdout_task = WASIP3_SUBTASK_HANDLE(res);

  if (!(*terminal)(&stdio->terminal_out))
    stdio->terminal_out.__handle = 0;

  descriptor_table_entry_t entry;
  entry.vtable = &stdio3_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

int __wasilibc_init_stdio() {
  if (stdio_add_input() < 0)
    return -1;
  if (stdio3_add_output(stdout_write_via_stream,
                        terminal_stdout_get_terminal_stdout) < 0)
    return -1;
  // assuming that stdout and stderr functions are compatible
  if (stdio3_add_output(
          (wasip3_subtask_status_t (*)(
              stdin_stream_u8_t,
              stdout_result_void_error_code_t *))stderr_write_via_stream,
          terminal_stderr_get_terminal_stderr) < 0)
    return -1;
  return 0;
}
#endif // __wasip3__
