// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/ioctl.h>

#include <wasi/api.h>
#include <errno.h>
#include <stdarg.h>
#include <memory.h>
#include <termios.h>

int ioctl(int fildes, int request, ...) {
  switch (request) {
    case FIONREAD: {
      // Poll the file descriptor to determine how many bytes can be read.
      __wasi_subscription_t subscriptions[2] = {
          {
              .u.tag = __WASI_EVENTTYPE_FD_READ,
              .u.u.fd_read.file_descriptor = fildes,
          },
      };
      __wasi_event_t events[__arraycount(subscriptions)];
      __wasi_size_t nevents;
      __wasi_errno_t error = __wasi_poll_oneoff(
          subscriptions, events, __arraycount(subscriptions), &nevents);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Location where result should be written.
      va_list ap;
      va_start(ap, request);
      int *result = va_arg(ap, int *);
      va_end(ap);

      // Extract number of bytes for reading from poll results.
      for (size_t i = 0; i < nevents; ++i) {
        __wasi_event_t *event = &events[i];
        if (event->error != 0) {
          errno = event->error;
          return -1;
        }
        if (event->type == __WASI_EVENTTYPE_FD_READ) {
          *result = event->fd_readwrite.nbytes;
          return 0;
        }
      }

      // No data available for reading.
      *result = 0;
      return 0;
    }
    case FIONBIO: {
      // Obtain the current file descriptor flags.
      __wasi_fdstat_t fds;
      __wasi_errno_t error = __wasi_fd_fdstat_get(fildes, &fds);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Toggle the non-blocking flag based on the argument.
      va_list ap;
      va_start(ap, request);
      if (*va_arg(ap, const int *) != 0)
        fds.fs_flags |= __WASI_FDFLAGS_NONBLOCK;
      else
        fds.fs_flags &= ~__WASI_FDFLAGS_NONBLOCK;
      va_end(ap);

      // Update the file descriptor flags.
      error = __wasi_fd_fdstat_set_flags(fildes, fds.fs_flags);
      if (error != 0) {
        errno = error;
        return -1;
      }
      return 0;
    }
    case TIOCGWINSZ: {
      va_list ap;
      va_start(ap, request);
      struct winsize *wsz = va_arg(ap, struct winsize *);
      va_end(ap);

      __wasi_tty_t tty;
      int r = __wasi_tty_get(&tty);
      if (r != 0) {
        errno = r;
        return -1;
      }

      memset(wsz, 0, sizeof(struct winsize));
      wsz->ws_col = tty.cols;
      wsz->ws_row = tty.rows;
      wsz->ws_xpixel = tty.width;
      wsz->ws_ypixel = tty.height;
      return 0;
    }
    case TIOCSWINSZ: {
      va_list ap;
      va_start(ap, request);
      const struct winsize *wsz = va_arg(ap, const struct winsize *);
      va_end(ap);

      __wasi_tty_t tty;
      tty.cols = wsz->ws_col;
      tty.rows = wsz->ws_row;
      tty.width = wsz->ws_xpixel;
      tty.height = wsz->ws_ypixel;
      
      // Set the updated TTY settings
      int r = __wasi_tty_set(&tty);
      if (r != 0) {
        errno = r;
        return -1;
      }
      return 0;
    }
    default:
      // Invalid request.
      errno = EINVAL;
      return -1;
  }
}
