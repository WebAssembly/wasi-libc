#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <wasi/__generated_webp2.h>
#include <wasi/report-error.h>
#include <wasi/utf8.h>
#include <wasi/webp2_stdio.h>

#define LINE_BUF_SIZE 4096

typedef struct line_buffer_t {
  bool warn;
  uint8_t buf[LINE_BUF_SIZE];
} line_buffer_t;

static line_buffer_t line_buffers[2] = {
    (line_buffer_t){},
    (line_buffer_t){.warn = true},
};
static wasilibc_utf8_writer_t utf8_writers[2] = {
    __wasilibc_make_utf8_writer(line_buffers[0].buf, LINE_BUF_SIZE),
    __wasilibc_make_utf8_writer(line_buffers[1].buf, LINE_BUF_SIZE),
};

static bool fd_closed[3];

static void line_buffer_out(line_buffer_t *lb, wasilibc_utf8_writer_t *utf8) {
  webp2_string_t out =
      (webp2_string_t){.ptr = utf8->buf, .len = utf8->valid_len};
  if (lb->warn) {
    webp2_console_warn(&out);
  } else {
    webp2_console_log(&out);
  }
}

// Flushes all data in line buffer to the console, replacing any trailing
// multi-byte code points with the replacement character.
static void finish_and_flush(int fd) {
  assert(fd == STDOUT_FILENO || fd == STDERR_FILENO);
  if (fd_closed[fd]) {
    return;
  }
  wasilibc_utf8_writer_t *utf8 = &utf8_writers[fd - 1];
  if (!__wasilibc_utf8_abort_current(utf8)) {
    // Emit what is already valid in the buffer first, and carry over the
    // in-progress codepoint
    __wasilibc_webp2_fflush(fd);
  }
  __wasilibc_utf8_abort_current(utf8);
  __wasilibc_webp2_fflush(fd);
}

bool __wasilibc_webp2_fd_ok(int fd) {
  return STDIN_FILENO <= fd && fd <= STDERR_FILENO && !fd_closed[fd];
}

bool __wasilibc_webp2_is_readable(int fd) {
  return fd == STDIN_FILENO && !fd_closed[fd];
}

bool __wasilibc_webp2_is_writable(int fd) {
  return (fd == STDOUT_FILENO || fd == STDERR_FILENO) && !fd_closed[fd];
}

bool __wasilibc_webp2_iovecs_ok(const struct iovec *iov, int iovcnt,
                                size_t *total_size) {
  size_t _total_size = 0;
  if (total_size) {
    *total_size = 0;
  }
  for (int i = 0; i < iovcnt; i++) {
    const struct iovec *v = &iov[i];
    if (v->iov_len > SSIZE_MAX || _total_size > SSIZE_MAX - v->iov_len) {
      return false;
    }
    _total_size += v->iov_len;
  }
  if (total_size) {
    *total_size = _total_size;
  }
  return true;
}

void __wasilibc_webp2_write(int fd, const void *buf, size_t nbyte) {
  assert(__wasilibc_webp2_is_writable(fd));
  line_buffer_t *line_buffer = &line_buffers[fd - 1];
  wasilibc_utf8_writer_t *utf8 = &utf8_writers[fd - 1];
  for (size_t i = 0; i < nbyte; i++) {
    uint8_t b = ((uint8_t *)buf)[i];
    // First write to the buffer, flushing first if there was no room for the
    // results (which can occur either because the buffer was completely full
    // or because there was no room for a replacement char).
    bool already_logged = false;
    if (!__wasilibc_utf8_write(utf8, b)) {
      line_buffer_out(line_buffer, utf8);
      __wasilibc_utf8_reset(utf8);
      __wasilibc_utf8_write(utf8, b); // infallible
      already_logged = true;
    }
    // Then, if there was a newline, drop it from the buffer and flush.
    if (b == '\n') {
      assert(utf8->valid_len == utf8->raw_len);
      utf8->raw_len--;
      utf8->valid_len--;
      bool skip = utf8->valid_len == 0 && already_logged;
      if (!skip) {
        line_buffer_out(line_buffer, utf8);
      }
      __wasilibc_utf8_reset(utf8);
    }
  }
}

void __wasilibc_webp2_close(int fd) {
  assert(__wasilibc_webp2_fd_ok(fd));
  if (__wasilibc_webp2_is_writable(fd)) {
    // Don't flush musl's buffers, but do flush everything we have.
    finish_and_flush(fd);
  }
  fd_closed[fd] = true;
}

void __wasilibc_webp2_fflush(int fd) {
  assert(fd == STDOUT_FILENO || fd == STDERR_FILENO);
  if (fd_closed[fd]) {
    return;
  }
  line_buffer_t *lb = &line_buffers[fd - 1];
  wasilibc_utf8_writer_t *utf8 = &utf8_writers[fd - 1];
  if (utf8->valid_len > 0) {
    line_buffer_out(lb, utf8);
    __wasilibc_utf8_reset(utf8);
  }
}

void __wasilibc_webp2_flush_and_close_stdio(bool also_musl) {
  if (also_musl) {
    fflush(stdout);
  }
  finish_and_flush(STDOUT_FILENO);
  fd_closed[STDOUT_FILENO] = true;

  if (also_musl) {
    fflush(stderr);
  }
  finish_and_flush(STDERR_FILENO);
  fd_closed[STDERR_FILENO] = true;
}
