// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi/api.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>

int poll(struct pollfd *fds, size_t nfds, int timeout) {
  // Construct events for poll().
  size_t maxevents = 2 * nfds + 1;
  __wasi_subscription_t subscriptions[maxevents];
  size_t nevents = 0;
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    if (pollfd->fd < 0)
      continue;
    bool created_events = false;
    if ((pollfd->events & POLLRDNORM) != 0) {
      __wasi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (__wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .u.tag = __WASI_EVENTTYPE_FD_READ,
          .u.u.fd_read.file_descriptor = pollfd->fd,
      };
      created_events = true;
    }
    if ((pollfd->events & POLLWRNORM) != 0) {
      __wasi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (__wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .u.tag = __WASI_EVENTTYPE_FD_WRITE,
          .u.u.fd_write.file_descriptor = pollfd->fd,
      };
      created_events = true;
    }

    // As entries are decomposed into separate read/write subscriptions,
    // we cannot detect POLLERR, POLLHUP and POLLNVAL if POLLRDNORM and
    // POLLWRNORM are not specified. Disallow this for now.
    if (!created_events) {
      errno = ENOSYS;
      return -1;
    }
  }

  // Create extra event for the timeout.
  if (timeout >= 0) {
    __wasi_subscription_t *subscription = &subscriptions[nevents++];
    *subscription = (__wasi_subscription_t){
        .u.tag = __WASI_EVENTTYPE_CLOCK,
        .u.u.clock.id = __WASI_CLOCKID_REALTIME,
        .u.u.clock.timeout = (__wasi_timestamp_t)timeout * 1000000,
    };
  }

  // Execute poll().
  __wasi_event_t events[nevents];
  __wasi_errno_t error =
#ifdef __wasilibc_unmodified_upstream
      __wasi_poll(subscriptions, events, nevents, &nevents);
#else
      __wasi_poll_oneoff(subscriptions, events, nevents, &nevents);
#endif
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Clear revents fields.
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    pollfd->revents = 0;
  }

  // Set revents fields.
  for (size_t i = 0; i < nevents; ++i) {
    const __wasi_event_t *event = &events[i];
    if (event->u.tag == __WASI_EVENTTYPE_FD_READ ||
        event->u.tag == __WASI_EVENTTYPE_FD_WRITE) {
      struct pollfd *pollfd = (struct pollfd *)(uintptr_t)event->userdata;
#ifdef __wasilibc_unmodified_upstream // generated constant names
      if (event->error == __WASI_EBADF) {
#else
      if (event->error == __WASI_ERRNO_BADF) {
#endif
        // Invalid file descriptor.
        pollfd->revents |= POLLNVAL;
#ifdef __wasilibc_unmodified_upstream // generated constant names
      } else if (event->error == __WASI_EPIPE) {
#else
      } else if (event->error == __WASI_ERRNO_PIPE) {
#endif
        // Hangup on write side of pipe.
        pollfd->revents |= POLLHUP;
      } else if (event->error != 0) {
        // Another error occurred.
        pollfd->revents |= POLLERR;
      } else {
        // Data can be read or written.
        if (event->u.tag == __WASI_EVENTTYPE_FD_READ) {
            pollfd->revents |= POLLRDNORM;
            if (event->u.u.fd_read.flags & __WASI_EVENTRWFLAGS_FD_READWRITE_HANGUP) {
              pollfd->revents |= POLLHUP;
            }
        } else if (event->u.tag == __WASI_EVENTTYPE_FD_WRITE) {
            pollfd->revents |= POLLWRNORM;
            if (event->u.u.fd_write.flags & __WASI_EVENTRWFLAGS_FD_READWRITE_HANGUP) {
              pollfd->revents |= POLLHUP;
            }
        }
      }
    }
  }

  // Return the number of events with a non-zero revents value.
  int retval = 0;
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    // POLLHUP contradicts with POLLWRNORM.
    if ((pollfd->revents & POLLHUP) != 0)
      pollfd->revents &= ~POLLWRNORM;
    if (pollfd->revents != 0)
      ++retval;
  }
  return retval;
}
