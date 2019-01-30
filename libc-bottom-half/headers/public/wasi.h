/*
 * This file describes the WASI interface, consisting of functions, types,
 * and defined values (macros).
 *
 * The interface described here is greatly inspired by [CloudABI]'s clean,
 * thoughtfully-designed, cabability-oriented, POSIX-style API.
 *
 * [CloudABI]: https://github.com/NuxiNL/cloudlibc
 */

#ifndef __wasi_h
#define __wasi_h

#ifndef __wasi__
#error <wasi.h> is only supported on WASI platforms.
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(__cplusplus)
#define _Alignof alignof
#define _Atomic(x) x
#define _Static_assert static_assert
#endif

_Static_assert(_Alignof(int8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(uint8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(int16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(uint16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(int32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(uint32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(int64_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(uint64_t) == 8, "non-wasi data layout");

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t __wasi_advice_t;
#define __WASI_ADVICE_NORMAL     (UINT8_C(0))
#define __WASI_ADVICE_SEQUENTIAL (UINT8_C(1))
#define __WASI_ADVICE_RANDOM     (UINT8_C(2))
#define __WASI_ADVICE_WILLNEED   (UINT8_C(3))
#define __WASI_ADVICE_DONTNEED   (UINT8_C(4))
#define __WASI_ADVICE_NOREUSE    (UINT8_C(5))

typedef uint32_t __wasi_clockid_t;
#define __WASI_CLOCK_REALTIME           (UINT32_C(0))
#define __WASI_CLOCK_MONOTONIC          (UINT32_C(1))
#define __WASI_CLOCK_PROCESS_CPUTIME_ID (UINT32_C(2))
#define __WASI_CLOCK_THREAD_CPUTIME_ID  (UINT32_C(3))

typedef uint32_t __wasi_condvar_t;
#define __WASI_CONDVAR_HAS_NO_WAITERS (UINT32_C(0))

typedef uint64_t __wasi_device_t;

typedef uint64_t __wasi_dircookie_t;
#define __WASI_DIRCOOKIE_START (UINT64_C(0))

typedef uint16_t __wasi_errno_t;
#define __WASI_ESUCCESS        (UINT16_C(0))
#define __WASI_E2BIG           (UINT16_C(1))
#define __WASI_EACCES          (UINT16_C(2))
#define __WASI_EADDRINUSE      (UINT16_C(3))
#define __WASI_EADDRNOTAVAIL   (UINT16_C(4))
#define __WASI_EAFNOSUPPORT    (UINT16_C(5))
#define __WASI_EAGAIN          (UINT16_C(6))
#define __WASI_EALREADY        (UINT16_C(7))
#define __WASI_EBADF           (UINT16_C(8))
#define __WASI_EBADMSG         (UINT16_C(9))
#define __WASI_EBUSY           (UINT16_C(10))
#define __WASI_ECANCELED       (UINT16_C(11))
#define __WASI_ECHILD          (UINT16_C(12))
#define __WASI_ECONNABORTED    (UINT16_C(13))
#define __WASI_ECONNREFUSED    (UINT16_C(14))
#define __WASI_ECONNRESET      (UINT16_C(15))
#define __WASI_EDEADLK         (UINT16_C(16))
#define __WASI_EDESTADDRREQ    (UINT16_C(17))
#define __WASI_EDOM            (UINT16_C(18))
#define __WASI_EDQUOT          (UINT16_C(19))
#define __WASI_EEXIST          (UINT16_C(20))
#define __WASI_EFAULT          (UINT16_C(21))
#define __WASI_EFBIG           (UINT16_C(22))
#define __WASI_EHOSTUNREACH    (UINT16_C(23))
#define __WASI_EIDRM           (UINT16_C(24))
#define __WASI_EILSEQ          (UINT16_C(25))
#define __WASI_EINPROGRESS     (UINT16_C(26))
#define __WASI_EINTR           (UINT16_C(27))
#define __WASI_EINVAL          (UINT16_C(28))
#define __WASI_EIO             (UINT16_C(29))
#define __WASI_EISCONN         (UINT16_C(30))
#define __WASI_EISDIR          (UINT16_C(31))
#define __WASI_ELOOP           (UINT16_C(32))
#define __WASI_EMFILE          (UINT16_C(33))
#define __WASI_EMLINK          (UINT16_C(34))
#define __WASI_EMSGSIZE        (UINT16_C(35))
#define __WASI_EMULTIHOP       (UINT16_C(36))
#define __WASI_ENAMETOOLONG    (UINT16_C(37))
#define __WASI_ENETDOWN        (UINT16_C(38))
#define __WASI_ENETRESET       (UINT16_C(39))
#define __WASI_ENETUNREACH     (UINT16_C(40))
#define __WASI_ENFILE          (UINT16_C(41))
#define __WASI_ENOBUFS         (UINT16_C(42))
#define __WASI_ENODEV          (UINT16_C(43))
#define __WASI_ENOENT          (UINT16_C(44))
#define __WASI_ENOEXEC         (UINT16_C(45))
#define __WASI_ENOLCK          (UINT16_C(46))
#define __WASI_ENOLINK         (UINT16_C(47))
#define __WASI_ENOMEM          (UINT16_C(48))
#define __WASI_ENOMSG          (UINT16_C(49))
#define __WASI_ENOPROTOOPT     (UINT16_C(50))
#define __WASI_ENOSPC          (UINT16_C(51))
#define __WASI_ENOSYS          (UINT16_C(52))
#define __WASI_ENOTCONN        (UINT16_C(53))
#define __WASI_ENOTDIR         (UINT16_C(54))
#define __WASI_ENOTEMPTY       (UINT16_C(55))
#define __WASI_ENOTRECOVERABLE (UINT16_C(56))
#define __WASI_ENOTSOCK        (UINT16_C(57))
#define __WASI_ENOTSUP         (UINT16_C(58))
#define __WASI_ENOTTY          (UINT16_C(59))
#define __WASI_ENXIO           (UINT16_C(60))
#define __WASI_EOVERFLOW       (UINT16_C(61))
#define __WASI_EOWNERDEAD      (UINT16_C(62))
#define __WASI_EPERM           (UINT16_C(63))
#define __WASI_EPIPE           (UINT16_C(64))
#define __WASI_EPROTO          (UINT16_C(65))
#define __WASI_EPROTONOSUPPORT (UINT16_C(66))
#define __WASI_EPROTOTYPE      (UINT16_C(67))
#define __WASI_ERANGE          (UINT16_C(68))
#define __WASI_EROFS           (UINT16_C(69))
#define __WASI_ESPIPE          (UINT16_C(70))
#define __WASI_ESRCH           (UINT16_C(71))
#define __WASI_ESTALE          (UINT16_C(72))
#define __WASI_ETIMEDOUT       (UINT16_C(73))
#define __WASI_ETXTBSY         (UINT16_C(74))
#define __WASI_EXDEV           (UINT16_C(75))
#define __WASI_ENOTCAPABLE     (UINT16_C(76))

typedef uint16_t __wasi_eventrwflags_t;
#define __WASI_EVENT_FD_READWRITE_HANGUP (UINT16_C(0x0001))

typedef uint8_t __wasi_eventtype_t;
#define __WASI_EVENTTYPE_CLOCK          (UINT8_C(0))
#define __WASI_EVENTTYPE_CONDVAR        (UINT8_C(1))
#define __WASI_EVENTTYPE_FD_READ        (UINT8_C(2))
#define __WASI_EVENTTYPE_FD_WRITE       (UINT8_C(3))
#define __WASI_EVENTTYPE_LOCK_RDLOCK    (UINT8_C(4))
#define __WASI_EVENTTYPE_LOCK_WRLOCK    (UINT8_C(5))
#define __WASI_EVENTTYPE_PROC_TERMINATE (UINT8_C(6))

typedef uint32_t __wasi_exitcode_t;

typedef uint32_t __wasi_fd_t;
#define __WASI_MAP_ANON_FD (UINT32_C(0xffffffff))

typedef uint16_t __wasi_fdflags_t;
#define __WASI_FDFLAG_APPEND   (UINT16_C(0x0001))
#define __WASI_FDFLAG_DSYNC    (UINT16_C(0x0002))
#define __WASI_FDFLAG_NONBLOCK (UINT16_C(0x0004))
#define __WASI_FDFLAG_RSYNC    (UINT16_C(0x0008))
#define __WASI_FDFLAG_SYNC     (UINT16_C(0x0010))

typedef uint16_t __wasi_fdsflags_t;
#define __WASI_FDSTAT_FLAGS  (UINT16_C(0x0001))
#define __WASI_FDSTAT_RIGHTS (UINT16_C(0x0002))

typedef int64_t __wasi_filedelta_t;

typedef uint64_t __wasi_filesize_t;

typedef uint8_t __wasi_filetype_t;
#define __WASI_FILETYPE_UNKNOWN          (UINT8_C(0))
#define __WASI_FILETYPE_BLOCK_DEVICE     (UINT8_C(1))
#define __WASI_FILETYPE_CHARACTER_DEVICE (UINT8_C(2))
#define __WASI_FILETYPE_DIRECTORY        (UINT8_C(3))
#define __WASI_FILETYPE_PROCESS          (UINT8_C(4))
#define __WASI_FILETYPE_REGULAR_FILE     (UINT8_C(5))
#define __WASI_FILETYPE_SHARED_MEMORY    (UINT8_C(6))
#define __WASI_FILETYPE_SOCKET_DGRAM     (UINT8_C(7))
#define __WASI_FILETYPE_SOCKET_STREAM    (UINT8_C(8))
#define __WASI_FILETYPE_SYMBOLIC_LINK    (UINT8_C(9))

typedef uint16_t __wasi_fsflags_t;
#define __WASI_FILESTAT_ATIM     (UINT16_C(0x0001))
#define __WASI_FILESTAT_ATIM_NOW (UINT16_C(0x0002))
#define __WASI_FILESTAT_MTIM     (UINT16_C(0x0004))
#define __WASI_FILESTAT_MTIM_NOW (UINT16_C(0x0008))
#define __WASI_FILESTAT_SIZE     (UINT16_C(0x0010))

typedef uint64_t __wasi_inode_t;

typedef uint32_t __wasi_linkcount_t;

typedef uint32_t __wasi_lock_t;
#define __WASI_LOCK_UNLOCKED       (UINT32_C(0x00000001))
#define __WASI_LOCK_WRLOCKED       (UINT32_C(0x00000002))
#define __WASI_LOCK_KERNEL_MANAGED (UINT32_C(0x00000004))
#define __WASI_LOCK_BOGUS          (UINT32_C(0x00000004))

typedef uint32_t __wasi_lookupflags_t;
#define __WASI_LOOKUP_SYMLINK_FOLLOW (UINT32_C(0x00000001))

typedef uint8_t __wasi_mflags_t;
#define __WASI_MAP_ANON    (UINT8_C(0x01))
#define __WASI_MAP_FIXED   (UINT8_C(0x02))
#define __WASI_MAP_PRIVATE (UINT8_C(0x04))
#define __WASI_MAP_SHARED  (UINT8_C(0x08))

typedef uint8_t __wasi_mprot_t;
#define __WASI_PROT_READ  (UINT8_C(0x01))
#define __WASI_PROT_WRITE (UINT8_C(0x02))
#define __WASI_PROT_EXEC  (UINT8_C(0x04))

typedef uint8_t __wasi_msflags_t;
#define __WASI_MS_ASYNC      (UINT8_C(0x01))
#define __WASI_MS_INVALIDATE (UINT8_C(0x02))
#define __WASI_MS_SYNC       (UINT8_C(0x04))

typedef uint32_t __wasi_nthreads_t;

typedef uint16_t __wasi_oflags_t;
#define __WASI_O_CREAT     (UINT16_C(0x0001))
#define __WASI_O_DIRECTORY (UINT16_C(0x0002))
#define __WASI_O_EXCL      (UINT16_C(0x0004))
#define __WASI_O_TRUNC     (UINT16_C(0x0008))

typedef uint16_t __wasi_riflags_t;
#define __WASI_SOCK_RECV_PEEK    (UINT16_C(0x0001))
#define __WASI_SOCK_RECV_WAITALL (UINT16_C(0x0002))

typedef uint64_t __wasi_rights_t;
#define __WASI_RIGHT_FD_DATASYNC           (UINT64_C(0x0000000000000001))
#define __WASI_RIGHT_FD_READ               (UINT64_C(0x0000000000000002))
#define __WASI_RIGHT_FD_SEEK               (UINT64_C(0x0000000000000004))
#define __WASI_RIGHT_FD_STAT_PUT_FLAGS     (UINT64_C(0x0000000000000008))
#define __WASI_RIGHT_FD_SYNC               (UINT64_C(0x0000000000000010))
#define __WASI_RIGHT_FD_TELL               (UINT64_C(0x0000000000000020))
#define __WASI_RIGHT_FD_WRITE              (UINT64_C(0x0000000000000040))
#define __WASI_RIGHT_FILE_ADVISE           (UINT64_C(0x0000000000000080))
#define __WASI_RIGHT_FILE_ALLOCATE         (UINT64_C(0x0000000000000100))
#define __WASI_RIGHT_FILE_CREATE_DIRECTORY (UINT64_C(0x0000000000000200))
#define __WASI_RIGHT_FILE_CREATE_FILE      (UINT64_C(0x0000000000000400))
#define __WASI_RIGHT_FILE_LINK_SOURCE      (UINT64_C(0x0000000000000800))
#define __WASI_RIGHT_FILE_LINK_TARGET      (UINT64_C(0x0000000000001000))
#define __WASI_RIGHT_FILE_OPEN             (UINT64_C(0x0000000000002000))
#define __WASI_RIGHT_FILE_READDIR          (UINT64_C(0x0000000000004000))
#define __WASI_RIGHT_FILE_READLINK         (UINT64_C(0x0000000000008000))
#define __WASI_RIGHT_FILE_RENAME_SOURCE    (UINT64_C(0x0000000000010000))
#define __WASI_RIGHT_FILE_RENAME_TARGET    (UINT64_C(0x0000000000020000))
#define __WASI_RIGHT_FILE_STAT_FGET        (UINT64_C(0x0000000000040000))
#define __WASI_RIGHT_FILE_STAT_FPUT_SIZE   (UINT64_C(0x0000000000080000))
#define __WASI_RIGHT_FILE_STAT_FPUT_TIMES  (UINT64_C(0x0000000000100000))
#define __WASI_RIGHT_FILE_STAT_GET         (UINT64_C(0x0000000000200000))
#define __WASI_RIGHT_FILE_STAT_PUT_TIMES   (UINT64_C(0x0000000000400000))
#define __WASI_RIGHT_FILE_SYMLINK          (UINT64_C(0x0000000000800000))
#define __WASI_RIGHT_FILE_UNLINK           (UINT64_C(0x0000000001000000))
#define __WASI_RIGHT_MEM_MAP               (UINT64_C(0x0000000002000000))
#define __WASI_RIGHT_POLL_FD_READWRITE     (UINT64_C(0x0000000004000000))
#define __WASI_RIGHT_POLL_PROC_TERMINATE   (UINT64_C(0x0000000008000000))
#define __WASI_RIGHT_SOCK_SHUTDOWN         (UINT64_C(0x0000000010000000))

typedef uint16_t __wasi_roflags_t;
#define __WASI_SOCK_RECV_FDS_TRUNCATED  (UINT16_C(0x0001))
#define __WASI_SOCK_RECV_DATA_TRUNCATED (UINT16_C(0x0002))

typedef uint8_t __wasi_scope_t;
#define __WASI_SCOPE_PRIVATE (UINT8_C(0))
#define __WASI_SCOPE_SHARED  (UINT8_C(1))

typedef uint8_t __wasi_sdflags_t;
#define __WASI_SHUT_RD (UINT8_C(0x01))
#define __WASI_SHUT_WR (UINT8_C(0x02))

typedef uint16_t __wasi_siflags_t;

typedef uint8_t __wasi_signal_t;
// UINT8_C(0) is reserved; POSIX has special semantics for kill(pid, 0).
#define __WASI_SIGHUP    (UINT8_C(1))
#define __WASI_SIGINT    (UINT8_C(2))
#define __WASI_SIGQUIT   (UINT8_C(3))
#define __WASI_SIGILL    (UINT8_C(4))
#define __WASI_SIGTRAP   (UINT8_C(5))
#define __WASI_SIGABRT   (UINT8_C(6))
#define __WASI_SIGBUS    (UINT8_C(7))
#define __WASI_SIGFPE    (UINT8_C(8))
#define __WASI_SIGKILL   (UINT8_C(9))
#define __WASI_SIGUSR1   (UINT8_C(10))
#define __WASI_SIGSEGV   (UINT8_C(11))
#define __WASI_SIGUSR2   (UINT8_C(12))
#define __WASI_SIGPIPE   (UINT8_C(13))
#define __WASI_SIGALRM   (UINT8_C(14))
#define __WASI_SIGTERM   (UINT8_C(15))
#define __WASI_SIGCHLD   (UINT8_C(16))
#define __WASI_SIGCONT   (UINT8_C(17))
#define __WASI_SIGSTOP   (UINT8_C(18))
#define __WASI_SIGTSTP   (UINT8_C(19))
#define __WASI_SIGTTIN   (UINT8_C(20))
#define __WASI_SIGTTOU   (UINT8_C(21))
#define __WASI_SIGURG    (UINT8_C(22))
#define __WASI_SIGXCPU   (UINT8_C(23))
#define __WASI_SIGXFSZ   (UINT8_C(24))
#define __WASI_SIGVTALRM (UINT8_C(25))
#define __WASI_SIGPROF   (UINT8_C(26))
#define __WASI_SIGWINCH  (UINT8_C(27))
#define __WASI_SIGPOLL   (UINT8_C(28))
#define __WASI_SIGPWR    (UINT8_C(29))
#define __WASI_SIGSYS    (UINT8_C(30))

typedef uint16_t __wasi_subclockflags_t;
#define __WASI_SUBSCRIPTION_CLOCK_ABSTIME (UINT16_C(0x0001))

typedef uint16_t __wasi_subrwflags_t;
#define __WASI_SUBSCRIPTION_FD_READWRITE_POLL (UINT16_C(0x0001))

typedef uint32_t __wasi_tid_t;

typedef uint64_t __wasi_timestamp_t;

typedef uint8_t __wasi_ulflags_t;
#define __WASI_UNLINK_REMOVEDIR (UINT8_C(0x01))

typedef uint64_t __wasi_userdata_t;

typedef uint8_t __wasi_whence_t;
#define __WASI_WHENCE_CUR (UINT8_C(0))
#define __WASI_WHENCE_END (UINT8_C(1))
#define __WASI_WHENCE_SET (UINT8_C(2))

typedef struct __wasi_dirent_t {
    __wasi_dircookie_t d_next;
    __wasi_inode_t d_ino;
    uint32_t d_namlen;
    __wasi_filetype_t d_type;
} __wasi_dirent_t;
_Static_assert(offsetof(__wasi_dirent_t, d_next) == 0, "non-wasi data layout");
_Static_assert(offsetof(__wasi_dirent_t, d_ino) == 8, "non-wasi data layout");
_Static_assert(offsetof(__wasi_dirent_t, d_namlen) == 16, "non-wasi data layout");
_Static_assert(offsetof(__wasi_dirent_t, d_type) == 20, "non-wasi data layout");
_Static_assert(sizeof(__wasi_dirent_t) == 24, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_dirent_t) == 8, "non-wasi data layout");

typedef struct __wasi_event_t {
    __wasi_userdata_t userdata;
    __wasi_errno_t error;
    __wasi_eventtype_t type;
    union {
        struct {
            __wasi_filesize_t nbytes;
            char unused[4];
            __wasi_eventrwflags_t flags;
        } fd_readwrite;
        struct {
            char unused[4];
            __wasi_signal_t signal;
            __wasi_exitcode_t exitcode;
        } proc_terminate;
    };
} __wasi_event_t;
_Static_assert(offsetof(__wasi_event_t, userdata) == 0, "non-wasi data layout");
_Static_assert(offsetof(__wasi_event_t, error) == 8, "non-wasi data layout");
_Static_assert(offsetof(__wasi_event_t, type) == 10, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, fd_readwrite.nbytes) == 16, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, fd_readwrite.unused) == 24, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, fd_readwrite.flags) == 28, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, proc_terminate.unused) == 16,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, proc_terminate.signal) == 20,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_event_t, proc_terminate.exitcode) == 24,
    "non-wasi data layout");
