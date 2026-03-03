#include <errno.h>
#include <wasi/version.h>

#ifdef __wasip3__
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <wasi/descriptor_table.h>
#include <wasi/wasip3.h>
#include <wasi/wasip3_block.h>

#define KNOWN_NOT_A_TERMINAL -1

typedef struct {
  // input stream and future for result<_, error-code-t>
  stdin_tuple2_stream_u8_future_result_void_error_code_t input;
  // tristate: zero=unknown, valid handle=yes, -1=no
  terminal_input_own_terminal_input_t terminal_in;
} stdin3_t;

typedef struct {
  // contains stream, result storage and result subtask
  wasip3_subtask_t subtask;
  stdin_stream_u8_writer_t output;
  bool output_done;
  stdout_result_void_error_code_t pending_result;
  // tristate: zero=unknown, valid handle=yes, -1=no
  terminal_output_own_terminal_output_t terminal_out;
  // stream creation function (delayed)
  wasip3_subtask_status_t (*stream_func)(
      stdin_stream_u8_t data, stdout_result_void_error_code_t *result);
  // function to determine whether this is a terminal (delayed)
  bool (*terminal_func)(terminal_stdout_own_terminal_output_t *ret);
} stdout3_t;

static void translate_error(wasi_cli_types_error_code_t err) {
  switch (err) {
  case WASI_CLI_TYPES_ERROR_CODE_IO:
    errno = EIO;
    break;
  case WASI_CLI_TYPES_ERROR_CODE_ILLEGAL_BYTE_SEQUENCE:
    errno = EILSEQ;
    break;
  case WASI_CLI_TYPES_ERROR_CODE_PIPE:
    errno = EPIPE;
    break;
  default:
    assert(0);
    break;
  }
}

static void stdin3_free(void *data) {
  stdin3_t *stdio = (stdin3_t *)data;
  if (stdio->terminal_in.__handle > 0)
    terminal_input_terminal_input_drop_own(stdio->terminal_in);
  if (stdio->input.f1)
    stdin_future_result_void_error_code_drop_readable(stdio->input.f1);
  if (stdio->input.f0)
    stdin_stream_u8_drop_readable(stdio->input.f0);
  free(stdio);
}

static void stdout3_free(void *data) {
  stdout3_t *stdio = (stdout3_t *)data;
  if (stdio->terminal_out.__handle > 0)
    terminal_output_terminal_output_drop_own(stdio->terminal_out);
  if (stdio->output)
    stdin_stream_u8_drop_writable(stdio->output);
  if (stdio->subtask)
    wasip3_subtask_block_on_and_drop(stdio->subtask);
  free(stdio);
}

static int stdout3_write_eof(void *data) {
  stdout3_t *stdio = (stdout3_t *)data;
  if (stdio->subtask != 0) {
    wasip3_subtask_block_on_and_drop(stdio->subtask);
    stdio->subtask = 0;
  }
  if (stdio->pending_result.is_err) {
    translate_error(stdio->pending_result.val.err);
    return -1;
  }
  return 0;
}

static int stdout3_write(void *data, wasi_write_t *out) {
  stdout3_t *stdio = (stdout3_t *)data;
  if (!stdio->output) {
    assert(!stdio->subtask);
    stdin_stream_u8_t read_side = stdin_stream_u8_new(&stdio->output);
    wasip3_subtask_status_t status = (*stdio->stream_func)(
        read_side, &stdio->pending_result);
    if (WASIP3_SUBTASK_STATE(status) != WASIP3_SUBTASK_RETURNED)
      stdio->subtask = WASIP3_SUBTASK_HANDLE(status);
  }
  out->offset = NULL;
  out->blocking = true;
  out->timeout = 0;
  out->output = stdio->output;
  out->done = &stdio->output_done;
  out->eof = stdout3_write_eof;
  out->eof_data = data;
  return 0;
}

static int stdin3_read_eof(void *data) {
  stdin3_t *stdio = (stdin3_t *)data;

  if (stdio->input.f1 != 0) {
    stdin_result_void_error_code_t result;
    wasip3_future_block_on(
        stdin_future_result_void_error_code_read(stdio->input.f1, &result),
        stdio->input.f1);
    stdin_future_result_void_error_code_drop_readable(stdio->input.f1);
    stdio->input.f1 = 0;
    if (result.is_err) {
      translate_error(result.val.err);
      return -1;
    }
  }
  return 0;
}

static int stdin3_read(void *data, wasi_read_t *read) {
  stdin3_t *stdio = (stdin3_t *)data;
  if (!stdio->input.f0) {
    assert(!stdio->input.f1);
    stdin_read_via_stream(&stdio->input);
  }
  read->stream = stdio->input.f0;
  read->offset = NULL;
  read->blocking = true;
  read->timeout = 0;
  read->eof_data = data;
  read->eof = stdin3_read_eof;
  return 0;
}

static int stdio3_fstat(void *data, struct stat *buf) {
  (void)data;
  memset(buf, 0, sizeof(*buf));
  return 0;
}

static int stdin3_fcntl_getfl(void *data) {
  (void)data;
  return O_RDONLY;
}

static int stdout3_fcntl_getfl(void *data) {
  (void)data;
  return O_WRONLY;
}

static int stdin3_isatty(void *data) {
  stdin3_t *stdio = (stdin3_t *)data;
  if (stdio->terminal_in.__handle == 0) {
    if (!terminal_stdin_get_terminal_stdin(&stdio->terminal_in))
      stdio->terminal_in.__handle = KNOWN_NOT_A_TERMINAL;
  }
  return stdio->terminal_in.__handle > 0;
}

static int stdout3_isatty(void *data) {
  stdout3_t *stdio = (stdout3_t *)data;
  if (stdio->terminal_out.__handle == 0) {
    if (!(*stdio->terminal_func)(&stdio->terminal_out))
      stdio->terminal_out.__handle = KNOWN_NOT_A_TERMINAL;
  }
  return stdio->terminal_out.__handle > 0;
}

static descriptor_vtable_t stdin3_vtable = {
    .free = stdin3_free,
    .get_read_stream = stdin3_read,
    .fstat = stdio3_fstat,
    .fcntl_getfl = stdin3_fcntl_getfl,
    .isatty = stdin3_isatty,
};

static descriptor_vtable_t stdout3_vtable = {
    .free = stdout3_free,
    .get_write_stream = stdout3_write,
    .fstat = stdio3_fstat,
    .fcntl_getfl = stdout3_fcntl_getfl,
    .isatty = stdout3_isatty,
};

static int stdio_add_input() {
  stdin3_t *stdio = calloc(1, sizeof(stdin3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  descriptor_table_entry_t entry;
  entry.vtable = &stdin3_vtable;
  entry.data = stdio;
  return descriptor_table_insert(entry);
}

static int stdio3_add_output(
    wasip3_subtask_status_t (*stream_func)(
        stdin_stream_u8_t data, stdout_result_void_error_code_t *result),
    bool (*terminal_func)(terminal_stdout_own_terminal_output_t *ret)) {
  stdout3_t *stdio = calloc(1, sizeof(stdout3_t));
  if (!stdio) {
    errno = ENOMEM;
    return -1;
  }
  stdio->stream_func = stream_func;
  stdio->terminal_func = terminal_func;

  descriptor_table_entry_t entry;
  entry.vtable = &stdout3_vtable;
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
          (wasip3_subtask_status_t(*)(
              stdin_stream_u8_t,
              stdout_result_void_error_code_t *))stderr_write_via_stream,
          terminal_stderr_get_terminal_stderr) < 0)
    return -1;
  return 0;
}
#endif // __wasip3__
