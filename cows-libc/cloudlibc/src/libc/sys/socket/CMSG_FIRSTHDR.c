// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <sys/socket.h>

#include "socket_impl.h"

struct cmsghdr *CMSG_FIRSTHDR(const struct msghdr *mhdr) {
  return CMSG_GET(mhdr, mhdr->msg_control);
}