_Static_assert(sizeof(__wasi_event_t) == 32, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_event_t) == 8, "non-wasi data layout");

typedef struct __wasi_fdstat_t {
    __wasi_filetype_t fs_filetype;
    __wasi_fdflags_t fs_flags;
    __wasi_rights_t fs_rights_base;
    __wasi_rights_t fs_rights_inheriting;
} __wasi_fdstat_t;
_Static_assert(
    offsetof(__wasi_fdstat_t, fs_filetype) == 0, "non-wasi data layout");
_Static_assert(offsetof(__wasi_fdstat_t, fs_flags) == 2, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_fdstat_t, fs_rights_base) == 8, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_fdstat_t, fs_rights_inheriting) == 16,
    "non-wasi data layout");
_Static_assert(sizeof(__wasi_fdstat_t) == 24, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_fdstat_t) == 8, "non-wasi data layout");

typedef struct __wasi_filestat_t {
    __wasi_device_t st_dev;
    __wasi_inode_t st_ino;
    __wasi_filetype_t st_filetype;
    __wasi_linkcount_t st_nlink;
    __wasi_filesize_t st_size;
    __wasi_timestamp_t st_atim;
    __wasi_timestamp_t st_mtim;
    __wasi_timestamp_t st_ctim;
} __wasi_filestat_t;
_Static_assert(offsetof(__wasi_filestat_t, st_dev) == 0, "non-wasi data layout");
_Static_assert(offsetof(__wasi_filestat_t, st_ino) == 8, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_filetype) == 16, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_nlink) == 20, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_size) == 24, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_atim) == 32, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_mtim) == 40, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_filestat_t, st_ctim) == 48, "non-wasi data layout");
_Static_assert(sizeof(__wasi_filestat_t) == 56, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_filestat_t) == 8, "non-wasi data layout");

