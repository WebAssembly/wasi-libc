// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <poll.h>
#include <stddef.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
  if (timeout < 0)
    return ppoll(fds, nfds, NULL, NULL);

  struct timespec ts;
  ts.tv_sec = timeout / 1000;
  ts.tv_nsec = (timeout % 1000) * 1000000;
  return ppoll(fds, nfds, &ts, NULL);
}
