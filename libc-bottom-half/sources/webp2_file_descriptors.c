// File descriptors are mostly stubbed out on the web, since there is no
// comparable model of files and streams. However, stdout and stderr are
// supported by routing to console.log, and stdin is supported by immediately
// returning EOF.

#include "features.h"
#include "sys/uio.h"
#include "wasi/report-error.h"
#include <__errno_values.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/__generated_webp2.h>
#include <wasi/utf8.h>

// unistd.h defines an `lseek` macro that would mangle the definition below.
#undef lseek

// ============================================================================
// Basic stdio implementation

#define LINE_BUF_SIZE 4096

typedef struct line_buffer_t {
  bool warn;
  uint8_t buf[LINE_BUF_SIZE];
} line_buffer_t;

static line_buffer_t line_buffers[2] = {
  (line_buffer_t){},
  (line_buffer_t){ .warn = true },
};
static wasilibc_utf8_writer_t utf8_writers[2] = {
  __wasilibc_make_utf8_writer(line_buffers[0].buf, LINE_BUF_SIZE),
  __wasilibc_make_utf8_writer(line_buffers[1].buf, LINE_BUF_SIZE),
};

static bool fd_closed[3];

static void line_buffer_out(line_buffer_t *lb, wasilibc_utf8_writer_t *utf8) {
  webp2_string_t out = (webp2_string_t){.ptr = utf8->buf, .len = utf8->valid_len};
  if (lb->warn) {
    webp2_console_warn(&out);
  } else {
    webp2_console_log(&out);
  }
}

static bool fd_ok(int fd) {
  return STDIN_FILENO <= fd && fd <= STDERR_FILENO && !fd_closed[fd];
}

static bool is_readable(int fd) { return fd == STDIN_FILENO; }

static bool is_writable(int fd) {
  return fd == STDOUT_FILENO || fd == STDERR_FILENO;
}

static bool iovecs_ok(const struct iovec *iov, int iovcnt, size_t *total_size) {
  // Check if the total size of all iovecs would overflow an ssize_t
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

int __isatty(int fd) {
  if (!fd_ok(fd)) {
    errno = EBADF;
    return 0;
  }
  errno = ENOTTY;
  return 0;
}
weak_alias(__isatty, isatty);

off_t __lseek(int fildes, off_t offset, int whence) {
  if (!fd_ok(fildes)) {
    errno = EBADF;
    return -1;
  }
  errno = ESPIPE;
  return -1;
}
weak_alias(__lseek, lseek);

off_t __wasilibc_tell(int fd) {
  if (!fd_ok(fd)) {
    errno = EBADF;
    return -1;
  }
  errno = ESPIPE;
  return -1;
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

int close(int fd) {
  if (!fd_ok(fd)) {
    errno = EBADF;
    return -1;
  }
  if (is_writable(fd)) {
    // Don't flush musl's buffers, but do flush everything we have.
    finish_and_flush(fd);
  }
  fd_closed[fd] = true;
  return 0;
}

int fcntl(int fildes, int cmd, ...) {
  if (!fd_ok(fildes)) {
    errno = EBADF;
    return -1;
  }
  switch (cmd) {
  case F_GETFD:
  case F_SETFD:
    // Don't bother setting FD_CLOEXEC at all. It's totally irrelevant but
    // there's no reason to crash if trying to set it.
    return 0;
  case F_GETFL:
    return is_readable(fildes) ? O_RDONLY : O_WRONLY;
  default:
    errno = EINVAL;
    return -1;
  }
}

ssize_t read(int fildes, void *buf, size_t nbyte) {
  if (nbyte > SSIZE_MAX) {
    errno = EINVAL;
    return -1;
  }
  if (!is_readable(fildes) || fd_closed[fildes]) {
    errno = EBADF;
    return -1;
  }
  return 0; // immediate EOF;
}

ssize_t readv(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0 || IOV_MAX < iovcnt) {
    errno = EINVAL;
    return -1;
  }
  if (!iovecs_ok(iov, iovcnt, NULL)) {
    errno = EINVAL;
    return -1;
  }
  if (!is_readable(fildes) || fd_closed[fildes]) {
    errno = EBADF;
    return -1;
  }

  size_t bytes_read = 0;
  for (int i = 0; i < iovcnt; i++) {
    const struct iovec *v = &iov[i];
    ssize_t res = read(fildes, v->iov_base, v->iov_len);
    if (res < 0) {
      // errno already set
      return res;
    }
    bytes_read += res;
    if ((size_t)res < v->iov_len) {
      break;
    }
  }
  return bytes_read;
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
  if (nbyte > SSIZE_MAX) {
    errno = EINVAL;
    return -1;
  }
  if (!is_writable(fildes) || fd_closed[fildes]) {
    errno = EBADF;
    return -1;
  }

  line_buffer_t *line_buffer = &line_buffers[fildes - 1];
  wasilibc_utf8_writer_t *utf8 = &utf8_writers[fildes - 1];
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

  return nbyte;
}

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt < 0 || IOV_MAX < iovcnt) {
    errno = EINVAL;
    return -1;
  }
  if (!is_writable(fildes) || fd_closed[fildes]) {
    errno = EBADF;
    return -1;
  }

  size_t total_size;
  if (!iovecs_ok(iov, iovcnt, &total_size)) {
    errno = EINVAL;
    return -1;
  }
  for (int i = 0; i < iovcnt; i++) {
    const struct iovec *v = &iov[i];
    int res = write(fildes, v->iov_base, v->iov_len);
    if (res < 0) {
      // errno already set
      return res;
    }
  }
  return total_size;
}

int fstat(int fildes, struct stat *buf) {
  if (!fd_ok(fildes)) {
    errno = EBADF;
    return -1;
  }
  memset(buf, 0, sizeof(*buf));
  buf->st_mode = S_IFCHR;
  return 0;
}

// ============================================================================
// Stubs

int __dup3(int fd, int newfd, int flags) { WEBP2_UNSUPPORTED("__dup3"); }

int dup(int fd) { WEBP2_UNSUPPORTED("dup"); }

int dup2(int fd, int newfd) { WEBP2_UNSUPPORTED("dup2"); }

int dup3(int fd, int newfd, int flags) { WEBP2_UNSUPPORTED("dup3"); }

int ioctl(int fildes, int request, ...) { WEBP2_UNSUPPORTED("ioctl"); }

int pipe(int fd[2]) { WEBP2_UNSUPPORTED("pipe"); }

int pipe2(int fd[2], int flags) { WEBP2_UNSUPPORTED("pipe2"); }

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
  WEBP2_UNSUPPORTED("poll");
}

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout,
          const sigset_t *sigmask) {
  WEBP2_UNSUPPORTED("ppoll");
}

int pselect(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
            fd_set *restrict errorfds, const struct timespec *restrict timeout,
            const sigset_t *restrict sigmask) {
  WEBP2_UNSUPPORTED("pselect");
}

int select(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
           fd_set *restrict errorfds, struct timeval *restrict timeout) {
  WEBP2_UNSUPPORTED("select");
}