typedef struct __wasi_lookup_t {
    __wasi_fd_t fd;
    __wasi_lookupflags_t flags;
} __wasi_lookup_t;
_Static_assert(offsetof(__wasi_lookup_t, fd) == 0, "non-wasi data layout");
_Static_assert(offsetof(__wasi_lookup_t, flags) == 4, "non-wasi data layout");
_Static_assert(sizeof(__wasi_lookup_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_lookup_t) == 4, "non-wasi data layout");

typedef struct __wasi_ciovec_t {
    const void *buf;
    size_t buf_len;
} __wasi_ciovec_t;
_Static_assert(offsetof(__wasi_ciovec_t, buf) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_ciovec_t, buf_len) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_ciovec_t, buf_len) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_ciovec_t) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_ciovec_t) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_ciovec_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_ciovec_t) == 8, "non-wasi data layout");

typedef struct __wasi_iovec_t {
    void *buf;
    size_t buf_len;
} __wasi_iovec_t;
_Static_assert(offsetof(__wasi_iovec_t, buf) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_iovec_t, buf_len) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_iovec_t, buf_len) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_iovec_t) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_iovec_t) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_iovec_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_iovec_t) == 8, "non-wasi data layout");

typedef struct __wasi_recv_in_t {
    const __wasi_iovec_t *ri_data;
    size_t ri_data_len;
    __wasi_fd_t *ri_fds;
    size_t ri_fds_len;
    __wasi_riflags_t ri_flags;
} __wasi_recv_in_t;
_Static_assert(offsetof(__wasi_recv_in_t, ri_data) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_in_t, ri_data_len) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_in_t, ri_data_len) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_in_t, ri_fds) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_in_t, ri_fds) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_in_t, ri_fds_len) == 12, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_in_t, ri_fds_len) == 24, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_in_t, ri_flags) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_in_t, ri_flags) == 32, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_recv_in_t) == 20, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_recv_in_t) == 40, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_recv_in_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_recv_in_t) == 8, "non-wasi data layout");

