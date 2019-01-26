/*
 * This file declares functions, types, and values used in WASI.
 *
 * Significant parts of the design and the contents of this file were derived
 * from CloudABI and CloudABI's cloudabi_types.h, cloudabi_types_common.h,
 * and cloudabi_syscalls.h, which bear the following notices:
 *
 * Copyright (c) 2016-2017 Nuxi (https://nuxi.nl/) and contributors.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Source: https://github.com/NuxiNL/cloudabi
 */

#ifndef __wasi_h
#define __wasi_h

#if 0 /* FIXME: Enable this once clang 8.0 has this define in it. */
#ifndef __wasi__
#error __wasi__ is not defined!
#endif
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
#define _Alignof alignof
#define _Atomic(x) x
#define _Static_assert static_assert
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t wasi_advice_t;
#define WASI_ADVICE_DONTNEED   (UINT8_C(1))
#define WASI_ADVICE_NOREUSE    (UINT8_C(2))
#define WASI_ADVICE_NORMAL     (UINT8_C(3))
#define WASI_ADVICE_RANDOM     (UINT8_C(4))
#define WASI_ADVICE_SEQUENTIAL (UINT8_C(5))
#define WASI_ADVICE_WILLNEED   (UINT8_C(6))

typedef uint32_t wasi_clockid_t;
#define WASI_CLOCK_MONOTONIC          (UINT32_C(1))
#define WASI_CLOCK_PROCESS_CPUTIME_ID (UINT32_C(2))
#define WASI_CLOCK_REALTIME           (UINT32_C(3))
#define WASI_CLOCK_THREAD_CPUTIME_ID  (UINT32_C(4))

typedef uint32_t wasi_condvar_t;
#define WASI_CONDVAR_HAS_NO_WAITERS (UINT32_C(0))

typedef uint64_t wasi_device_t;

typedef uint64_t wasi_dircookie_t;
#define WASI_DIRCOOKIE_START (UINT64_C(0))

