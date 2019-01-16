// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

ssize_t sendmsg(int socket, const struct msghdr *message, int flags) {
  // This implementation does not support any flags.
  if (flags != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  // Number of I/O vectors must be positive.
  if (message->msg_iovlen <= 0) {
    errno = EMSGSIZE;
    return -1;
  }

  // Prepare input parameters.
  cloudabi_send_in_t si = {
      .si_data = (const cloudabi_ciovec_t *)message->msg_iov,
      .si_data_len = message->msg_iovlen,
  };

  // Process ancillary data.
  for (struct cmsghdr *cmsg = CMSG_FIRSTHDR(message); cmsg != NULL;
       cmsg = CMSG_NXTHDR(message, cmsg)) {
    if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
      // File descriptor passing. Only allow up to one of these.
      if (si.si_fds != NULL) {
        errno = ENOSYS;
        return -1;
      }
      // Extract file descriptor list.
      si.si_fds = (const cloudabi_fd_t *)CMSG_DATA(cmsg);
      si.si_fds_len = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(cloudabi_fd_t);
    } else {
      // Invalid control message type.
      errno = EINVAL;
      return -1;
    }
  }

  // Perform system call.
  cloudabi_send_out_t so;
  cloudabi_errno_t error = cloudabi_sys_sock_send(socket, &si, &so);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  return so.so_datalen;
}
