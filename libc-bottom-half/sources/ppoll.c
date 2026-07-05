// Copyright 2026 The ChromiumOS Authors

#include <poll.h>
#include <stdlib.h>

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout,
          const sigset_t *sigmask) {
  (void)sigmask;
  int ptimeout = timeout == NULL
                     ? -1
                     : (timeout->tv_sec * 1000 + timeout->tv_nsec / 1000000);
  return poll(fds, nfds, ptimeout);
}