typedef uint16_t wasi_errno_t;
#define WASI_ESUCCESS         (UINT16_C(0))
#define WASI_E2BIG            (UINT16_C(1))
#define WASI_EACCES           (UINT16_C(2))
#define WASI_EADDRINUSE       (UINT16_C(3))
#define WASI_EADDRNOTAVAIL    (UINT16_C(4))
#define WASI_EAFNOSUPPORT     (UINT16_C(5))
#define WASI_EAGAIN           (UINT16_C(6))
#define WASI_EALREADY         (UINT16_C(7))
#define WASI_EBADF            (UINT16_C(8))
#define WASI_EBADMSG          (UINT16_C(9))
#define WASI_EBUSY           (UINT16_C(10))
#define WASI_ECANCELED       (UINT16_C(11))
#define WASI_ECHILD          (UINT16_C(12))
#define WASI_ECONNABORTED    (UINT16_C(13))
#define WASI_ECONNREFUSED    (UINT16_C(14))
#define WASI_ECONNRESET      (UINT16_C(15))
#define WASI_EDEADLK         (UINT16_C(16))
#define WASI_EDESTADDRREQ    (UINT16_C(17))
#define WASI_EDOM            (UINT16_C(18))
#define WASI_EDQUOT          (UINT16_C(19))
#define WASI_EEXIST          (UINT16_C(20))
#define WASI_EFAULT          (UINT16_C(21))
#define WASI_EFBIG           (UINT16_C(22))
#define WASI_EHOSTUNREACH    (UINT16_C(23))
#define WASI_EIDRM           (UINT16_C(24))
#define WASI_EILSEQ          (UINT16_C(25))
#define WASI_EINPROGRESS     (UINT16_C(26))
#define WASI_EINTR           (UINT16_C(27))
#define WASI_EINVAL          (UINT16_C(28))
#define WASI_EIO             (UINT16_C(29))
#define WASI_EISCONN         (UINT16_C(30))
#define WASI_EISDIR          (UINT16_C(31))
#define WASI_ELOOP           (UINT16_C(32))
#define WASI_EMFILE          (UINT16_C(33))
#define WASI_EMLINK          (UINT16_C(34))
#define WASI_EMSGSIZE        (UINT16_C(35))
#define WASI_EMULTIHOP       (UINT16_C(36))
#define WASI_ENAMETOOLONG    (UINT16_C(37))
#define WASI_ENETDOWN        (UINT16_C(38))
#define WASI_ENETRESET       (UINT16_C(39))
#define WASI_ENETUNREACH     (UINT16_C(40))
#define WASI_ENFILE          (UINT16_C(41))
#define WASI_ENOBUFS         (UINT16_C(42))
#define WASI_ENODEV          (UINT16_C(43))
#define WASI_ENOENT          (UINT16_C(44))
#define WASI_ENOEXEC         (UINT16_C(45))
#define WASI_ENOLCK          (UINT16_C(46))
#define WASI_ENOLINK         (UINT16_C(47))
#define WASI_ENOMEM          (UINT16_C(48))
#define WASI_ENOMSG          (UINT16_C(49))
#define WASI_ENOPROTOOPT     (UINT16_C(50))
#define WASI_ENOSPC          (UINT16_C(51))
#define WASI_ENOSYS          (UINT16_C(52))
#define WASI_ENOTCONN        (UINT16_C(53))
#define WASI_ENOTDIR         (UINT16_C(54))
#define WASI_ENOTEMPTY       (UINT16_C(55))
#define WASI_ENOTRECOVERABLE (UINT16_C(56))
#define WASI_ENOTSOCK        (UINT16_C(57))
#define WASI_ENOTSUP         (UINT16_C(58))
#define WASI_ENOTTY          (UINT16_C(59))
#define WASI_ENXIO           (UINT16_C(60))
#define WASI_EOVERFLOW       (UINT16_C(61))
#define WASI_EOWNERDEAD      (UINT16_C(62))
#define WASI_EPERM           (UINT16_C(63))
#define WASI_EPIPE           (UINT16_C(64))
#define WASI_EPROTO          (UINT16_C(65))
#define WASI_EPROTONOSUPPORT (UINT16_C(66))
#define WASI_EPROTOTYPE      (UINT16_C(67))
#define WASI_ERANGE          (UINT16_C(68))
#define WASI_EROFS           (UINT16_C(69))
#define WASI_ESPIPE          (UINT16_C(70))
#define WASI_ESRCH           (UINT16_C(71))
#define WASI_ESTALE          (UINT16_C(72))
#define WASI_ETIMEDOUT       (UINT16_C(73))
#define WASI_ETXTBSY         (UINT16_C(74))
#define WASI_EXDEV           (UINT16_C(75))
#define WASI_ENOTCAPABLE     (UINT16_C(76))

typedef uint16_t wasi_eventrwflags_t;
#define WASI_EVENT_FD_READWRITE_HANGUP (UINT16_C(0x0001))

typedef uint8_t wasi_eventtype_t;
#define WASI_EVENTTYPE_CLOCK          (UINT8_C(1))
#define WASI_EVENTTYPE_CONDVAR        (UINT8_C(2))
#define WASI_EVENTTYPE_FD_READ        (UINT8_C(3))
#define WASI_EVENTTYPE_FD_WRITE       (UINT8_C(4))
#define WASI_EVENTTYPE_LOCK_RDLOCK    (UINT8_C(5))
#define WASI_EVENTTYPE_LOCK_WRLOCK    (UINT8_C(6))
#define WASI_EVENTTYPE_PROC_TERMINATE (UINT8_C(7))

typedef uint32_t wasi_exitcode_t;

typedef uint32_t wasi_fd_t;
#define WASI_PROCESS_CHILD (UINT32_C(0xffffffff))
#define WASI_MAP_ANON_FD   (UINT32_C(0xffffffff))

typedef uint16_t wasi_fdflags_t;
#define WASI_FDFLAG_APPEND   (UINT16_C(0x0001))
#define WASI_FDFLAG_DSYNC    (UINT16_C(0x0002))
#define WASI_FDFLAG_NONBLOCK (UINT16_C(0x0004))
#define WASI_FDFLAG_RSYNC    (UINT16_C(0x0008))
#define WASI_FDFLAG_SYNC     (UINT16_C(0x0010))

typedef uint16_t wasi_fdsflags_t;
#define WASI_FDSTAT_FLAGS  (UINT16_C(0x0001))
#define WASI_FDSTAT_RIGHTS (UINT16_C(0x0002))

typedef int64_t wasi_filedelta_t;

typedef uint64_t wasi_filesize_t;