typedef struct __wasi_recv_out_t {
    size_t ro_datalen;
    size_t ro_fdslen;
    char ro_unused[40];
    __wasi_roflags_t ro_flags;
} __wasi_recv_out_t;
_Static_assert(
    offsetof(__wasi_recv_out_t, ro_datalen) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_out_t, ro_fdslen) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_out_t, ro_fdslen) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_out_t, ro_unused) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_out_t, ro_unused) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_recv_out_t, ro_flags) == 48, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_recv_out_t, ro_flags) == 56, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_recv_out_t) == 52, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_recv_out_t) == 64, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_recv_out_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_recv_out_t) == 8, "non-wasi data layout");

typedef struct __wasi_send_in_t {
    const __wasi_ciovec_t *si_data;
    size_t si_data_len;
    const __wasi_fd_t *si_fds;
    size_t si_fds_len;
    __wasi_siflags_t si_flags;
} __wasi_send_in_t;
_Static_assert(
    offsetof(__wasi_send_in_t, si_data) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_send_in_t, si_data_len) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_send_in_t, si_data_len) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_send_in_t, si_fds) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_send_in_t, si_fds) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_send_in_t, si_fds_len) == 12, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_send_in_t, si_fds_len) == 24, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_send_in_t, si_flags) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_send_in_t, si_flags) == 32, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_send_in_t) == 20, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_send_in_t) == 40, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_send_in_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_send_in_t) == 8, "non-wasi data layout");

