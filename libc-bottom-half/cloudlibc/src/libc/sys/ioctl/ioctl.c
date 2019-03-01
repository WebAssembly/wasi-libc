// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/ioctl.h>

#include <wasi.h>
#include <errno.h>
#include <stdarg.h>

int ioctl(int fildes, int request, ...) {
  switch (request) {
    case FIONREAD: {
      // Poll the file descriptor to determine how many bytes can be read.
      __wasi_subscription_t subscriptions[2] = {
          {
              .type = __WASI_EVENTTYPE_FD_READ,
#ifdef __wasilibc_unmodified_upstream // non-anonymous unions
              .fd_readwrite.fd = fildes,
              .fd_readwrite.flags = __WASI_SUBSCRIPTION_FD_READWRITE_POLL,
#else
              .u.fd_readwrite.fd = fildes,
              .u.fd_readwrite.flags = __WASI_SUBSCRIPTION_FD_READWRITE_POLL,
#endif
          },
          {
              .type = __WASI_EVENTTYPE_CLOCK,
#ifdef __wasilibc_unmodified_upstream // non-anonymous unions
              .clock.clock_id = __WASI_CLOCK_MONOTONIC,
#else
              .u.clock.clock_id = __WASI_CLOCK_MONOTONIC,
#endif
          },
      };
      __wasi_event_t events[__arraycount(subscriptions)];
      size_t nevents;
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
#ifdef __wasilibc_unmodified_upstream // non-anonymous unions
          *result = event->fd_readwrite.nbytes;
#else
          *result = event->u.fd_readwrite.nbytes;
#endif
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
      __wasi_errno_t error = __wasi_fd_stat_get(fildes, &fds);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Toggle the non-blocking flag based on the argument.
      va_list ap;
      va_start(ap, request);
      if (*va_arg(ap, const int *) != 0)
        fds.fs_flags |= __WASI_FDFLAG_NONBLOCK;
      else
        fds.fs_flags &= ~__WASI_FDFLAG_NONBLOCK;
      va_end(ap);

      // Update the file descriptor flags.
#ifdef __wasilibc_unmodified_upstream // fstat
      error = __wasi_fd_stat_put(fildes, &fds, __WASI_FDSTAT_FLAGS);
#else
      error = __wasi_fd_stat_set_flags(fildes, fds.fs_flags);
#endif
      if (error != 0) {
        errno = error;
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