typedef uint8_t wasi_filetype_t;
#define WASI_FILETYPE_UNKNOWN            (UINT8_C(0))
#define WASI_FILETYPE_BLOCK_DEVICE      (UINT8_C(16))
#define WASI_FILETYPE_CHARACTER_DEVICE  (UINT8_C(17))
#define WASI_FILETYPE_DIRECTORY         (UINT8_C(32))
#define WASI_FILETYPE_PROCESS           (UINT8_C(80))
#define WASI_FILETYPE_REGULAR_FILE      (UINT8_C(96))
#define WASI_FILETYPE_SHARED_MEMORY    (UINT8_C(112))
#define WASI_FILETYPE_SOCKET_DGRAM     (UINT8_C(128))
#define WASI_FILETYPE_SOCKET_STREAM    (UINT8_C(130))
#define WASI_FILETYPE_SYMBOLIC_LINK    (UINT8_C(144))

typedef uint16_t wasi_fsflags_t;
#define WASI_FILESTAT_ATIM     (UINT16_C(0x0001))
#define WASI_FILESTAT_ATIM_NOW (UINT16_C(0x0002))
#define WASI_FILESTAT_MTIM     (UINT16_C(0x0004))
#define WASI_FILESTAT_MTIM_NOW (UINT16_C(0x0008))
#define WASI_FILESTAT_SIZE     (UINT16_C(0x0010))

typedef uint64_t wasi_inode_t;

typedef uint32_t wasi_linkcount_t;

typedef uint32_t wasi_lock_t;
#define WASI_LOCK_UNLOCKED       (UINT32_C(0x00000000))
#define WASI_LOCK_WRLOCKED       (UINT32_C(0x40000000))
#define WASI_LOCK_KERNEL_MANAGED (UINT32_C(0x80000000))
#define WASI_LOCK_BOGUS          (UINT32_C(0x80000000))

typedef uint32_t wasi_lookupflags_t;
#define WASI_LOOKUP_SYMLINK_FOLLOW (UINT32_C(0x00000001))

typedef uint8_t wasi_mflags_t;
#define WASI_MAP_ANON    (UINT8_C(0x01))
#define WASI_MAP_FIXED   (UINT8_C(0x02))
#define WASI_MAP_PRIVATE (UINT8_C(0x04))
#define WASI_MAP_SHARED  (UINT8_C(0x08))

typedef uint8_t wasi_mprot_t;
#define WASI_PROT_EXEC  (UINT8_C(0x01))
#define WASI_PROT_WRITE (UINT8_C(0x02))
#define WASI_PROT_READ  (UINT8_C(0x04))

typedef uint8_t wasi_msflags_t;
#define WASI_MS_ASYNC      (UINT8_C(0x01))
#define WASI_MS_INVALIDATE (UINT8_C(0x02))
#define WASI_MS_SYNC       (UINT8_C(0x04))

typedef uint32_t wasi_nthreads_t;

typedef uint16_t wasi_oflags_t;
#define WASI_O_CREAT     (UINT16_C(0x0001))
#define WASI_O_DIRECTORY (UINT16_C(0x0002))
#define WASI_O_EXCL      (UINT16_C(0x0004))
#define WASI_O_TRUNC     (UINT16_C(0x0008))

typedef uint16_t wasi_riflags_t;
#define WASI_SOCK_RECV_PEEK    (UINT16_C(0x0004))
#define WASI_SOCK_RECV_WAITALL (UINT16_C(0x0010))

