// File descriptors are mostly stubbed out on the web, since there is no
// comparable model of files and streams. However, stdout and stderr are
// supported by routing to console.log, and stdin is supported by immediately
// returning EOF.

#include "features.h"
#include "sys/uio.h"
#include "wasi/report-error.h"
#include <__errno_values.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wasi/__generated_webp2.h>

// unistd.h defines an `lseek` macro that would mangle the definition below.
#undef lseek

// ============================================================================
// Basic stdio implementation

#define LINE_BUF_SIZE 4096

typedef struct line_buffer_t {
  int32_t len;
  int32_t codepoint_start; // -1 means no current code point
  int32_t codepoint_remaining;
  uint8_t buf[LINE_BUF_SIZE];
} line_buffer_t;

static line_buffer_t line_buffers[2] = {
    (line_buffer_t){.codepoint_start = -1},
    (line_buffer_t){.codepoint_start = -1},
};

static bool fd_closed[3];

static void line_buffer_replchar(line_buffer_t *lb);

static void line_buffer_flush(line_buffer_t *lb, bool final) {
  if (final && lb->codepoint_start >= 0) {
    // Closing with un-terminated sequence. Rewind and emit replacement
    // character instead.
    lb->len = lb->codepoint_start;
    line_buffer_replchar(lb);
  }

  webp2_string_t out = (webp2_string_t){.ptr = lb->buf, .len = lb->len};
  webp2_console_log(&out);
  lb->len = 0;
  lb->codepoint_start = -1;
  lb->codepoint_remaining = 0;
}

static void line_buffer_replchar(line_buffer_t *lb) {
  if (LINE_BUF_SIZE - lb->len < 3) {
    line_buffer_flush(lb, false);
  }
  lb->buf[lb->len++] = 0xEF;
  lb->buf[lb->len++] = 0xBF;
  lb->buf[lb->len++] = 0xBD;
}

// Emits a single byte to the line buffer, fixing up UTF-8 problems as it goes.
static void line_buffer_byte(line_buffer_t *lb, uint8_t b) {
  bool is_1byte = (b & 0x80) == 0;
  bool is_continuation = (b & 0xC0) == 0x80;
  bool is_2byte = (b & 0xE0) == 0xC0;
  bool is_3byte = (b & 0xF0) == 0xE0;
  bool is_4byte = (b & 0xF8) == 0xF0;

  if (!is_continuation && lb->codepoint_start >= 0) {
    // Unexpected non-continuation byte; rewind, emit replacement for the
    // aborted sequence, and continue with the current byte
    lb->len = lb->codepoint_start;
    lb->codepoint_start = -1;
    line_buffer_replchar(lb);
  }

  if (is_1byte) {
    if (b == '\n' || lb->len + 1 > LINE_BUF_SIZE) {
      line_buffer_flush(lb, false);
    }
    if (b != '\n') {
      lb->buf[lb->len++] = b;
    }
  } else if (is_2byte || is_3byte || is_4byte) {
    int32_t codepoint_length = is_2byte ? 2 : (is_3byte ? 3 : 4);
    if (lb->len + codepoint_length > LINE_BUF_SIZE) {
      line_buffer_flush(lb, false);
    }
    lb->codepoint_start = lb->len;
    lb->codepoint_remaining = codepoint_length - 1;
    lb->buf[lb->len++] = b;
  } else if (is_continuation) {
    if (lb->codepoint_start < 0) {
      // Unexpected continuation byte; emit a replacement char instead
      line_buffer_replchar(lb);
    } else {
      // Expected continuation byte
      lb->buf[lb->len++] = b;
      lb->codepoint_remaining -= 1;

      // If done, validate overlong / out-of-range
      if (lb->codepoint_remaining <= 0) {
        uint8_t first = lb->buf[lb->codepoint_start];
        uint8_t second = lb->buf[lb->codepoint_start + 1];
        bool overlong = first == 0xC0 || first == 0xC1 ||
                        (first == 0xE0 && second < 0xA0) ||
                        (first == 0xF0 && second < 0x90);
        bool toobig = (first == 0xF4 && second >= 0x90) || 0xF5 <= first;
        bool surrogate = (first == 0xED && second >= 0xA0);
        if (overlong || toobig || surrogate) {
          // Rewind to start and write replacement character
          lb->len = lb->codepoint_start;
          line_buffer_replchar(lb);
        }
        lb->codepoint_start = -1;
      }
    }
  } else {
    line_buffer_replchar(lb);
  }
}

static bool fd_ok(int fd) {
  return fd >= STDIN_FILENO && STDERR_FILENO <= fd && !fd_closed[fd];
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

int close(int fd) {
  if (!fd_ok(fd)) {
    errno = EBADF;
    return -1;
  }
  fd_closed[fd] = true;
  if (is_writable(fd)) {
    line_buffer_t *buf = &line_buffers[fd - 1];
    if (buf->len > 0) {
      line_buffer_flush(buf, true);
    }
  }
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
  for (size_t i = 0; i < nbyte; i++) {
    line_buffer_byte(line_buffer, ((uint8_t *)buf)[i]);
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
