// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/time.h>
#include <sys/select.h>
#include <wasi/api.h>
#include <errno.h>
#include <poll.h>

int pselect(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
            fd_set *restrict errorfds, const struct timespec *restrict timeout,
            const sigset_t *sigmask) {
  (void) sigmask;
  // Negative file descriptor upperbound.
  if (nfds < 0) {
    errno = EINVAL;
    return -1;
  }

  // Replace NULL pointers by the empty set.
  fd_set empty;
  FD_ZERO(&empty);
  if (readfds == NULL)
    readfds = &empty;
  if (writefds == NULL)
    writefds = &empty;
  if (errorfds == NULL)
    errorfds = &empty;

  struct pollfd poll_fds[readfds->__nfds + writefds->__nfds + errorfds->__nfds];
  size_t poll_nfds = 0;

  // Combine the event bits when the same file descriptor appears in more than
  // one set.
  struct {
    const fd_set *set;
    short events;
  } sets[3] = {
      {readfds, POLLRDNORM},
      {writefds, POLLWRNORM},
      {errorfds, POLLPRI},
  };
  for (size_t s = 0; s < 3; s++) {
    for (size_t i = 0; i < sets[s].set->__nfds; i++) {
      int fd = sets[s].set->__fds[i];
      if (fd >= nfds)
        continue;
      size_t j = 0;
      while (j < poll_nfds && poll_fds[j].fd != fd)
        ++j;
      if (j == poll_nfds) {
        poll_fds[poll_nfds++] = (struct pollfd){
            .fd = fd,
            .events = 0,
            .revents = 0,
        };
      }
      poll_fds[j].events |= sets[s].events;
    }
  }

  int poll_timeout;
  if (timeout) {
    uint64_t timeout_u64;
#if defined(__wasip1__)
    if (!timespec_to_timestamp_clamp(timeout, &timeout_u64) ) {
#elif defined(__wasip2__) || defined(__wasip3__)
    if (!timespec_to_instant_clamp(timeout, &timeout_u64) ) {
#else
# error "Unknown WASI version"
#endif
      errno = EINVAL;
      return -1;
    }

    // Convert nanoseconds to milliseconds:
    timeout_u64 /= 1000000;

    if (timeout_u64 > INT_MAX) {
      timeout_u64 = INT_MAX;
    }

    poll_timeout = (int) timeout_u64;
  } else {
    poll_timeout = -1;
  };

  if (poll(poll_fds, poll_nfds, poll_timeout) < 0) {
    return -1;
  }

  FD_ZERO(readfds);
  FD_ZERO(writefds);
  FD_ZERO(errorfds);
  for (size_t i = 0; i < poll_nfds; ++i) {
    struct pollfd* pollfd = poll_fds + i;
    if ((pollfd->revents & POLLRDNORM) != 0) {
      readfds->__fds[readfds->__nfds++] = pollfd->fd;
    }
    if ((pollfd->revents & POLLWRNORM) != 0) {
      writefds->__fds[writefds->__nfds++] = pollfd->fd;
    }
    if ((pollfd->revents & POLLPRI) != 0) {
      errorfds->__fds[errorfds->__nfds++] = pollfd->fd;
    }
  }

  return readfds->__nfds + writefds->__nfds + errorfds->__nfds;
}