typedef uint64_t wasi_rights_t;
#define WASI_RIGHT_FD_DATASYNC           (UINT64_C(0x0000000000000001))
#define WASI_RIGHT_FD_READ               (UINT64_C(0x0000000000000002))
#define WASI_RIGHT_FD_SEEK               (UINT64_C(0x0000000000000004))
#define WASI_RIGHT_FD_STAT_PUT_FLAGS     (UINT64_C(0x0000000000000008))
#define WASI_RIGHT_FD_SYNC               (UINT64_C(0x0000000000000010))
#define WASI_RIGHT_FD_TELL               (UINT64_C(0x0000000000000020))
#define WASI_RIGHT_FD_WRITE              (UINT64_C(0x0000000000000040))
#define WASI_RIGHT_FILE_ADVISE           (UINT64_C(0x0000000000000080))
#define WASI_RIGHT_FILE_ALLOCATE         (UINT64_C(0x0000000000000100))
#define WASI_RIGHT_FILE_CREATE_DIRECTORY (UINT64_C(0x0000000000000200))
#define WASI_RIGHT_FILE_CREATE_FILE      (UINT64_C(0x0000000000000400))
#define WASI_RIGHT_FILE_LINK_SOURCE      (UINT64_C(0x0000000000001000))
#define WASI_RIGHT_FILE_LINK_TARGET      (UINT64_C(0x0000000000002000))
#define WASI_RIGHT_FILE_OPEN             (UINT64_C(0x0000000000004000))
#define WASI_RIGHT_FILE_READDIR          (UINT64_C(0x0000000000008000))
#define WASI_RIGHT_FILE_READLINK         (UINT64_C(0x0000000000010000))
#define WASI_RIGHT_FILE_RENAME_SOURCE    (UINT64_C(0x0000000000020000))
#define WASI_RIGHT_FILE_RENAME_TARGET    (UINT64_C(0x0000000000040000))
#define WASI_RIGHT_FILE_STAT_FGET        (UINT64_C(0x0000000000080000))
#define WASI_RIGHT_FILE_STAT_FPUT_SIZE   (UINT64_C(0x0000000000100000))
#define WASI_RIGHT_FILE_STAT_FPUT_TIMES  (UINT64_C(0x0000000000200000))
#define WASI_RIGHT_FILE_STAT_GET         (UINT64_C(0x0000000000400000))
#define WASI_RIGHT_FILE_STAT_PUT_TIMES   (UINT64_C(0x0000000000800000))
#define WASI_RIGHT_FILE_SYMLINK          (UINT64_C(0x0000000001000000))
#define WASI_RIGHT_FILE_UNLINK           (UINT64_C(0x0000000002000000))
#define WASI_RIGHT_MEM_MAP               (UINT64_C(0x0000000004000000))
#define WASI_RIGHT_MEM_MAP_EXEC          (UINT64_C(0x0000000008000000))
#define WASI_RIGHT_POLL_FD_READWRITE     (UINT64_C(0x0000000010000000))
#define WASI_RIGHT_POLL_PROC_TERMINATE   (UINT64_C(0x0000000040000000))
#define WASI_RIGHT_PROC_EXEC             (UINT64_C(0x0000000100000000))
#define WASI_RIGHT_SOCK_SHUTDOWN         (UINT64_C(0x0000008000000000))

typedef uint16_t wasi_roflags_t;
#define WASI_SOCK_RECV_FDS_TRUNCATED  (UINT16_C(0x0001))
#define WASI_SOCK_RECV_DATA_TRUNCATED (UINT16_C(0x0008))

typedef uint8_t wasi_scope_t;
#define WASI_SCOPE_PRIVATE (UINT8_C(4))
#define WASI_SCOPE_SHARED  (UINT8_C(8))

typedef uint8_t wasi_sdflags_t;
#define WASI_SHUT_RD (UINT8_C(0x01))
#define WASI_SHUT_WR (UINT8_C(0x02))

typedef uint16_t wasi_siflags_t;

typedef uint8_t wasi_signal_t;
#define WASI_SIGABRT    (UINT8_C(1))
#define WASI_SIGALRM    (UINT8_C(2))
#define WASI_SIGBUS     (UINT8_C(3))
#define WASI_SIGCHLD    (UINT8_C(4))
#define WASI_SIGCONT    (UINT8_C(5))
#define WASI_SIGFPE     (UINT8_C(6))
#define WASI_SIGHUP     (UINT8_C(7))
#define WASI_SIGILL     (UINT8_C(8))
#define WASI_SIGINT     (UINT8_C(9))
#define WASI_SIGKILL   (UINT8_C(10))
#define WASI_SIGPIPE   (UINT8_C(11))
#define WASI_SIGQUIT   (UINT8_C(12))
#define WASI_SIGSEGV   (UINT8_C(13))
#define WASI_SIGSTOP   (UINT8_C(14))
#define WASI_SIGSYS    (UINT8_C(15))
#define WASI_SIGTERM   (UINT8_C(16))
#define WASI_SIGTRAP   (UINT8_C(17))
#define WASI_SIGTSTP   (UINT8_C(18))
#define WASI_SIGTTIN   (UINT8_C(19))
#define WASI_SIGTTOU   (UINT8_C(20))
#define WASI_SIGURG    (UINT8_C(21))
#define WASI_SIGUSR1   (UINT8_C(22))
#define WASI_SIGUSR2   (UINT8_C(23))
#define WASI_SIGVTALRM (UINT8_C(24))
#define WASI_SIGXCPU   (UINT8_C(25))
#define WASI_SIGXFSZ   (UINT8_C(26))

