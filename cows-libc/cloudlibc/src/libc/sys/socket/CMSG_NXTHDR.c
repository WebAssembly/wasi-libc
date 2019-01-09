// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#include "socket_impl.h"

struct cmsghdr *CMSG_NXTHDR(const struct msghdr *mhdr,
                            const struct cmsghdr *cmsg) {
  // RFC 2292: if the value of the cmsg pointer is NULL, a pointer to
  // the cmsghdr structure describing the first ancillary data object
  // is returned.
  if (cmsg == NULL)
    return CMSG_GET(mhdr, mhdr->msg_control);

  // Safety belt: CMSG_NXTHDR() should not be called on a malformed
  // control message header. This could cause an infinite loop if
  // cmsg_len were zero.
  assert(cmsg->cmsg_len >= offsetof(struct cmsghdr, __cmsg_data) &&
         "Message header has negative data length");

  // Obtain the address of the next entry by adding this entry's length,
  // rounding it up until the alignment is correct.
  return CMSG_GET(mhdr, (void *)__roundup((uintptr_t)cmsg + cmsg->cmsg_len,
                                          alignof(struct cmsghdr)));
}
