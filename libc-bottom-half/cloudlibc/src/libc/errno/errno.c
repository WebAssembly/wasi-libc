// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <assert.h>
#include <wasi.h>
#include <errno.h>
#include <threads.h>

static_assert(E2BIG == __WASI_E2BIG, "Value mismatch");
static_assert(EACCES == __WASI_EACCES, "Value mismatch");
static_assert(EADDRINUSE == __WASI_EADDRINUSE, "Value mismatch");
static_assert(EADDRNOTAVAIL == __WASI_EADDRNOTAVAIL, "Value mismatch");
static_assert(EAFNOSUPPORT == __WASI_EAFNOSUPPORT, "Value mismatch");
static_assert(EAGAIN == __WASI_EAGAIN, "Value mismatch");
static_assert(EALREADY == __WASI_EALREADY, "Value mismatch");
static_assert(EBADF == __WASI_EBADF, "Value mismatch");
static_assert(EBADMSG == __WASI_EBADMSG, "Value mismatch");
static_assert(EBUSY == __WASI_EBUSY, "Value mismatch");
static_assert(ECANCELED == __WASI_ECANCELED, "Value mismatch");
static_assert(ECHILD == __WASI_ECHILD, "Value mismatch");
static_assert(ECONNABORTED == __WASI_ECONNABORTED, "Value mismatch");
static_assert(ECONNREFUSED == __WASI_ECONNREFUSED, "Value mismatch");
static_assert(ECONNRESET == __WASI_ECONNRESET, "Value mismatch");
static_assert(EDEADLK == __WASI_EDEADLK, "Value mismatch");
static_assert(EDESTADDRREQ == __WASI_EDESTADDRREQ, "Value mismatch");
static_assert(EDOM == __WASI_EDOM, "Value mismatch");
static_assert(EDQUOT == __WASI_EDQUOT, "Value mismatch");
static_assert(EEXIST == __WASI_EEXIST, "Value mismatch");
static_assert(EFAULT == __WASI_EFAULT, "Value mismatch");
static_assert(EFBIG == __WASI_EFBIG, "Value mismatch");
static_assert(EHOSTUNREACH == __WASI_EHOSTUNREACH, "Value mismatch");
static_assert(EIDRM == __WASI_EIDRM, "Value mismatch");
static_assert(EILSEQ == __WASI_EILSEQ, "Value mismatch");
static_assert(EINPROGRESS == __WASI_EINPROGRESS, "Value mismatch");
static_assert(EINTR == __WASI_EINTR, "Value mismatch");
static_assert(EINVAL == __WASI_EINVAL, "Value mismatch");
static_assert(EIO == __WASI_EIO, "Value mismatch");
static_assert(EISCONN == __WASI_EISCONN, "Value mismatch");
static_assert(EISDIR == __WASI_EISDIR, "Value mismatch");
static_assert(ELOOP == __WASI_ELOOP, "Value mismatch");
static_assert(EMFILE == __WASI_EMFILE, "Value mismatch");
static_assert(EMLINK == __WASI_EMLINK, "Value mismatch");
static_assert(EMSGSIZE == __WASI_EMSGSIZE, "Value mismatch");
static_assert(EMULTIHOP == __WASI_EMULTIHOP, "Value mismatch");
static_assert(ENAMETOOLONG == __WASI_ENAMETOOLONG, "Value mismatch");
static_assert(ENETDOWN == __WASI_ENETDOWN, "Value mismatch");
static_assert(ENETRESET == __WASI_ENETRESET, "Value mismatch");
static_assert(ENETUNREACH == __WASI_ENETUNREACH, "Value mismatch");
static_assert(ENFILE == __WASI_ENFILE, "Value mismatch");
static_assert(ENOBUFS == __WASI_ENOBUFS, "Value mismatch");
static_assert(ENODEV == __WASI_ENODEV, "Value mismatch");
static_assert(ENOENT == __WASI_ENOENT, "Value mismatch");
static_assert(ENOEXEC == __WASI_ENOEXEC, "Value mismatch");
static_assert(ENOLCK == __WASI_ENOLCK, "Value mismatch");
static_assert(ENOLINK == __WASI_ENOLINK, "Value mismatch");
static_assert(ENOMEM == __WASI_ENOMEM, "Value mismatch");
static_assert(ENOMSG == __WASI_ENOMSG, "Value mismatch");
static_assert(ENOPROTOOPT == __WASI_ENOPROTOOPT, "Value mismatch");
static_assert(ENOSPC == __WASI_ENOSPC, "Value mismatch");
static_assert(ENOSYS == __WASI_ENOSYS, "Value mismatch");
static_assert(ENOTCAPABLE == __WASI_ENOTCAPABLE, "Value mismatch");
static_assert(ENOTCONN == __WASI_ENOTCONN, "Value mismatch");
static_assert(ENOTDIR == __WASI_ENOTDIR, "Value mismatch");
static_assert(ENOTEMPTY == __WASI_ENOTEMPTY, "Value mismatch");
static_assert(ENOTRECOVERABLE == __WASI_ENOTRECOVERABLE, "Value mismatch");
static_assert(ENOTSOCK == __WASI_ENOTSOCK, "Value mismatch");
static_assert(ENOTSUP == __WASI_ENOTSUP, "Value mismatch");
static_assert(ENOTTY == __WASI_ENOTTY, "Value mismatch");
static_assert(ENXIO == __WASI_ENXIO, "Value mismatch");
static_assert(EOVERFLOW == __WASI_EOVERFLOW, "Value mismatch");
static_assert(EOWNERDEAD == __WASI_EOWNERDEAD, "Value mismatch");
static_assert(EPERM == __WASI_EPERM, "Value mismatch");
static_assert(EPIPE == __WASI_EPIPE, "Value mismatch");
static_assert(EPROTO == __WASI_EPROTO, "Value mismatch");
static_assert(EPROTONOSUPPORT == __WASI_EPROTONOSUPPORT, "Value mismatch");
static_assert(EPROTOTYPE == __WASI_EPROTOTYPE, "Value mismatch");
static_assert(ERANGE == __WASI_ERANGE, "Value mismatch");
static_assert(EROFS == __WASI_EROFS, "Value mismatch");
static_assert(ESPIPE == __WASI_ESPIPE, "Value mismatch");
static_assert(ESRCH == __WASI_ESRCH, "Value mismatch");
static_assert(ESTALE == __WASI_ESTALE, "Value mismatch");
static_assert(ETIMEDOUT == __WASI_ETIMEDOUT, "Value mismatch");
static_assert(ETXTBSY == __WASI_ETXTBSY, "Value mismatch");
static_assert(EXDEV == __WASI_EXDEV, "Value mismatch");

thread_local int errno = 0;