typedef uint16_t wasi_subclockflags_t;
#define WASI_SUBSCRIPTION_CLOCK_ABSTIME (UINT16_C(0x0001))

typedef uint16_t wasi_subrwflags_t;
#define WASI_SUBSCRIPTION_FD_READWRITE_POLL (UINT16_C(0x0001))

typedef uint32_t wasi_tid_t;

typedef uint64_t wasi_timestamp_t;

typedef uint8_t wasi_ulflags_t;
#define WASI_UNLINK_REMOVEDIR (UINT8_C(0x01))

typedef uint64_t wasi_userdata_t;

typedef uint8_t wasi_whence_t;
#define WASI_WHENCE_CUR (UINT8_C(1))
#define WASI_WHENCE_END (UINT8_C(2))
#define WASI_WHENCE_SET (UINT8_C(3))

typedef struct {
    wasi_dircookie_t d_next;
    wasi_inode_t d_ino;
    uint32_t d_namlen;
    wasi_filetype_t d_type;
} wasi_dirent_t;
_Static_assert(offsetof(
    wasi_dirent_t, d_next) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_dirent_t, d_ino) == 8, "Incorrect layout");
_Static_assert(offsetof(
    wasi_dirent_t, d_namlen) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_dirent_t, d_type) == 20, "Incorrect layout");
_Static_assert(sizeof(wasi_dirent_t) == 24, "Incorrect layout");
_Static_assert(_Alignof(wasi_dirent_t) == 8, "Incorrect layout");

typedef struct {
    wasi_userdata_t userdata;
    wasi_errno_t error;
    wasi_eventtype_t type;
    union {
        struct {
            wasi_filesize_t nbytes;
            char unused[4];
            wasi_eventrwflags_t flags;
        } fd_readwrite;
        struct {
            char unused[4];
            wasi_signal_t signal;
            wasi_exitcode_t exitcode;
        } proc_terminate;
    };
} wasi_event_t;
_Static_assert(offsetof(
    wasi_event_t, userdata) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, error) == 8, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, type) == 10, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, fd_readwrite.nbytes) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, fd_readwrite.unused) == 24, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, fd_readwrite.flags) == 28, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, proc_terminate.unused) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, proc_terminate.signal) == 20, "Incorrect layout");
_Static_assert(offsetof(
    wasi_event_t, proc_terminate.exitcode) == 24, "Incorrect layout");
_Static_assert(sizeof(wasi_event_t) == 32, "Incorrect layout");
_Static_assert(_Alignof(wasi_event_t) == 8, "Incorrect layout");

typedef struct {
    wasi_filetype_t fs_filetype;
    wasi_fdflags_t fs_flags;
    wasi_rights_t fs_rights_base;
    wasi_rights_t fs_rights_inheriting;
} wasi_fdstat_t;
_Static_assert(offsetof(
    wasi_fdstat_t, fs_filetype) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_fdstat_t, fs_flags) == 2, "Incorrect layout");
_Static_assert(offsetof(
    wasi_fdstat_t, fs_rights_base) == 8, "Incorrect layout");
_Static_assert(offsetof(
    wasi_fdstat_t, fs_rights_inheriting) == 16, "Incorrect layout");
_Static_assert(sizeof(wasi_fdstat_t) == 24, "Incorrect layout");
_Static_assert(_Alignof(wasi_fdstat_t) == 8, "Incorrect layout");

typedef struct {
    wasi_device_t st_dev;
    wasi_inode_t st_ino;
    wasi_filetype_t st_filetype;
    wasi_linkcount_t st_nlink;
    wasi_filesize_t st_size;
    wasi_timestamp_t st_atim;
    wasi_timestamp_t st_mtim;
    wasi_timestamp_t st_ctim;
} wasi_filestat_t;
_Static_assert(offsetof(
    wasi_filestat_t, st_dev) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_ino) == 8, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_filetype) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_nlink) == 20, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_size) == 24, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_atim) == 32, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_mtim) == 40, "Incorrect layout");
