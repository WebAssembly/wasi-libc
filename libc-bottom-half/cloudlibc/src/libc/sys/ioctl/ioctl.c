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
      wasi_subscription_t subscriptions[2] = {
          {
              .type = WASI_EVENTTYPE_FD_READ,
              .fd_readwrite.fd = fildes,
              .fd_readwrite.flags = WASI_SUBSCRIPTION_FD_READWRITE_POLL,
          },
          {
              .type = WASI_EVENTTYPE_CLOCK,
              .clock.clock_id = WASI_CLOCK_MONOTONIC,
          },
      };
      wasi_event_t events[__arraycount(subscriptions)];
      size_t nevents;
      wasi_errno_t error = wasi_poll(
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
        wasi_event_t *event = &events[i];
        if (event->error != 0) {
          errno = event->error;
          return -1;
        }
        if (event->type == WASI_EVENTTYPE_FD_READ) {
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
      wasi_fdstat_t fds;
      wasi_errno_t error = wasi_fd_stat_get(fildes, &fds);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Toggle the non-blocking flag based on the argument.
      va_list ap;
      va_start(ap, request);
      if (*va_arg(ap, const int *) != 0)
        fds.fs_flags |= WASI_FDFLAG_NONBLOCK;
      else
        fds.fs_flags &= ~WASI_FDFLAG_NONBLOCK;
      va_end(ap);

      // Update the file descriptor flags.
      error = wasi_fd_stat_put(fildes, &fds, WASI_FDSTAT_FLAGS);
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
