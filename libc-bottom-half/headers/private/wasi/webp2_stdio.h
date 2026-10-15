#ifndef WASI_WEBP2_STDIO_H
#define WASI_WEBP2_STDIO_H

#include <wasi/version.h>

#ifdef __webp2__

#include <features.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/uio.h>

// File descriptors are mostly stubbed out on the web, since there is no
// comparable model of files and streams. However, stdout and stderr are
// supported by routing to console.log, and stdin is supported by immediately
// returning EOF.

hidden bool __wasilibc_webp2_fd_ok(int fd);
hidden bool __wasilibc_webp2_is_readable(int fd);
hidden bool __wasilibc_webp2_is_writable(int fd);

// Check if the total size of all iovecs would overflow an ssize_t.
// `total_size` can be NULL.
hidden bool __wasilibc_webp2_iovecs_ok(const struct iovec *iov, int iovcnt,
                                       size_t *total_size);

// Implementation for the `write` function, writing valid UTF-8 to the stdio
// line buffers. Aborts if `fd` is not writable.
hidden void __wasilibc_webp2_write(int fd, const void *buf, size_t nbyte);

// Implementation for the `close` function, i.e. finish and flush any line
// buffers and close the fd. Aborts if `fd` is not open.
hidden void __wasilibc_webp2_close(int fd);

#endif // __webp2__

#endif // WASI_WEBP2_STDIO_H