_Static_assert(offsetof(
    wasi_filestat_t, st_ctim) == 48, "Incorrect layout");
_Static_assert(sizeof(wasi_filestat_t) == 56, "Incorrect layout");
_Static_assert(_Alignof(wasi_filestat_t) == 8, "Incorrect layout");

typedef struct {
    wasi_fd_t fd;
    wasi_lookupflags_t flags;
} wasi_lookup_t;
_Static_assert(offsetof(
    wasi_lookup_t, fd) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_lookup_t, flags) == 4, "Incorrect layout");
_Static_assert(sizeof(wasi_lookup_t) == 8, "Incorrect layout");
_Static_assert(_Alignof(wasi_lookup_t) == 4, "Incorrect layout");

typedef struct {
    const void *buf;
    size_t buf_len;
} wasi_ciovec_t;
_Static_assert(offsetof(
    wasi_ciovec_t, buf) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_ciovec_t, buf_len) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_ciovec_t, buf_len) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_ciovec_t) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_ciovec_t) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_ciovec_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_ciovec_t) == 8, "Incorrect layout");

typedef struct {
    void *buf;
    size_t buf_len;
} wasi_iovec_t;
_Static_assert(offsetof(
    wasi_iovec_t, buf) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_iovec_t, buf_len) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_iovec_t, buf_len) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_iovec_t) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_iovec_t) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_iovec_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_iovec_t) == 8, "Incorrect layout");

typedef struct {
    const wasi_iovec_t *ri_data;
    size_t ri_data_len;
    wasi_fd_t *ri_fds;
    size_t ri_fds_len;
    wasi_riflags_t ri_flags;
} wasi_recv_in_t;
_Static_assert(offsetof(
    wasi_recv_in_t, ri_data) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_in_t, ri_data_len) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_in_t, ri_data_len) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_in_t, ri_fds) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_in_t, ri_fds) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_in_t, ri_fds_len) == 12, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_in_t, ri_fds_len) == 24, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_in_t, ri_flags) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_in_t, ri_flags) == 32, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_recv_in_t) == 20, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_recv_in_t) == 40, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_recv_in_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_recv_in_t) == 8, "Incorrect layout");

typedef struct {
    size_t ro_datalen;
    size_t ro_fdslen;
    char ro_unused[40];
    wasi_roflags_t ro_flags;
} wasi_recv_out_t;
_Static_assert(offsetof(
    wasi_recv_out_t, ro_datalen) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_out_t, ro_fdslen) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_out_t, ro_fdslen) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_out_t, ro_unused) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_out_t, ro_unused) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_recv_out_t, ro_flags) == 48, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_recv_out_t, ro_flags) == 56, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_recv_out_t) == 52, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_recv_out_t) == 64, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_recv_out_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_recv_out_t) == 8, "Incorrect layout");

typedef struct {
    const wasi_ciovec_t *si_data;
    size_t si_data_len;
    const wasi_fd_t *si_fds;
    size_t si_fds_len;
    wasi_siflags_t si_flags;
} wasi_send_in_t;
_Static_assert(offsetof(
    wasi_send_in_t, si_data) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_send_in_t, si_data_len) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_send_in_t, si_data_len) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_send_in_t, si_fds) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_send_in_t, si_fds) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_send_in_t, si_fds_len) == 12, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_send_in_t, si_fds_len) == 24, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_send_in_t, si_flags) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_send_in_t, si_flags) == 32, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_send_in_t) == 20, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_send_in_t) == 40, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_send_in_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_send_in_t) == 8, "Incorrect layout");

typedef struct {
    size_t so_datalen;
} wasi_send_out_t;
_Static_assert(offsetof(
    wasi_send_out_t, so_datalen) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_send_out_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_send_out_t) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_send_out_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_send_out_t) == 8, "Incorrect layout");

