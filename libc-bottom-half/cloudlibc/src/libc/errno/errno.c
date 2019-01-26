// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <wasi.h>
#include <errno.h>
#include <threads.h>

static_assert(E2BIG == WASI_E2BIG, "Value mismatch");
static_assert(EACCES == WASI_EACCES, "Value mismatch");
static_assert(EADDRINUSE == WASI_EADDRINUSE, "Value mismatch");
static_assert(EADDRNOTAVAIL == WASI_EADDRNOTAVAIL, "Value mismatch");
static_assert(EAFNOSUPPORT == WASI_EAFNOSUPPORT, "Value mismatch");
static_assert(EAGAIN == WASI_EAGAIN, "Value mismatch");
static_assert(EALREADY == WASI_EALREADY, "Value mismatch");
static_assert(EBADF == WASI_EBADF, "Value mismatch");
static_assert(EBADMSG == WASI_EBADMSG, "Value mismatch");
static_assert(EBUSY == WASI_EBUSY, "Value mismatch");
static_assert(ECANCELED == WASI_ECANCELED, "Value mismatch");
static_assert(ECHILD == WASI_ECHILD, "Value mismatch");
static_assert(ECONNABORTED == WASI_ECONNABORTED, "Value mismatch");
static_assert(ECONNREFUSED == WASI_ECONNREFUSED, "Value mismatch");
static_assert(ECONNRESET == WASI_ECONNRESET, "Value mismatch");
static_assert(EDEADLK == WASI_EDEADLK, "Value mismatch");
static_assert(EDESTADDRREQ == WASI_EDESTADDRREQ, "Value mismatch");
static_assert(EDOM == WASI_EDOM, "Value mismatch");
static_assert(EDQUOT == WASI_EDQUOT, "Value mismatch");
static_assert(EEXIST == WASI_EEXIST, "Value mismatch");
static_assert(EFAULT == WASI_EFAULT, "Value mismatch");
static_assert(EFBIG == WASI_EFBIG, "Value mismatch");
static_assert(EHOSTUNREACH == WASI_EHOSTUNREACH, "Value mismatch");
static_assert(EIDRM == WASI_EIDRM, "Value mismatch");
static_assert(EILSEQ == WASI_EILSEQ, "Value mismatch");
static_assert(EINPROGRESS == WASI_EINPROGRESS, "Value mismatch");
static_assert(EINTR == WASI_EINTR, "Value mismatch");
static_assert(EINVAL == WASI_EINVAL, "Value mismatch");
static_assert(EIO == WASI_EIO, "Value mismatch");
static_assert(EISCONN == WASI_EISCONN, "Value mismatch");
static_assert(EISDIR == WASI_EISDIR, "Value mismatch");
static_assert(ELOOP == WASI_ELOOP, "Value mismatch");
static_assert(EMFILE == WASI_EMFILE, "Value mismatch");
static_assert(EMLINK == WASI_EMLINK, "Value mismatch");
static_assert(EMSGSIZE == WASI_EMSGSIZE, "Value mismatch");
static_assert(EMULTIHOP == WASI_EMULTIHOP, "Value mismatch");
static_assert(ENAMETOOLONG == WASI_ENAMETOOLONG, "Value mismatch");
static_assert(ENETDOWN == WASI_ENETDOWN, "Value mismatch");
static_assert(ENETRESET == WASI_ENETRESET, "Value mismatch");
static_assert(ENETUNREACH == WASI_ENETUNREACH, "Value mismatch");
static_assert(ENFILE == WASI_ENFILE, "Value mismatch");
static_assert(ENOBUFS == WASI_ENOBUFS, "Value mismatch");
static_assert(ENODEV == WASI_ENODEV, "Value mismatch");
static_assert(ENOENT == WASI_ENOENT, "Value mismatch");
static_assert(ENOEXEC == WASI_ENOEXEC, "Value mismatch");
static_assert(ENOLCK == WASI_ENOLCK, "Value mismatch");
static_assert(ENOLINK == WASI_ENOLINK, "Value mismatch");
static_assert(ENOMEM == WASI_ENOMEM, "Value mismatch");
static_assert(ENOMSG == WASI_ENOMSG, "Value mismatch");
static_assert(ENOPROTOOPT == WASI_ENOPROTOOPT, "Value mismatch");
static_assert(ENOSPC == WASI_ENOSPC, "Value mismatch");
static_assert(ENOSYS == WASI_ENOSYS, "Value mismatch");
static_assert(ENOTCAPABLE == WASI_ENOTCAPABLE, "Value mismatch");
static_assert(ENOTCONN == WASI_ENOTCONN, "Value mismatch");
static_assert(ENOTDIR == WASI_ENOTDIR, "Value mismatch");
static_assert(ENOTEMPTY == WASI_ENOTEMPTY, "Value mismatch");
static_assert(ENOTRECOVERABLE == WASI_ENOTRECOVERABLE, "Value mismatch");
static_assert(ENOTSOCK == WASI_ENOTSOCK, "Value mismatch");
static_assert(ENOTSUP == WASI_ENOTSUP, "Value mismatch");
static_assert(ENOTTY == WASI_ENOTTY, "Value mismatch");
static_assert(ENXIO == WASI_ENXIO, "Value mismatch");
static_assert(EOVERFLOW == WASI_EOVERFLOW, "Value mismatch");
static_assert(EOWNERDEAD == WASI_EOWNERDEAD, "Value mismatch");
static_assert(EPERM == WASI_EPERM, "Value mismatch");
static_assert(EPIPE == WASI_EPIPE, "Value mismatch");
static_assert(EPROTO == WASI_EPROTO, "Value mismatch");
static_assert(EPROTONOSUPPORT == WASI_EPROTONOSUPPORT, "Value mismatch");
static_assert(EPROTOTYPE == WASI_EPROTOTYPE, "Value mismatch");
static_assert(ERANGE == WASI_ERANGE, "Value mismatch");
static_assert(EROFS == WASI_EROFS, "Value mismatch");
static_assert(ESPIPE == WASI_ESPIPE, "Value mismatch");
static_assert(ESRCH == WASI_ESRCH, "Value mismatch");
static_assert(ESTALE == WASI_ESTALE, "Value mismatch");
static_assert(ETIMEDOUT == WASI_ETIMEDOUT, "Value mismatch");
static_assert(ETXTBSY == WASI_ETXTBSY, "Value mismatch");
static_assert(EXDEV == WASI_EXDEV, "Value mismatch");

thread_local int errno = 0;
