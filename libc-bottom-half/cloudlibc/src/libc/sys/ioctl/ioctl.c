// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/ioctl.h>

#include <wasi/api.h>
#include <errno.h>
#include <stdarg.h>

int ioctl(int fildes, int request, ...) {
  switch (request) {
    case FIONREAD: {
      // Poll the file descriptor to determine how many bytes can be read.
      __wasi_subscription_t subscriptions[2] = {
          {
              .u.tag = __WASI_EVENTTYPE_FD_READ,
              .u.u.fd_read.file_descriptor = fildes,
          },
          {
              .u.tag = __WASI_EVENTTYPE_CLOCK,
              .u.u.clock.id = __WASI_CLOCKID_MONOTONIC,
          },
      };
      __wasi_event_t events[__arraycount(subscriptions)];
      size_t nevents;
#ifdef __wasilibc_unmodified_upstream
      __wasi_errno_t error = __wasi_poll(
#else
      __wasi_errno_t error = __wasi_poll_oneoff(
#endif
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
        if (event->u.tag == __WASI_EVENTTYPE_FD_READ) {
          *result = event->u.u.fd_read.nbytes;
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
#ifdef __wasilibc_unmodified_upstream
      __wasi_errno_t error = __wasi_fd_stat_get(fildes, &fds);
#else
      __wasi_errno_t error = __wasi_fd_fdstat_get(fildes, &fds);
#endif
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Toggle the non-blocking flag based on the argument.
      va_list ap;
      va_start(ap, request);
      if (*va_arg(ap, const int *) != 0)
#ifdef __wasilibc_unmodified_upstream // generated constant names
        fds.fs_flags |= __WASI_FDFLAG_NONBLOCK;
#else
        fds.fs_flags |= __WASI_FDFLAGS_NONBLOCK;
#endif
      else
#ifdef __wasilibc_unmodified_upstream // generated constant names
        fds.fs_flags &= ~__WASI_FDFLAG_NONBLOCK;
#else
        fds.fs_flags &= ~__WASI_FDFLAGS_NONBLOCK;
#endif
      va_end(ap);

      // Update the file descriptor flags.
#ifdef __wasilibc_unmodified_upstream // fstat
      error = __wasi_fd_stat_put(fildes, &fds, __WASI_FDSTAT_FLAGS);
#else
      error = __wasi_fd_fdstat_set_flags(fildes, fds.fs_flags);
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