typedef struct {
    wasi_userdata_t userdata;
    uint16_t unused;
    wasi_eventtype_t type;
    union {
        struct {
            wasi_userdata_t identifier;
            wasi_clockid_t clock_id;
            wasi_timestamp_t timeout;
            wasi_timestamp_t precision;
            wasi_subclockflags_t flags;
        } clock;
        struct {
            _Atomic(wasi_condvar_t) *condvar;
            _Atomic(wasi_lock_t) *lock;
            wasi_scope_t condvar_scope;
            wasi_scope_t lock_scope;
        } condvar;
        struct {
            wasi_fd_t fd;
            wasi_subrwflags_t flags;
        } fd_readwrite;
        struct {
            _Atomic(wasi_lock_t) *lock;
            wasi_scope_t lock_scope;
        } lock;
        struct {
            wasi_fd_t fd;
        } proc_terminate;
    };
} wasi_subscription_t;
_Static_assert(offsetof(
    wasi_subscription_t, userdata) == 0, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, unused) == 8, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, type) == 10, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, clock.identifier) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, clock.clock_id) == 24, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, clock.timeout) == 32, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, clock.precision) == 40, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, clock.flags) == 48, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, condvar.condvar) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_subscription_t, condvar.lock) == 20, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_subscription_t, condvar.lock) == 24, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_subscription_t, condvar.condvar_scope) == 24, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_subscription_t, condvar.condvar_scope) == 32, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_subscription_t, condvar.lock_scope) == 25, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_subscription_t, condvar.lock_scope) == 33, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, fd_readwrite.fd) == 16, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, fd_readwrite.flags) == 20, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, lock.lock) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_subscription_t, lock.lock_scope) == 20, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_subscription_t, lock.lock_scope) == 24, "Incorrect layout");
_Static_assert(offsetof(
    wasi_subscription_t, proc_terminate.fd) == 16, "Incorrect layout");
_Static_assert(sizeof(wasi_subscription_t) == 56, "Incorrect layout");
_Static_assert(_Alignof(wasi_subscription_t) == 8, "Incorrect layout");

typedef struct {
    void *parent;
} wasi_tcb_t;
_Static_assert(offsetof(
    wasi_tcb_t, parent) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_tcb_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_tcb_t) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_tcb_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_tcb_t) == 8, "Incorrect layout");

typedef void wasi_threadentry_t(
    wasi_tid_t tid,
    void *aux
);

typedef struct {
    wasi_threadentry_t *entry_point;
    void *stack;
    size_t stack_len;
    void *argument;
} wasi_threadattr_t;
_Static_assert(offsetof(
    wasi_threadattr_t, entry_point) == 0, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_threadattr_t, stack) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_threadattr_t, stack) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_threadattr_t, stack_len) == 8, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_threadattr_t, stack_len) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || offsetof(
    wasi_threadattr_t, argument) == 12, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || offsetof(
    wasi_threadattr_t, argument) == 24, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || sizeof(
    wasi_threadattr_t) == 16, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || sizeof(
    wasi_threadattr_t) == 32, "Incorrect layout");
_Static_assert(sizeof(void *) != 4 || _Alignof(
    wasi_threadattr_t) == 4, "Incorrect layout");
_Static_assert(sizeof(void *) != 8 || _Alignof(
    wasi_threadattr_t) == 8, "Incorrect layout");

