// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <cloudabi_types.h>
#include <errno.h>
#include <threads.h>

static_assert(E2BIG == CLOUDABI_E2BIG, "Value mismatch");
static_assert(EACCES == CLOUDABI_EACCES, "Value mismatch");
static_assert(EADDRINUSE == CLOUDABI_EADDRINUSE, "Value mismatch");
static_assert(EADDRNOTAVAIL == CLOUDABI_EADDRNOTAVAIL, "Value mismatch");
static_assert(EAFNOSUPPORT == CLOUDABI_EAFNOSUPPORT, "Value mismatch");
static_assert(EAGAIN == CLOUDABI_EAGAIN, "Value mismatch");
static_assert(EALREADY == CLOUDABI_EALREADY, "Value mismatch");
static_assert(EBADF == CLOUDABI_EBADF, "Value mismatch");
static_assert(EBADMSG == CLOUDABI_EBADMSG, "Value mismatch");
static_assert(EBUSY == CLOUDABI_EBUSY, "Value mismatch");
static_assert(ECANCELED == CLOUDABI_ECANCELED, "Value mismatch");
static_assert(ECHILD == CLOUDABI_ECHILD, "Value mismatch");
static_assert(ECONNABORTED == CLOUDABI_ECONNABORTED, "Value mismatch");
static_assert(ECONNREFUSED == CLOUDABI_ECONNREFUSED, "Value mismatch");
static_assert(ECONNRESET == CLOUDABI_ECONNRESET, "Value mismatch");
static_assert(EDEADLK == CLOUDABI_EDEADLK, "Value mismatch");
static_assert(EDESTADDRREQ == CLOUDABI_EDESTADDRREQ, "Value mismatch");
static_assert(EDOM == CLOUDABI_EDOM, "Value mismatch");
static_assert(EDQUOT == CLOUDABI_EDQUOT, "Value mismatch");
static_assert(EEXIST == CLOUDABI_EEXIST, "Value mismatch");
static_assert(EFAULT == CLOUDABI_EFAULT, "Value mismatch");
static_assert(EFBIG == CLOUDABI_EFBIG, "Value mismatch");
static_assert(EHOSTUNREACH == CLOUDABI_EHOSTUNREACH, "Value mismatch");
static_assert(EIDRM == CLOUDABI_EIDRM, "Value mismatch");
static_assert(EILSEQ == CLOUDABI_EILSEQ, "Value mismatch");
static_assert(EINPROGRESS == CLOUDABI_EINPROGRESS, "Value mismatch");
static_assert(EINTR == CLOUDABI_EINTR, "Value mismatch");
static_assert(EINVAL == CLOUDABI_EINVAL, "Value mismatch");
static_assert(EIO == CLOUDABI_EIO, "Value mismatch");
static_assert(EISCONN == CLOUDABI_EISCONN, "Value mismatch");
static_assert(EISDIR == CLOUDABI_EISDIR, "Value mismatch");
static_assert(ELOOP == CLOUDABI_ELOOP, "Value mismatch");
static_assert(EMFILE == CLOUDABI_EMFILE, "Value mismatch");
static_assert(EMLINK == CLOUDABI_EMLINK, "Value mismatch");
static_assert(EMSGSIZE == CLOUDABI_EMSGSIZE, "Value mismatch");
static_assert(EMULTIHOP == CLOUDABI_EMULTIHOP, "Value mismatch");
static_assert(ENAMETOOLONG == CLOUDABI_ENAMETOOLONG, "Value mismatch");
static_assert(ENETDOWN == CLOUDABI_ENETDOWN, "Value mismatch");
static_assert(ENETRESET == CLOUDABI_ENETRESET, "Value mismatch");
static_assert(ENETUNREACH == CLOUDABI_ENETUNREACH, "Value mismatch");
static_assert(ENFILE == CLOUDABI_ENFILE, "Value mismatch");
static_assert(ENOBUFS == CLOUDABI_ENOBUFS, "Value mismatch");
static_assert(ENODEV == CLOUDABI_ENODEV, "Value mismatch");
static_assert(ENOENT == CLOUDABI_ENOENT, "Value mismatch");
static_assert(ENOEXEC == CLOUDABI_ENOEXEC, "Value mismatch");
static_assert(ENOLCK == CLOUDABI_ENOLCK, "Value mismatch");
static_assert(ENOLINK == CLOUDABI_ENOLINK, "Value mismatch");
static_assert(ENOMEM == CLOUDABI_ENOMEM, "Value mismatch");
static_assert(ENOMSG == CLOUDABI_ENOMSG, "Value mismatch");
static_assert(ENOPROTOOPT == CLOUDABI_ENOPROTOOPT, "Value mismatch");
static_assert(ENOSPC == CLOUDABI_ENOSPC, "Value mismatch");
static_assert(ENOSYS == CLOUDABI_ENOSYS, "Value mismatch");
static_assert(ENOTCAPABLE == CLOUDABI_ENOTCAPABLE, "Value mismatch");
static_assert(ENOTCONN == CLOUDABI_ENOTCONN, "Value mismatch");
static_assert(ENOTDIR == CLOUDABI_ENOTDIR, "Value mismatch");
static_assert(ENOTEMPTY == CLOUDABI_ENOTEMPTY, "Value mismatch");
static_assert(ENOTRECOVERABLE == CLOUDABI_ENOTRECOVERABLE, "Value mismatch");
static_assert(ENOTSOCK == CLOUDABI_ENOTSOCK, "Value mismatch");
static_assert(ENOTSUP == CLOUDABI_ENOTSUP, "Value mismatch");
static_assert(ENOTTY == CLOUDABI_ENOTTY, "Value mismatch");
static_assert(ENXIO == CLOUDABI_ENXIO, "Value mismatch");
static_assert(EOVERFLOW == CLOUDABI_EOVERFLOW, "Value mismatch");
static_assert(EOWNERDEAD == CLOUDABI_EOWNERDEAD, "Value mismatch");
static_assert(EPERM == CLOUDABI_EPERM, "Value mismatch");
static_assert(EPIPE == CLOUDABI_EPIPE, "Value mismatch");
static_assert(EPROTO == CLOUDABI_EPROTO, "Value mismatch");
static_assert(EPROTONOSUPPORT == CLOUDABI_EPROTONOSUPPORT, "Value mismatch");
static_assert(EPROTOTYPE == CLOUDABI_EPROTOTYPE, "Value mismatch");
static_assert(ERANGE == CLOUDABI_ERANGE, "Value mismatch");
static_assert(EROFS == CLOUDABI_EROFS, "Value mismatch");
static_assert(ESPIPE == CLOUDABI_ESPIPE, "Value mismatch");
static_assert(ESRCH == CLOUDABI_ESRCH, "Value mismatch");
static_assert(ESTALE == CLOUDABI_ESTALE, "Value mismatch");
static_assert(ETIMEDOUT == CLOUDABI_ETIMEDOUT, "Value mismatch");
static_assert(ETXTBSY == CLOUDABI_ETXTBSY, "Value mismatch");
static_assert(EXDEV == CLOUDABI_EXDEV, "Value mismatch");

thread_local int errno = 0;