typedef struct __wasi_send_out_t {
    size_t so_datalen;
} __wasi_send_out_t;
_Static_assert(
    offsetof(__wasi_send_out_t, so_datalen) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_send_out_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_send_out_t) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_send_out_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_send_out_t) == 8, "non-wasi data layout");

typedef struct __wasi_subscription_t {
    __wasi_userdata_t userdata;
    uint16_t unused;
    __wasi_eventtype_t type;
    union {
        struct {
            __wasi_userdata_t identifier;
            __wasi_clockid_t clock_id;
            __wasi_timestamp_t timeout;
            __wasi_timestamp_t precision;
            __wasi_subclockflags_t flags;
        } clock;
        struct {
            _Atomic(__wasi_condvar_t) *condvar;
            _Atomic(__wasi_lock_t) *lock;
            __wasi_scope_t condvar_scope;
            __wasi_scope_t lock_scope;
        } condvar;
        struct {
            __wasi_fd_t fd;
            __wasi_subrwflags_t flags;
        } fd_readwrite;
        struct {
            _Atomic(__wasi_lock_t) *lock;
            __wasi_scope_t lock_scope;
        } lock;
        struct {
            __wasi_fd_t fd;
        } proc_terminate;
    };
} __wasi_subscription_t;
_Static_assert(
    offsetof(__wasi_subscription_t, userdata) == 0, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, unused) == 8, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, type) == 10, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, clock.identifier) == 16,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, clock.clock_id) == 24,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, clock.timeout) == 32, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, clock.precision) == 40,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, clock.flags) == 48, "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, condvar.condvar) == 16,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_subscription_t, condvar.lock) == 20,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_subscription_t, condvar.lock) == 24,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_subscription_t, condvar.condvar_scope) == 24,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_subscription_t, condvar.condvar_scope) == 32,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_subscription_t, condvar.lock_scope) == 25,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_subscription_t, condvar.lock_scope) == 33,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, fd_readwrite.fd) == 16,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, fd_readwrite.flags) == 20,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, lock.lock) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_subscription_t, lock.lock_scope) == 20,
    "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_subscription_t, lock.lock_scope) == 24,
    "non-wasi data layout");
