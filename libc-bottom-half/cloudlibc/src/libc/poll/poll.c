// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <wasi.h>
#include <errno.h>
#include <poll.h>
#include <stdbool.h>

int poll(struct pollfd *fds, size_t nfds, int timeout) {
  // Construct events for poll().
  size_t maxevents = 2 * nfds + 1;
  wasi_subscription_t subscriptions[maxevents];
  size_t nevents = 0;
  for (size_t i = 0; i < nfds; ++i) {
    struct pollfd *pollfd = &fds[i];
    if (pollfd->fd < 0)
      continue;
    bool created_events = false;
    if ((pollfd->events & POLLRDNORM) != 0) {
      wasi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .type = WASI_EVENTTYPE_FD_READ,
          .fd_readwrite.fd = pollfd->fd,
          .fd_readwrite.flags = WASI_SUBSCRIPTION_FD_READWRITE_POLL,
      };
      created_events = true;
    }
    if ((pollfd->events & POLLWRNORM) != 0) {
      wasi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (wasi_subscription_t){
          .userdata = (uintptr_t)pollfd,
          .type = WASI_EVENTTYPE_FD_WRITE,
          .fd_readwrite.fd = pollfd->fd,
          .fd_readwrite.flags = WASI_SUBSCRIPTION_FD_READWRITE_POLL,
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
    wasi_subscription_t *subscription = &subscriptions[nevents++];
    *subscription = (wasi_subscription_t){
        .type = WASI_EVENTTYPE_CLOCK,
        .clock.clock_id = WASI_CLOCK_REALTIME,
        .clock.timeout = (wasi_timestamp_t)timeout * 1000000,
    };
  }

  // Execute poll().
  wasi_event_t events[nevents];
  wasi_errno_t error =
      wasi_poll(subscriptions, events, nevents, &nevents);
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
    const wasi_event_t *event = &events[i];
    if (event->type == WASI_EVENTTYPE_FD_READ ||
        event->type == WASI_EVENTTYPE_FD_WRITE) {
      struct pollfd *pollfd = (struct pollfd *)(uintptr_t)event->userdata;
      if (event->error == WASI_EBADF) {
        // Invalid file descriptor.
        pollfd->revents |= POLLNVAL;
      } else if (event->error == WASI_EPIPE) {
        // Hangup on write side of pipe.
        pollfd->revents |= POLLHUP;
      } else if (event->error != 0) {
        // Another error occurred.
        pollfd->revents |= POLLERR;
      } else {
        // Data can be read or written.
        pollfd->revents |=
            event->type == WASI_EVENTTYPE_FD_READ ? POLLRDNORM : POLLWRNORM;
        if (event->fd_readwrite.flags & WASI_EVENT_FD_READWRITE_HANGUP)
          pollfd->revents |= POLLHUP;
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