wasi_errno_t wasi_clock_res_get(
    wasi_clockid_t clock_id,
    wasi_timestamp_t *resolution
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_clock_time_get(
    wasi_clockid_t clock_id,
    wasi_timestamp_t precision,
    wasi_timestamp_t *time
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_condvar_signal(
    _Atomic(wasi_condvar_t) *condvar,
    wasi_scope_t scope,
    wasi_nthreads_t nwaiters
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_close(
    wasi_fd_t fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_create1(
    wasi_filetype_t type,
    wasi_fd_t *fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_create2(
    wasi_filetype_t type,
    wasi_fd_t *fd1,
    wasi_fd_t *fd2
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_datasync(
    wasi_fd_t fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_dup(
    wasi_fd_t from,
    wasi_fd_t *fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_pread(
    wasi_fd_t fd,
    const wasi_iovec_t *iovs,
    size_t iovs_len,
    wasi_filesize_t offset,
    size_t *nread
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_pwrite(
    wasi_fd_t fd,
    const wasi_ciovec_t *iovs,
    size_t iovs_len,
    wasi_filesize_t offset,
    size_t *nwritten
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_read(
    wasi_fd_t fd,
    const wasi_iovec_t *iovs,
    size_t iovs_len,
    size_t *nread
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_replace(
    wasi_fd_t from,
    wasi_fd_t to
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_seek(
    wasi_fd_t fd,
    wasi_filedelta_t offset,
    wasi_whence_t whence,
    wasi_filesize_t *newoffset
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_stat_get(
    wasi_fd_t fd,
    wasi_fdstat_t *buf
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_stat_put(
    wasi_fd_t fd,
    const wasi_fdstat_t *buf,
    wasi_fdsflags_t flags
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_sync(
    wasi_fd_t fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_fd_write(
    wasi_fd_t fd,
    const wasi_ciovec_t *iovs,
    size_t iovs_len,
    size_t *nwritten
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_advise(
    wasi_fd_t fd,
    wasi_filesize_t offset,
    wasi_filesize_t len,
    wasi_advice_t advice
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_allocate(
    wasi_fd_t fd,
    wasi_filesize_t offset,
    wasi_filesize_t len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_create(
    wasi_fd_t fd,
    const char *path,
    size_t path_len,
    wasi_filetype_t type
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_link(
    wasi_lookup_t fd1,
    const char *path1,
    size_t path1_len,
    wasi_fd_t fd2,
    const char *path2,
    size_t path2_len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_open(
    wasi_lookup_t dirfd,
    const char *path,
    size_t path_len,
    wasi_oflags_t oflags,
    const wasi_fdstat_t *fds,
    wasi_fd_t *fd
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_readdir(
    wasi_fd_t fd,
    void *buf,
    size_t buf_len,
    wasi_dircookie_t cookie,
    size_t *bufused
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_readlink(
    wasi_fd_t fd,
    const char *path,
    size_t path_len,
    char *buf,
    size_t buf_len,
    size_t *bufused
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_rename(
    wasi_fd_t fd1,
    const char *path1,
    size_t path1_len,
    wasi_fd_t fd2,
    const char *path2,
    size_t path2_len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_stat_fget(
    wasi_fd_t fd,
    wasi_filestat_t *buf
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_stat_fput(
    wasi_fd_t fd,
    const wasi_filestat_t *buf,
    wasi_fsflags_t flags
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_stat_get(
    wasi_lookup_t fd,
    const char *path,
    size_t path_len,
    wasi_filestat_t *buf
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_stat_put(
    wasi_lookup_t fd,
    const char *path,
    size_t path_len,
    const wasi_filestat_t *buf,
    wasi_fsflags_t flags
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_symlink(
    const char *path1,
    size_t path1_len,
    wasi_fd_t fd,
    const char *path2,
    size_t path2_len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_file_unlink(
    wasi_fd_t fd,
    const char *path,
    size_t path_len,
    wasi_ulflags_t flags
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_lock_unlock(
    _Atomic(wasi_lock_t) *lock,
    wasi_scope_t scope
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_mem_advise(
    void *mapping,
    size_t mapping_len,
    wasi_advice_t advice
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_mem_map(
    void *addr,
    size_t len,
    wasi_mprot_t prot,
    wasi_mflags_t flags,
    wasi_fd_t fd,
    wasi_filesize_t off,
    void **mem
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_mem_protect(
    void *mapping,
    size_t mapping_len,
    wasi_mprot_t prot
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_mem_sync(
    void *mapping,
    size_t mapping_len,
    wasi_msflags_t flags
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_mem_unmap(
    void *mapping,
    size_t mapping_len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_poll(
    const wasi_subscription_t *in,
    wasi_event_t *out,
    size_t nsubscriptions,
    size_t *nevents
) __attribute__((__warn_unused_result__));

_Noreturn void wasi_proc_exit(
    wasi_exitcode_t rval
);

wasi_errno_t wasi_proc_raise(
    wasi_signal_t sig
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_random_get(
    void *buf,
    size_t buf_len
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_sock_recv(
    wasi_fd_t sock,
    const wasi_recv_in_t *in,
    wasi_recv_out_t *out
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_sock_send(
    wasi_fd_t sock,
   const wasi_send_in_t *in,
   wasi_send_out_t *out
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_sock_shutdown(
    wasi_fd_t sock,
    wasi_sdflags_t how
) __attribute__((__warn_unused_result__));

wasi_errno_t wasi_thread_create(
    wasi_threadattr_t *attr,
    wasi_tid_t *tid
) __attribute__((__warn_unused_result__));

_Noreturn void wasi_thread_exit(
    _Atomic(wasi_lock_t) *lock,
    wasi_scope_t scope
);

wasi_errno_t wasi_thread_yield(void)
    __attribute__((__warn_unused_result__));

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