_Static_assert(
    offsetof(__wasi_subscription_t, proc_terminate.fd) == 16,
    "non-wasi data layout");
_Static_assert(sizeof(__wasi_subscription_t) == 56, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_subscription_t) == 8, "non-wasi data layout");

typedef struct __wasi_tcb_t {
    void *parent;
} __wasi_tcb_t;
_Static_assert(
    offsetof(__wasi_tcb_t, parent) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_tcb_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_tcb_t) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_tcb_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_tcb_t) == 8, "non-wasi data layout");

typedef void __wasi_threadentry_t(
    __wasi_tid_t tid,
    void *aux
);

typedef struct __wasi_threadattr_t {
    __wasi_threadentry_t *entry_point;
    void *stack;
    size_t stack_len;
    void *argument;
} __wasi_threadattr_t;
_Static_assert(
    offsetof(__wasi_threadattr_t, entry_point) == 0, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_threadattr_t, stack) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_threadattr_t, stack) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_threadattr_t, stack_len) == 8, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_threadattr_t, stack_len) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    offsetof(__wasi_threadattr_t, argument) == 12, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    offsetof(__wasi_threadattr_t, argument) == 24, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    sizeof(__wasi_threadattr_t) == 16, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    sizeof(__wasi_threadattr_t) == 32, "non-wasi data layout");
_Static_assert(sizeof(void *) != 4 ||
    _Alignof(__wasi_threadattr_t) == 4, "non-wasi data layout");
_Static_assert(sizeof(void *) != 8 ||
    _Alignof(__wasi_threadattr_t) == 8, "non-wasi data layout");

#ifdef NEW
#define __WASI_SYSCALL(name) \
    __asm__(#name) \
    __attribute__((__import_module__("wasi")))
#else
#define __WASI_SYSCALL(name)
#endif

__wasi_errno_t __wasi_clock_res_get(
    __wasi_clockid_t clock_id,
    __wasi_timestamp_t *resolution
) __WASI_SYSCALL(clock_res_get) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_clock_time_get(
    __wasi_clockid_t clock_id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *time
) __WASI_SYSCALL(clock_time_get) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_condvar_signal(
    _Atomic(__wasi_condvar_t) *condvar,
    __wasi_scope_t scope,
    __wasi_nthreads_t nwaiters
) __WASI_SYSCALL(condvar_signal) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
) __WASI_SYSCALL(fd_close) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_create1(
    __wasi_filetype_t type,
    __wasi_fd_t *fd
) __WASI_SYSCALL(fd_create1) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_create2(
    __wasi_filetype_t type,
    __wasi_fd_t *fd0,
    __wasi_fd_t *fd1
) __WASI_SYSCALL(fd_create2) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
) __WASI_SYSCALL(fd_datasync) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_dup(
    __wasi_fd_t from,
    __wasi_fd_t *fd
) __WASI_SYSCALL(fd_dup) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    size_t *nread
) __WASI_SYSCALL(fd_pread) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    size_t *nwritten
) __WASI_SYSCALL(fd_pwrite) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    size_t *nread
) __WASI_SYSCALL(fd_read) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_replace(
    __wasi_fd_t from,
    __wasi_fd_t to
) __WASI_SYSCALL(fd_replace) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *newoffset
) __WASI_SYSCALL(fd_seek) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_stat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *buf
) __WASI_SYSCALL(fd_stat_get) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_stat_put(
    __wasi_fd_t fd,
    const __wasi_fdstat_t *buf,
    __wasi_fdsflags_t flags
) __WASI_SYSCALL(fd_stat_put) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
) __WASI_SYSCALL(fd_sync) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    size_t *nwritten
) __WASI_SYSCALL(fd_write) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_advise(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
) __WASI_SYSCALL(file_advise) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_allocate(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
) __WASI_SYSCALL(file_allocate) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_create(
    __wasi_fd_t fd,
    const char *path,
    size_t path_len,
    __wasi_filetype_t type
) __WASI_SYSCALL(file_create) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_link(
    __wasi_lookup_t old_fd,
    const char *old_path,
    size_t old_path_len,
    __wasi_fd_t new_fd,
    const char *new_path,
    size_t new_path_len
) __WASI_SYSCALL(file_link) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_open(
    __wasi_lookup_t dirfd,
    const char *path,
    size_t path_len,
    __wasi_oflags_t oflags,
    const __wasi_fdstat_t *fds,
    __wasi_fd_t *fd
) __WASI_SYSCALL(file_open) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_readdir(
    __wasi_fd_t fd,
    void *buf,
    size_t buf_len,
    __wasi_dircookie_t cookie,
    size_t *bufused
) __WASI_SYSCALL(file_readdir) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_readlink(
    __wasi_fd_t fd,
    const char *path,
    size_t path_len,
    char *buf,
    size_t buf_len,
    size_t *bufused
) __WASI_SYSCALL(file_readlink) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_rename(
    __wasi_fd_t old_fd0,
    const char *old_path,
    size_t old_path_len,
    __wasi_fd_t new_fd,
    const char *new_path,
    size_t new_path_len
) __WASI_SYSCALL(file_rename) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_stat_fget(
    __wasi_fd_t fd,
    __wasi_filestat_t *buf
) __WASI_SYSCALL(file_stat_fget) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_stat_fput(
    __wasi_fd_t fd,
    const __wasi_filestat_t *buf,
    __wasi_fsflags_t flags
) __WASI_SYSCALL(file_stat_fput) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_stat_get(
    __wasi_lookup_t fd,
    const char *path,
    size_t path_len,
    __wasi_filestat_t *buf
) __WASI_SYSCALL(file_stat_get) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_stat_put(
    __wasi_lookup_t fd,
    const char *path,
    size_t path_len,
    const __wasi_filestat_t *buf,
    __wasi_fsflags_t flags
) __WASI_SYSCALL(file_stat_put) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_symlink(
    const char *path1,
    size_t path1_len,
    __wasi_fd_t fd,
    const char *path2,
    size_t path2_len
) __WASI_SYSCALL(file_symlink) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_file_unlink(
    __wasi_fd_t fd,
    const char *path,
    size_t path_len,
    __wasi_ulflags_t flags
) __WASI_SYSCALL(file_unlink) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_lock_unlock(
    _Atomic(__wasi_lock_t) *lock,
    __wasi_scope_t scope
) __WASI_SYSCALL(lock_unlock) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_mem_advise(
    void *mapping,
    size_t mapping_len,
    __wasi_advice_t advice
) __WASI_SYSCALL(mem_advise) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_mem_map(
    void *addr,
    size_t len,
    __wasi_mprot_t prot,
    __wasi_mflags_t flags,
    __wasi_fd_t fd,
    __wasi_filesize_t off,
    void **mem
) __WASI_SYSCALL(mem_map) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_mem_protect(
    void *mapping,
    size_t mapping_len,
    __wasi_mprot_t prot
) __WASI_SYSCALL(mem_protect) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_mem_sync(
    void *mapping,
    size_t mapping_len,
    __wasi_msflags_t flags
) __WASI_SYSCALL(mem_sync) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_mem_unmap(
    void *mapping,
    size_t mapping_len
) __WASI_SYSCALL(mem_unmap) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_poll(
    const __wasi_subscription_t *in,
    __wasi_event_t *out,
    size_t nsubscriptions,
    size_t *nevents
) __WASI_SYSCALL(poll) __attribute__((__warn_unused_result__));

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
) __WASI_SYSCALL(proc_exit);

__wasi_errno_t __wasi_proc_raise(
    __wasi_signal_t sig
) __WASI_SYSCALL(proc_raise) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_random_get(
    void *buf,
    size_t buf_len
) __WASI_SYSCALL(random_get) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t sock,
    const __wasi_recv_in_t *in,
    __wasi_recv_out_t *out
) __WASI_SYSCALL(sock_recv) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t sock,
    const __wasi_send_in_t *in,
    __wasi_send_out_t *out
) __WASI_SYSCALL(sock_send) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t sock,
    __wasi_sdflags_t how
) __WASI_SYSCALL(sock_shutdown) __attribute__((__warn_unused_result__));

__wasi_errno_t __wasi_thread_create(
    __wasi_threadattr_t *attr,
    __wasi_tid_t *tid
) __WASI_SYSCALL(thread_create) __attribute__((__warn_unused_result__));

_Noreturn void __wasi_thread_exit(
    _Atomic(__wasi_lock_t) *lock,
    __wasi_scope_t scope
) __WASI_SYSCALL(thread_exit);

__wasi_errno_t __wasi_thread_yield(void)
    __WASI_SYSCALL(thread_exit) __attribute__((__warn_unused_result__));

#ifdef __cplusplus
}  // extern "C"
#endif

#endif
