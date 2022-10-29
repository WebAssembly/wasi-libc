/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   wasix_v1.witx
 *
 * To regenerate this file execute:
 *
 *     cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
 */

#ifndef __wasi__
#define __wasi__ 1
#endif

#ifndef __wasi_api_h
#define __wasi_api_h

#include <stddef.h>
#include <stdint.h>

_Static_assert(_Alignof(int8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(uint8_t) == 1, "non-wasi data layout");
_Static_assert(_Alignof(int16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(uint16_t) == 2, "non-wasi data layout");
_Static_assert(_Alignof(int32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(uint32_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(int64_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(uint64_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(intptr_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(uintptr_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(void*) == 8, "non-wasi data layout");
typedef int64_t __wasi_int_t;
typedef uint64_t __wasi_uint_t;
_Static_assert(_Alignof(__wasi_int_t) == 8, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_uint_t) == 8, "non-wasi data layout");
#ifdef __cplusplus
extern "C" {
#endif

// TODO: Encoding this in witx.
#define __WASI_DIRCOOKIE_START (UINT64_C(0))
/**
 * Represents the length of a piece of data referenced by a pointer
 */
typedef __wasi_uint_t __wasi_pointersize_t;

_Static_assert(sizeof(__wasi_pointersize_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_pointersize_t) == 8, "witx calculated align");

/**
 * Represents a number of items
 */
typedef uint32_t __wasi_size_t;

_Static_assert(sizeof(__wasi_size_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_size_t) == 4, "witx calculated align");

/**
 * Represents a large number of items
 */
typedef uint64_t __wasi_longsize_t;

_Static_assert(sizeof(__wasi_longsize_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_longsize_t) == 8, "witx calculated align");

/**
 * Non-negative file size or length of a region within a file.
 */
typedef uint64_t __wasi_filesize_t;

_Static_assert(sizeof(__wasi_filesize_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_filesize_t) == 8, "witx calculated align");

/**
 * Timestamp in nanoseconds.
 */
typedef uint64_t __wasi_timestamp_t;

_Static_assert(sizeof(__wasi_timestamp_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_timestamp_t) == 8, "witx calculated align");

/**
 * Key for a thread local variable.
 */
typedef uint32_t __wasi_tl_key_t;

_Static_assert(sizeof(__wasi_tl_key_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_tl_key_t) == 4, "witx calculated align");

/**
 * Value of a thread local variable.
 */
typedef uint64_t __wasi_tl_val_t;

_Static_assert(sizeof(__wasi_tl_val_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_tl_val_t) == 8, "witx calculated align");

/**
 * Value of a short hash
 */
typedef uint64_t __wasi_short_hash_t;

_Static_assert(sizeof(__wasi_short_hash_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_short_hash_t) == 8, "witx calculated align");

/**
 * Represents a 128bit number
 */
typedef struct __wasi_hugesize_t {
    /**
     * First set of 64 bits
     */
    uint64_t b0;

    /**
     * second set of 64 bits
     */
    uint64_t b1;

} __wasi_hugesize_t;

_Static_assert(sizeof(__wasi_hugesize_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_hugesize_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_hugesize_t, b0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_hugesize_t, b1) == 8, "witx calculated offset");

/**
 * Represents a part of the stack (128 bytes)
 */
typedef struct __wasi_stack_part_t {
    __wasi_hugesize_t a1;

    __wasi_hugesize_t a2;

    __wasi_hugesize_t a3;

    __wasi_hugesize_t a4;

    __wasi_hugesize_t a5;

    __wasi_hugesize_t a6;

    __wasi_hugesize_t a7;

    __wasi_hugesize_t a8;

} __wasi_stack_part_t;

_Static_assert(sizeof(__wasi_stack_part_t) == 128, "witx calculated size");
_Static_assert(_Alignof(__wasi_stack_part_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_stack_part_t, a1) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a2) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a3) == 32, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a4) == 48, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a5) == 64, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a6) == 80, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a7) == 96, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_part_t, a8) == 112, "witx calculated offset");

/**
 * Represents the first 128 bits of a SHA256 hash
 */
typedef struct __wasi_hash_t {
    /**
     * First set of 64 bits
     */
    uint64_t b0;

    /**
     * second set of 64 bits
     */
    uint64_t b1;

} __wasi_hash_t;

_Static_assert(sizeof(__wasi_hash_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_hash_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_hash_t, b0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_hash_t, b1) == 8, "witx calculated offset");

/**
 * Represents a snapshot of a point in time of the stack that can be restored later
 */
typedef struct __wasi_stack_snapshot_t {
    /**
     * User defined field that can be used by functions
     */
    uint64_t user;

    /**
     * Hash used to identify which stack snapshot to restore
     */
    __wasi_hash_t hash;

} __wasi_stack_snapshot_t;

_Static_assert(sizeof(__wasi_stack_snapshot_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_stack_snapshot_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_stack_snapshot_t, user) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_stack_snapshot_t, hash) == 8, "witx calculated offset");

/**
 * Identifiers for clocks.
 */
typedef uint32_t __wasi_clockid_t;

/**
 * The clock measuring real time. Time value zero corresponds with
 * 1970-01-01T00:00:00Z.
 */
#define __WASI_CLOCKID_REALTIME (UINT32_C(0))

/**
 * The store-wide monotonic clock, which is defined as a clock measuring
 * real time, whose value cannot be adjusted and which cannot have negative
 * clock jumps. The epoch of this clock is undefined. The absolute time
 * value of this clock therefore has no meaning.
 */
#define __WASI_CLOCKID_MONOTONIC (UINT32_C(1))

/**
 * The CPU-time clock associated with the current process.
 */
#define __WASI_CLOCKID_PROCESS_CPUTIME_ID (UINT32_C(2))

/**
 * The CPU-time clock associated with the current thread.
 */
#define __WASI_CLOCKID_THREAD_CPUTIME_ID (UINT32_C(3))

_Static_assert(sizeof(__wasi_clockid_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_clockid_t) == 4, "witx calculated align");

/**
 * Error codes returned by functions.
 * Not all of these error codes are returned by the functions provided by this
 * API; some are used in higher-level library layers, and others are provided
 * merely for alignment with POSIX.
 */
typedef uint16_t __wasi_errno_t;

/**
 * No error occurred. System call completed successfully.
 */
#define __WASI_ERRNO_SUCCESS (UINT16_C(0))

/**
 * Argument list too long.
 */
#define __WASI_ERRNO_2BIG (UINT16_C(1))

/**
 * Permission denied.
 */
#define __WASI_ERRNO_ACCES (UINT16_C(2))

/**
 * Address in use.
 */
#define __WASI_ERRNO_ADDRINUSE (UINT16_C(3))

/**
 * Address not available.
 */
#define __WASI_ERRNO_ADDRNOTAVAIL (UINT16_C(4))

/**
 * Address family not supported.
 */
#define __WASI_ERRNO_AFNOSUPPORT (UINT16_C(5))

/**
 * Resource unavailable, or operation would block.
 */
#define __WASI_ERRNO_AGAIN (UINT16_C(6))

/**
 * Connection already in progress.
 */
#define __WASI_ERRNO_ALREADY (UINT16_C(7))

/**
 * Bad file descriptor.
 */
#define __WASI_ERRNO_BADF (UINT16_C(8))

/**
 * Bad message.
 */
#define __WASI_ERRNO_BADMSG (UINT16_C(9))

/**
 * Device or resource busy.
 */
#define __WASI_ERRNO_BUSY (UINT16_C(10))

/**
 * Operation canceled.
 */
#define __WASI_ERRNO_CANCELED (UINT16_C(11))

/**
 * No child processes.
 */
#define __WASI_ERRNO_CHILD (UINT16_C(12))

/**
 * Connection aborted.
 */
#define __WASI_ERRNO_CONNABORTED (UINT16_C(13))

/**
 * Connection refused.
 */
#define __WASI_ERRNO_CONNREFUSED (UINT16_C(14))

/**
 * Connection reset.
 */
#define __WASI_ERRNO_CONNRESET (UINT16_C(15))

/**
 * Resource deadlock would occur.
 */
#define __WASI_ERRNO_DEADLK (UINT16_C(16))

/**
 * Destination address required.
 */
#define __WASI_ERRNO_DESTADDRREQ (UINT16_C(17))

/**
 * Mathematics argument out of domain of function.
 */
#define __WASI_ERRNO_DOM (UINT16_C(18))

/**
 * Reserved.
 */
#define __WASI_ERRNO_DQUOT (UINT16_C(19))

/**
 * File exists.
 */
#define __WASI_ERRNO_EXIST (UINT16_C(20))

/**
 * Bad address.
 */
#define __WASI_ERRNO_FAULT (UINT16_C(21))

/**
 * File too large.
 */
#define __WASI_ERRNO_FBIG (UINT16_C(22))

/**
 * Host is unreachable.
 */
#define __WASI_ERRNO_HOSTUNREACH (UINT16_C(23))

/**
 * Identifier removed.
 */
#define __WASI_ERRNO_IDRM (UINT16_C(24))

/**
 * Illegal byte sequence.
 */
#define __WASI_ERRNO_ILSEQ (UINT16_C(25))

/**
 * Operation in progress.
 */
#define __WASI_ERRNO_INPROGRESS (UINT16_C(26))

/**
 * Interrupted function.
 */
#define __WASI_ERRNO_INTR (UINT16_C(27))

/**
 * Invalid argument.
 */
#define __WASI_ERRNO_INVAL (UINT16_C(28))

/**
 * I/O error.
 */
#define __WASI_ERRNO_IO (UINT16_C(29))

/**
 * Socket is connected.
 */
#define __WASI_ERRNO_ISCONN (UINT16_C(30))

/**
 * Is a directory.
 */
#define __WASI_ERRNO_ISDIR (UINT16_C(31))

/**
 * Too many levels of symbolic links.
 */
#define __WASI_ERRNO_LOOP (UINT16_C(32))

/**
 * File descriptor value too large.
 */
#define __WASI_ERRNO_MFILE (UINT16_C(33))

/**
 * Too many links.
 */
#define __WASI_ERRNO_MLINK (UINT16_C(34))

/**
 * Message too large.
 */
#define __WASI_ERRNO_MSGSIZE (UINT16_C(35))

/**
 * Reserved.
 */
#define __WASI_ERRNO_MULTIHOP (UINT16_C(36))

/**
 * Filename too long.
 */
#define __WASI_ERRNO_NAMETOOLONG (UINT16_C(37))

/**
 * Network is down.
 */
#define __WASI_ERRNO_NETDOWN (UINT16_C(38))

/**
 * Connection aborted by network.
 */
#define __WASI_ERRNO_NETRESET (UINT16_C(39))

/**
 * Network unreachable.
 */
#define __WASI_ERRNO_NETUNREACH (UINT16_C(40))

/**
 * Too many files open in system.
 */
#define __WASI_ERRNO_NFILE (UINT16_C(41))

/**
 * No buffer space available.
 */
#define __WASI_ERRNO_NOBUFS (UINT16_C(42))

/**
 * No such device.
 */
#define __WASI_ERRNO_NODEV (UINT16_C(43))

/**
 * No such file or directory.
 */
#define __WASI_ERRNO_NOENT (UINT16_C(44))

/**
 * Executable file format error.
 */
#define __WASI_ERRNO_NOEXEC (UINT16_C(45))

/**
 * No locks available.
 */
#define __WASI_ERRNO_NOLCK (UINT16_C(46))

/**
 * Reserved.
 */
#define __WASI_ERRNO_NOLINK (UINT16_C(47))

/**
 * Not enough space.
 */
#define __WASI_ERRNO_NOMEM (UINT16_C(48))

/**
 * No message of the desired type.
 */
#define __WASI_ERRNO_NOMSG (UINT16_C(49))

/**
 * Protocol not available.
 */
#define __WASI_ERRNO_NOPROTOOPT (UINT16_C(50))

/**
 * No space left on device.
 */
#define __WASI_ERRNO_NOSPC (UINT16_C(51))

/**
 * Function not supported.
 */
#define __WASI_ERRNO_NOSYS (UINT16_C(52))

/**
 * The socket is not connected.
 */
#define __WASI_ERRNO_NOTCONN (UINT16_C(53))

/**
 * Not a directory or a symbolic link to a directory.
 */
#define __WASI_ERRNO_NOTDIR (UINT16_C(54))

/**
 * Directory not empty.
 */
#define __WASI_ERRNO_NOTEMPTY (UINT16_C(55))

/**
 * State not recoverable.
 */
#define __WASI_ERRNO_NOTRECOVERABLE (UINT16_C(56))

/**
 * Not a socket.
 */
#define __WASI_ERRNO_NOTSOCK (UINT16_C(57))

/**
 * Not supported, or operation not supported on socket.
 */
#define __WASI_ERRNO_NOTSUP (UINT16_C(58))

/**
 * Inappropriate I/O control operation.
 */
#define __WASI_ERRNO_NOTTY (UINT16_C(59))

/**
 * No such device or address.
 */
#define __WASI_ERRNO_NXIO (UINT16_C(60))

/**
 * Value too large to be stored in data type.
 */
#define __WASI_ERRNO_OVERFLOW (UINT16_C(61))

/**
 * Previous owner died.
 */
#define __WASI_ERRNO_OWNERDEAD (UINT16_C(62))

/**
 * Operation not permitted.
 */
#define __WASI_ERRNO_PERM (UINT16_C(63))

/**
 * Broken pipe.
 */
#define __WASI_ERRNO_PIPE (UINT16_C(64))

/**
 * Protocol error.
 */
#define __WASI_ERRNO_PROTO (UINT16_C(65))

/**
 * Protocol not supported.
 */
#define __WASI_ERRNO_PROTONOSUPPORT (UINT16_C(66))

/**
 * Protocol wrong type for socket.
 */
#define __WASI_ERRNO_PROTOTYPE (UINT16_C(67))

/**
 * Result too large.
 */
#define __WASI_ERRNO_RANGE (UINT16_C(68))

/**
 * Read-only file system.
 */
#define __WASI_ERRNO_ROFS (UINT16_C(69))

/**
 * Invalid seek.
 */
#define __WASI_ERRNO_SPIPE (UINT16_C(70))

/**
 * No such process.
 */
#define __WASI_ERRNO_SRCH (UINT16_C(71))

/**
 * Reserved.
 */
#define __WASI_ERRNO_STALE (UINT16_C(72))

/**
 * Connection timed out.
 */
#define __WASI_ERRNO_TIMEDOUT (UINT16_C(73))

/**
 * Text file busy.
 */
#define __WASI_ERRNO_TXTBSY (UINT16_C(74))

/**
 * Cross-device link.
 */
#define __WASI_ERRNO_XDEV (UINT16_C(75))

/**
 * Extension: Capabilities insufficient.
 */
#define __WASI_ERRNO_NOTCAPABLE (UINT16_C(76))

/**
 * Cannot send after socket shutdown.
 */
#define __WASI_ERRNO_SHUTDOWN (UINT16_C(77))

_Static_assert(sizeof(__wasi_errno_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_errno_t) == 2, "witx calculated align");

/**
 * File descriptor rights, determining which actions may be performed.
 */
typedef uint64_t __wasi_rights_t;

/**
 * The right to invoke `fd_datasync`.
 * If `path_open` is set, includes the right to invoke
 * `path_open` with `fdflags::dsync`.
 */
#define __WASI_RIGHTS_FD_DATASYNC ((__wasi_rights_t)(1 << 0))

/**
 * The right to invoke `fd_read` and `sock_recv`.
 * If `rights::fd_seek` is set, includes the right to invoke `fd_pread`.
 */
#define __WASI_RIGHTS_FD_READ ((__wasi_rights_t)(1 << 1))

/**
 * The right to invoke `fd_seek`. This flag implies `rights::fd_tell`.
 */
#define __WASI_RIGHTS_FD_SEEK ((__wasi_rights_t)(1 << 2))

/**
 * The right to invoke `fd_fdstat_set_flags`.
 */
#define __WASI_RIGHTS_FD_FDSTAT_SET_FLAGS ((__wasi_rights_t)(1 << 3))

/**
 * The right to invoke `fd_sync`.
 * If `path_open` is set, includes the right to invoke
 * `path_open` with `fdflags::rsync` and `fdflags::dsync`.
 */
#define __WASI_RIGHTS_FD_SYNC ((__wasi_rights_t)(1 << 4))

/**
 * The right to invoke `fd_seek` in such a way that the file offset
 * remains unaltered (i.e., `whence::cur` with offset zero), or to
 * invoke `fd_tell`.
 */
#define __WASI_RIGHTS_FD_TELL ((__wasi_rights_t)(1 << 5))

/**
 * The right to invoke `fd_write` and `sock_send`.
 * If `rights::fd_seek` is set, includes the right to invoke `fd_pwrite`.
 */
#define __WASI_RIGHTS_FD_WRITE ((__wasi_rights_t)(1 << 6))

/**
 * The right to invoke `fd_advise`.
 */
#define __WASI_RIGHTS_FD_ADVISE ((__wasi_rights_t)(1 << 7))

/**
 * The right to invoke `fd_allocate`.
 */
#define __WASI_RIGHTS_FD_ALLOCATE ((__wasi_rights_t)(1 << 8))

/**
 * The right to invoke `path_create_directory`.
 */
#define __WASI_RIGHTS_PATH_CREATE_DIRECTORY ((__wasi_rights_t)(1 << 9))

/**
 * If `path_open` is set, the right to invoke `path_open` with `oflags::creat`.
 */
#define __WASI_RIGHTS_PATH_CREATE_FILE ((__wasi_rights_t)(1 << 10))

/**
 * The right to invoke `path_link` with the file descriptor as the
 * source directory.
 */
#define __WASI_RIGHTS_PATH_LINK_SOURCE ((__wasi_rights_t)(1 << 11))

/**
 * The right to invoke `path_link` with the file descriptor as the
 * target directory.
 */
#define __WASI_RIGHTS_PATH_LINK_TARGET ((__wasi_rights_t)(1 << 12))

/**
 * The right to invoke `path_open`.
 */
#define __WASI_RIGHTS_PATH_OPEN ((__wasi_rights_t)(1 << 13))

/**
 * The right to invoke `fd_readdir`.
 */
#define __WASI_RIGHTS_FD_READDIR ((__wasi_rights_t)(1 << 14))

/**
 * The right to invoke `path_readlink`.
 */
#define __WASI_RIGHTS_PATH_READLINK ((__wasi_rights_t)(1 << 15))

/**
 * The right to invoke `path_rename` with the file descriptor as the source directory.
 */
#define __WASI_RIGHTS_PATH_RENAME_SOURCE ((__wasi_rights_t)(1 << 16))

/**
 * The right to invoke `path_rename` with the file descriptor as the target directory.
 */
#define __WASI_RIGHTS_PATH_RENAME_TARGET ((__wasi_rights_t)(1 << 17))

/**
 * The right to invoke `path_filestat_get`.
 */
#define __WASI_RIGHTS_PATH_FILESTAT_GET ((__wasi_rights_t)(1 << 18))

/**
 * The right to change a file's size (there is no `path_filestat_set_size`).
 * If `path_open` is set, includes the right to invoke `path_open` with `oflags::trunc`.
 */
#define __WASI_RIGHTS_PATH_FILESTAT_SET_SIZE ((__wasi_rights_t)(1 << 19))

/**
 * The right to invoke `path_filestat_set_times`.
 */
#define __WASI_RIGHTS_PATH_FILESTAT_SET_TIMES ((__wasi_rights_t)(1 << 20))

/**
 * The right to invoke `fd_filestat_get`.
 */
#define __WASI_RIGHTS_FD_FILESTAT_GET ((__wasi_rights_t)(1 << 21))

/**
 * The right to invoke `fd_filestat_set_size`.
 */
#define __WASI_RIGHTS_FD_FILESTAT_SET_SIZE ((__wasi_rights_t)(1 << 22))

/**
 * The right to invoke `fd_filestat_set_times`.
 */
#define __WASI_RIGHTS_FD_FILESTAT_SET_TIMES ((__wasi_rights_t)(1 << 23))

/**
 * The right to invoke `path_symlink`.
 */
#define __WASI_RIGHTS_PATH_SYMLINK ((__wasi_rights_t)(1 << 24))

/**
 * The right to invoke `path_remove_directory`.
 */
#define __WASI_RIGHTS_PATH_REMOVE_DIRECTORY ((__wasi_rights_t)(1 << 25))

/**
 * The right to invoke `path_unlink_file`.
 */
#define __WASI_RIGHTS_PATH_UNLINK_FILE ((__wasi_rights_t)(1 << 26))

/**
 * If `rights::fd_read` is set, includes the right to invoke `poll_oneoff` to subscribe to `eventtype::fd_read`.
 * If `rights::fd_write` is set, includes the right to invoke `poll_oneoff` to subscribe to `eventtype::fd_write`.
 */
#define __WASI_RIGHTS_POLL_FD_READWRITE ((__wasi_rights_t)(1 << 27))

/**
 * The right to invoke `sock_shutdown`.
 */
#define __WASI_RIGHTS_SOCK_SHUTDOWN ((__wasi_rights_t)(1 << 28))

/**
 * Accept incoming connection
 */
#define __WASI_RIGHTS_SOCK_ACCEPT ((__wasi_rights_t)(1 << 29))

/**
 * Connect to an address
 */
#define __WASI_RIGHTS_SOCK_CONNECT ((__wasi_rights_t)(1 << 30))

/**
 * Listen for incoming connection on an address
 */
#define __WASI_RIGHTS_SOCK_LISTEN ((__wasi_rights_t)(1 << 31))

/**
 * Bind an address to a socket
 */
#define __WASI_RIGHTS_SOCK_BIND ((__wasi_rights_t)(1 << 32))

/**
 * Receive data on a socket
 */
#define __WASI_RIGHTS_SOCK_RECV ((__wasi_rights_t)(1 << 33))

/**
 * Send data on a socket
 */
#define __WASI_RIGHTS_SOCK_SEND ((__wasi_rights_t)(1 << 34))

/**
 * Retrieve locally bound address on a socket
 */
#define __WASI_RIGHTS_SOCK_ADDR_LOCAL ((__wasi_rights_t)(1 << 35))

/**
 * Retrieve remote address on a socket
 */
#define __WASI_RIGHTS_SOCK_ADDR_REMOTE ((__wasi_rights_t)(1 << 36))

/**
 * Receive datagram on a socket
 */
#define __WASI_RIGHTS_SOCK_RECV_FROM ((__wasi_rights_t)(1 << 37))

/**
 * Send datagram on a socket
 */
#define __WASI_RIGHTS_SOCK_SEND_TO ((__wasi_rights_t)(1 << 38))

/**
 * Option type
 */
typedef uint8_t __wasi_option_t;

#define __WASI_OPTION_NONE (UINT8_C(0))

#define __WASI_OPTION_SOME (UINT8_C(1))

_Static_assert(sizeof(__wasi_option_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_t) == 1, "witx calculated align");

/**
 * Represents an optional timestamp
 */
typedef union __wasi_option_timestamp_u_t {
    uint8_t none;
    __wasi_timestamp_t some;
} __wasi_option_timestamp_u_t;
typedef struct __wasi_option_timestamp_t {
    uint8_t tag;
    __wasi_option_timestamp_u_t u;
} __wasi_option_timestamp_t;

_Static_assert(sizeof(__wasi_option_timestamp_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_timestamp_t) == 8, "witx calculated align");

/**
 * Represents an optional hash
 */
typedef union __wasi_option_hash_u_t {
    uint8_t none;
    __wasi_hash_t some;
} __wasi_option_hash_u_t;
typedef struct __wasi_option_hash_t {
    uint8_t tag;
    __wasi_option_hash_u_t u;
} __wasi_option_hash_t;

_Static_assert(sizeof(__wasi_option_hash_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_hash_t) == 8, "witx calculated align");

/**
 * A file descriptor handle.
 */
typedef int __wasi_fd_t;

_Static_assert(sizeof(__wasi_fd_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_fd_t) == 4, "witx calculated align");

/**
 * A process descriptor handle.
 */
typedef int __wasi_pid_t;

_Static_assert(sizeof(__wasi_pid_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_pid_t) == 4, "witx calculated align");

/**
 * A thread handle
 */
typedef int __wasi_tid_t;

_Static_assert(sizeof(__wasi_tid_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_tid_t) == 4, "witx calculated align");

/**
 * A bus process handle
 */
typedef int __wasi_bid_t;

_Static_assert(sizeof(__wasi_bid_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_bid_t) == 4, "witx calculated align");

/**
 * Represents an optional bus process
 */
typedef union __wasi_option_bid_u_t {
    uint8_t none;
    __wasi_bid_t some;
} __wasi_option_bid_u_t;
typedef struct __wasi_option_bid_t {
    uint8_t tag;
    __wasi_option_bid_u_t u;
} __wasi_option_bid_t;

_Static_assert(sizeof(__wasi_option_bid_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_bid_t) == 4, "witx calculated align");

/**
 * A bus call handle
 */
typedef uint64_t __wasi_cid_t;

_Static_assert(sizeof(__wasi_cid_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_cid_t) == 8, "witx calculated align");

/**
 * Represents an optional call handle
 */
typedef union __wasi_option_cid_u_t {
    uint8_t none;
    __wasi_cid_t some;
} __wasi_option_cid_u_t;
typedef struct __wasi_option_cid_t {
    uint8_t tag;
    __wasi_option_cid_u_t u;
} __wasi_option_cid_t;

_Static_assert(sizeof(__wasi_option_cid_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_cid_t) == 8, "witx calculated align");

/**
 * Represents an optional file descriptior
 */
typedef union __wasi_option_fd_u_t {
    uint8_t none;
    __wasi_fd_t some;
} __wasi_option_fd_u_t;
typedef struct __wasi_option_fd_t {
    uint8_t tag;
    __wasi_option_fd_u_t u;
} __wasi_option_fd_t;

_Static_assert(sizeof(__wasi_option_fd_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_fd_t) == 4, "witx calculated align");

/**
 * Bool type
 */
typedef uint8_t __wasi_bool_t;

#define __WASI_BOOL_FALSE (UINT8_C(0))

#define __WASI_BOOL_TRUE (UINT8_C(1))

_Static_assert(sizeof(__wasi_bool_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_bool_t) == 1, "witx calculated align");

/**
 * A region of memory for scatter/gather reads.
 */
typedef struct __wasi_iovec_t {
    /**
     * The address of the buffer to be filled.
     */
    uint8_t * buf;

    /**
     * The length of the buffer to be filled.
     */
    __wasi_pointersize_t buf_len;

} __wasi_iovec_t;

_Static_assert(sizeof(__wasi_iovec_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_iovec_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_iovec_t, buf) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_iovec_t, buf_len) == 8, "witx calculated offset");

/**
 * A region of memory for scatter/gather writes.
 */
typedef struct __wasi_ciovec_t {
    /**
     * The address of the buffer to be written.
     */
    const uint8_t * buf;

    /**
     * The length of the buffer to be written.
     */
    __wasi_pointersize_t buf_len;

} __wasi_ciovec_t;

_Static_assert(sizeof(__wasi_ciovec_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_ciovec_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_ciovec_t, buf) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_ciovec_t, buf_len) == 8, "witx calculated offset");

/**
 * Relative offset within a file.
 */
typedef int64_t __wasi_filedelta_t;

_Static_assert(sizeof(__wasi_filedelta_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_filedelta_t) == 8, "witx calculated align");

/**
 * The position relative to which to set the offset of the file descriptor.
 */
typedef uint8_t __wasi_whence_t;

/**
 * Seek relative to start-of-file.
 */
#define __WASI_WHENCE_SET (UINT8_C(0))

/**
 * Seek relative to current position.
 */
#define __WASI_WHENCE_CUR (UINT8_C(1))

/**
 * Seek relative to end-of-file.
 */
#define __WASI_WHENCE_END (UINT8_C(2))

_Static_assert(sizeof(__wasi_whence_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_whence_t) == 1, "witx calculated align");

/**
 * A reference to the offset of a directory entry.
 * 
 * The value 0 signifies the start of the directory.
 */
typedef uint64_t __wasi_dircookie_t;

_Static_assert(sizeof(__wasi_dircookie_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_dircookie_t) == 8, "witx calculated align");

/**
 * The type for the `dirent::d_namlen` field of `dirent` struct.
 */
typedef uint32_t __wasi_dirnamlen_t;

_Static_assert(sizeof(__wasi_dirnamlen_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_dirnamlen_t) == 4, "witx calculated align");

/**
 * File serial number that is unique within its file system.
 */
typedef uint64_t __wasi_inode_t;

_Static_assert(sizeof(__wasi_inode_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_inode_t) == 8, "witx calculated align");

/**
 * The type of a file descriptor or file.
 */
typedef uint8_t __wasi_filetype_t;

/**
 * The type of the file descriptor or file is unknown or is different from any of the other types specified.
 */
#define __WASI_FILETYPE_UNKNOWN (UINT8_C(0))

/**
 * The file descriptor or file refers to a block device inode.
 */
#define __WASI_FILETYPE_BLOCK_DEVICE (UINT8_C(1))

/**
 * The file descriptor or file refers to a character device inode.
 */
#define __WASI_FILETYPE_CHARACTER_DEVICE (UINT8_C(2))

/**
 * The file descriptor or file refers to a directory inode.
 */
#define __WASI_FILETYPE_DIRECTORY (UINT8_C(3))

/**
 * The file descriptor or file refers to a regular file inode.
 */
#define __WASI_FILETYPE_REGULAR_FILE (UINT8_C(4))

/**
 * The file descriptor or file refers to a datagram socket.
 */
#define __WASI_FILETYPE_SOCKET_DGRAM (UINT8_C(5))

/**
 * The file descriptor or file refers to a byte-stream socket.
 */
#define __WASI_FILETYPE_SOCKET_STREAM (UINT8_C(6))

/**
 * The file refers to a symbolic link inode.
 */
#define __WASI_FILETYPE_SYMBOLIC_LINK (UINT8_C(7))

/**
 * The file descriptor or file refers to a raw socket.
 */
#define __WASI_FILETYPE_SOCKET_RAW (UINT8_C(8))

/**
 * The file descriptor or file refers to a sequential packet socket.
 */
#define __WASI_FILETYPE_SOCKET_SEQPACKET (UINT8_C(9))

_Static_assert(sizeof(__wasi_filetype_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_filetype_t) == 1, "witx calculated align");

/**
 * A directory entry.
 */
typedef struct __wasi_dirent_t {
    /**
     * The offset of the next directory entry stored in this directory.
     */
    __wasi_dircookie_t d_next;

    /**
     * The serial number of the file referred to by this directory entry.
     */
    __wasi_inode_t d_ino;

    /**
     * The length of the name of the directory entry.
     */
    __wasi_dirnamlen_t d_namlen;

    /**
     * The type of the file referred to by this directory entry.
     */
    __wasi_filetype_t d_type;

} __wasi_dirent_t;

_Static_assert(sizeof(__wasi_dirent_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_dirent_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_dirent_t, d_next) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_ino) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_namlen) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_type) == 20, "witx calculated offset");

/**
 * File or memory access pattern advisory information.
 */
typedef uint8_t __wasi_advice_t;

/**
 * The application has no advice to give on its behavior with respect to the specified data.
 */
#define __WASI_ADVICE_NORMAL (UINT8_C(0))

/**
 * The application expects to access the specified data sequentially from lower offsets to higher offsets.
 */
#define __WASI_ADVICE_SEQUENTIAL (UINT8_C(1))

/**
 * The application expects to access the specified data in a random order.
 */
#define __WASI_ADVICE_RANDOM (UINT8_C(2))

/**
 * The application expects to access the specified data in the near future.
 */
#define __WASI_ADVICE_WILLNEED (UINT8_C(3))

/**
 * The application expects that it will not access the specified data in the near future.
 */
#define __WASI_ADVICE_DONTNEED (UINT8_C(4))

/**
 * The application expects to access the specified data once and then not reuse it thereafter.
 */
#define __WASI_ADVICE_NOREUSE (UINT8_C(5))

_Static_assert(sizeof(__wasi_advice_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_advice_t) == 1, "witx calculated align");

/**
 * File descriptor flags.
 */
typedef uint16_t __wasi_fdflags_t;

/**
 * Append mode: Data written to the file is always appended to the file's end.
 */
#define __WASI_FDFLAGS_APPEND ((__wasi_fdflags_t)(1 << 0))

/**
 * Write according to synchronized I/O data integrity completion. Only the data stored in the file is synchronized.
 */
#define __WASI_FDFLAGS_DSYNC ((__wasi_fdflags_t)(1 << 1))

/**
 * Non-blocking mode.
 */
#define __WASI_FDFLAGS_NONBLOCK ((__wasi_fdflags_t)(1 << 2))

/**
 * Synchronized read I/O operations.
 */
#define __WASI_FDFLAGS_RSYNC ((__wasi_fdflags_t)(1 << 3))

/**
 * Write according to synchronized I/O file integrity completion. In
 * addition to synchronizing the data stored in the file, the implementation
 * may also synchronously update the file's metadata.
 */
#define __WASI_FDFLAGS_SYNC ((__wasi_fdflags_t)(1 << 4))

/**
 * File descriptor attributes.
 */
typedef struct __wasi_fdstat_t {
    /**
     * File type.
     */
    __wasi_filetype_t fs_filetype;

    /**
     * File descriptor flags.
     */
    __wasi_fdflags_t fs_flags;

    /**
     * Rights that apply to this file descriptor.
     */
    __wasi_rights_t fs_rights_base;

    /**
     * Maximum set of rights that may be installed on new file descriptors that
     * are created through this file descriptor, e.g., through `path_open`.
     */
    __wasi_rights_t fs_rights_inheriting;

} __wasi_fdstat_t;

_Static_assert(sizeof(__wasi_fdstat_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_fdstat_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_fdstat_t, fs_filetype) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_flags) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_rights_base) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_rights_inheriting) == 16, "witx calculated offset");

/**
 * Identifier for a device containing a file system. Can be used in combination
 * with `inode` to uniquely identify a file or directory in the filesystem.
 */
typedef uint64_t __wasi_device_t;

_Static_assert(sizeof(__wasi_device_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_device_t) == 8, "witx calculated align");

/**
 * Which file time attributes to adjust.
 */
typedef uint16_t __wasi_fstflags_t;

/**
 * Adjust the last data access timestamp to the value stored in `filestat::atim`.
 */
#define __WASI_FSTFLAGS_ATIM ((__wasi_fstflags_t)(1 << 0))

/**
 * Adjust the last data access timestamp to the time of clock `clockid::realtime`.
 */
#define __WASI_FSTFLAGS_ATIM_NOW ((__wasi_fstflags_t)(1 << 1))

/**
 * Adjust the last data modification timestamp to the value stored in `filestat::mtim`.
 */
#define __WASI_FSTFLAGS_MTIM ((__wasi_fstflags_t)(1 << 2))

/**
 * Adjust the last data modification timestamp to the time of clock `clockid::realtime`.
 */
#define __WASI_FSTFLAGS_MTIM_NOW ((__wasi_fstflags_t)(1 << 3))

/**
 * Flags determining the method of how paths are resolved.
 */
typedef uint32_t __wasi_lookupflags_t;

/**
 * As long as the resolved path corresponds to a symbolic link, it is expanded.
 */
#define __WASI_LOOKUPFLAGS_SYMLINK_FOLLOW ((__wasi_lookupflags_t)(1 << 0))

/**
 * Open flags used by `path_open`.
 */
typedef uint16_t __wasi_oflags_t;

/**
 * Create file if it does not exist.
 */
#define __WASI_OFLAGS_CREAT ((__wasi_oflags_t)(1 << 0))

/**
 * Fail if not a directory.
 */
#define __WASI_OFLAGS_DIRECTORY ((__wasi_oflags_t)(1 << 1))

/**
 * Fail if file already exists.
 */
#define __WASI_OFLAGS_EXCL ((__wasi_oflags_t)(1 << 2))

/**
 * Truncate file to size 0.
 */
#define __WASI_OFLAGS_TRUNC ((__wasi_oflags_t)(1 << 3))

/**
 * Number of hard links to an inode.
 */
typedef uint64_t __wasi_linkcount_t;

_Static_assert(sizeof(__wasi_linkcount_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_linkcount_t) == 8, "witx calculated align");

/**
 * File attributes.
 */
typedef struct __wasi_filestat_t {
    /**
     * Device ID of device containing the file.
     */
    __wasi_device_t dev;

    /**
     * File serial number.
     */
    __wasi_inode_t ino;

    /**
     * File type.
     */
    __wasi_filetype_t filetype;

    /**
     * Number of hard links to the file.
     */
    __wasi_linkcount_t nlink;

    /**
     * For regular files, the file size in bytes. For symbolic links, the length in bytes of the pathname contained in the symbolic link.
     */
    __wasi_filesize_t size;

    /**
     * Last data access timestamp.
     */
    __wasi_timestamp_t atim;

    /**
     * Last data modification timestamp.
     */
    __wasi_timestamp_t mtim;

    /**
     * Last file status change timestamp.
     */
    __wasi_timestamp_t ctim;

} __wasi_filestat_t;

_Static_assert(sizeof(__wasi_filestat_t) == 64, "witx calculated size");
_Static_assert(_Alignof(__wasi_filestat_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_filestat_t, dev) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, ino) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, filetype) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, nlink) == 24, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, size) == 32, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, atim) == 40, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, mtim) == 48, "witx calculated offset");
_Static_assert(offsetof(__wasi_filestat_t, ctim) == 56, "witx calculated offset");

/**
 * User-provided value that may be attached to objects that is retained when
 * extracted from the implementation.
 */
typedef uint64_t __wasi_userdata_t;

_Static_assert(sizeof(__wasi_userdata_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_userdata_t) == 8, "witx calculated align");

/**
 * Type of a subscription to an event or its occurrence.
 */
typedef uint8_t __wasi_eventtype_t;

/**
 * The time value of clock `subscription_clock::id` has
 * reached timestamp `subscription_clock::timeout`.
 */
#define __WASI_EVENTTYPE_CLOCK (UINT8_C(0))

/**
 * File descriptor `subscription_fd_readwrite::file_descriptor` has data
 * available for reading. This event always triggers for regular files.
 */
#define __WASI_EVENTTYPE_FD_READ (UINT8_C(1))

/**
 * File descriptor `subscription_fd_readwrite::file_descriptor` has capacity
 * available for writing. This event always triggers for regular files.
 */
#define __WASI_EVENTTYPE_FD_WRITE (UINT8_C(2))

_Static_assert(sizeof(__wasi_eventtype_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_eventtype_t) == 1, "witx calculated align");

/**
 * The state of the file descriptor subscribed to with
 * `eventtype::fd_read` or `eventtype::fd_write`.
 */
typedef uint16_t __wasi_eventrwflags_t;

/**
 * The peer of this socket has closed or disconnected.
 */
#define __WASI_EVENTRWFLAGS_FD_READWRITE_HANGUP ((__wasi_eventrwflags_t)(1 << 0))

/**
 * Flags for the 'fd_event' call
 */
typedef uint16_t __wasi_eventfdflags_t;

/**
 * Indicates if this event file description will run as a semaphore
 */
#define __WASI_EVENTFDFLAGS_SEMAPHORE ((__wasi_eventfdflags_t)(1 << 0))

/**
 * The contents of an `event` when type is `eventtype::fd_read` or
 * `eventtype::fd_write`.
 */
typedef struct __wasi_event_fd_readwrite_t {
    /**
     * The number of bytes available for reading or writing.
     */
    __wasi_filesize_t nbytes;

    /**
     * The state of the file descriptor.
     */
    __wasi_eventrwflags_t flags;

} __wasi_event_fd_readwrite_t;

_Static_assert(sizeof(__wasi_event_fd_readwrite_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_event_fd_readwrite_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_event_fd_readwrite_t, nbytes) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_fd_readwrite_t, flags) == 8, "witx calculated offset");

/**
 * An event that occurred.
 */
typedef struct __wasi_event_t {
    /**
     * User-provided value that got attached to `subscription::userdata`.
     */
    __wasi_userdata_t userdata;

    /**
     * If non-zero, an error that occurred while processing the subscription request.
     */
    __wasi_errno_t error;

    /**
     * The type of event that occured
     */
    __wasi_eventtype_t type;

    /**
     * The contents of the event, if it is an `eventtype::fd_read` or
     * `eventtype::fd_write`. `eventtype::clock` events ignore this field.
     */
    __wasi_event_fd_readwrite_t fd_readwrite;

} __wasi_event_t;

_Static_assert(sizeof(__wasi_event_t) == 32, "witx calculated size");
_Static_assert(_Alignof(__wasi_event_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_event_t, userdata) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, error) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, type) == 10, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, fd_readwrite) == 16, "witx calculated offset");

/**
 * Flags determining how to interpret the timestamp provided in
 * `subscription_clock::timeout`.
 */
typedef uint16_t __wasi_subclockflags_t;

/**
 * If set, treat the timestamp provided in
 * `subscription_clock::timeout` as an absolute timestamp of clock
 * `subscription_clock::id`. If clear, treat the timestamp
 * provided in `subscription_clock::timeout` relative to the
 * current time value of clock `subscription_clock::id`.
 */
#define __WASI_SUBCLOCKFLAGS_SUBSCRIPTION_CLOCK_ABSTIME ((__wasi_subclockflags_t)(1 << 0))

/**
 * The contents of a `subscription` when type is `eventtype::clock`.
 */
typedef struct __wasi_subscription_clock_t {
    /**
     * The clock against which to compare the timestamp.
     */
    __wasi_clockid_t id;

    /**
     * The absolute or relative timestamp.
     */
    __wasi_timestamp_t timeout;

    /**
     * The amount of time that the implementation may wait additionally
     * to coalesce with other events.
     */
    __wasi_timestamp_t precision;

    /**
     * Flags specifying whether the timeout is absolute or relative
     */
    __wasi_subclockflags_t flags;

} __wasi_subscription_clock_t;

_Static_assert(sizeof(__wasi_subscription_clock_t) == 32, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_clock_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_clock_t, id) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, timeout) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, precision) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, flags) == 24, "witx calculated offset");

/**
 * The contents of a `subscription` when type is type is
 * `eventtype::fd_read` or `eventtype::fd_write`.
 */
typedef struct __wasi_subscription_fd_readwrite_t {
    /**
     * The file descriptor on which to wait for it to become ready for reading or writing.
     */
    __wasi_fd_t file_descriptor;

} __wasi_subscription_fd_readwrite_t;

_Static_assert(sizeof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_fd_readwrite_t, file_descriptor) == 0, "witx calculated offset");

/**
 * The contents of a `subscription`.
 */
typedef union __wasi_subscription_u_u_t {
    __wasi_subscription_clock_t clock;
    __wasi_subscription_fd_readwrite_t fd_read;
    __wasi_subscription_fd_readwrite_t fd_write;
} __wasi_subscription_u_u_t;
typedef struct __wasi_subscription_u_t {
    uint8_t tag;
    __wasi_subscription_u_u_t u;
} __wasi_subscription_u_t;

_Static_assert(sizeof(__wasi_subscription_u_t) == 40, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_u_t) == 8, "witx calculated align");

/**
 * Subscription to an event.
 */
typedef struct __wasi_subscription_t {
    /**
     * User-provided value that is attached to the subscription in the
     * implementation and returned through `event::userdata`.
     */
    __wasi_userdata_t userdata;

    /**
     * The type of the event to which to subscribe, and its contents
     */
    __wasi_subscription_u_t u;

} __wasi_subscription_t;

_Static_assert(sizeof(__wasi_subscription_t) == 48, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_t, userdata) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_t, u) == 8, "witx calculated offset");

/**
 * Exit code generated by a process when exiting.
 */
typedef uint32_t __wasi_exitcode_t;

_Static_assert(sizeof(__wasi_exitcode_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_exitcode_t) == 4, "witx calculated align");

/**
 * Rect that represents the TTY.
 */
typedef struct __wasi_tty_t {
    /**
     * Number of columns
     */
    uint32_t cols;

    /**
     * Number of rows
     */
    uint32_t rows;

    /**
     * Width of the screen in pixels
     */
    uint32_t width;

    /**
     * Height of the screen in pixels
     */
    uint32_t height;

    /**
     * Indicates if stdin is a TTY
     */
    __wasi_bool_t stdin_tty;

    /**
     * Indicates if stdout is a TTY
     */
    __wasi_bool_t stdout_tty;

    /**
     * Indicates if stderr is a TTY
     */
    __wasi_bool_t stderr_tty;

    /**
     * When enabled the TTY will echo input to console
     */
    __wasi_bool_t echo;

    /**
     * When enabled buffers the input until the return key is pressed
     */
    __wasi_bool_t line_buffered;

} __wasi_tty_t;

_Static_assert(sizeof(__wasi_tty_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_tty_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_tty_t, cols) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, rows) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, width) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, height) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, stdin_tty) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, stdout_tty) == 17, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, stderr_tty) == 18, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, echo) == 19, "witx calculated offset");
_Static_assert(offsetof(__wasi_tty_t, line_buffered) == 20, "witx calculated offset");

/**
 * Fault codes that can be raised on a bus session.
 */
typedef uint32_t __wasi_bus_error_t;

/**
 * operation successful
 */
#define __WASI_BUS_ERROR_SUCCESS (UINT32_C(0))

/**
 * there was an error while serializing the request or response.
 */
#define __WASI_BUS_ERROR_SERIALIZATION (UINT32_C(1))

/**
 * there was an error while deserializing the request or response.
 */
#define __WASI_BUS_ERROR_DESERIALIZATION (UINT32_C(2))

/**
 * the specified WAPM module does not exist.
 */
#define __WASI_BUS_ERROR_INVALID_WAPM (UINT32_C(3))

/**
 * failed to fetch the WAPM module.
 */
#define __WASI_BUS_ERROR_FETCH_WAPM (UINT32_C(4))

/**
 * failed to compile the WAPM module.
 */
#define __WASI_BUS_ERROR_COMPILE_ERROR (UINT32_C(5))

/**
 * the ABI is invalid for cross module calls.
 */
#define __WASI_BUS_ERROR_INVALID_ABI (UINT32_C(6))

/**
 * the request has been aborted.
 */
#define __WASI_BUS_ERROR_ABORTED (UINT32_C(7))

/**
 * the handle is not valid.
 */
#define __WASI_BUS_ERROR_INVALID_HANDLE (UINT32_C(8))

#define __WASI_BUS_ERROR_INVALID_TOPIC (UINT32_C(9))

/**
 * some mandatory callbacks were not registered.
 */
#define __WASI_BUS_ERROR_MISSING_CALLBACK (UINT32_C(10))

/**
 * this operation is not supported on this platform.
 */
#define __WASI_BUS_ERROR_UNSUPPORTED (UINT32_C(11))

/**
 * invalid input was supplied in the call resulting in a bad request.
 */
#define __WASI_BUS_ERROR_BAD_REQUEST (UINT32_C(12))

/**
 * access denied
 */
#define __WASI_BUS_ERROR_ACCESS_DENIED (UINT32_C(13))

/**
 * an internal failure has occured
 */
#define __WASI_BUS_ERROR_INTERNAL_FAILURE (UINT32_C(14))

/**
 * memory allocation has failed
 */
#define __WASI_BUS_ERROR_MEMORY_ALLOCATION_FAILED (UINT32_C(15))

/**
 * bus invocation has failed
 */
#define __WASI_BUS_ERROR_BUS_INVOCATION_FAILED (UINT32_C(16))

/**
 * result already consumed
 */
#define __WASI_BUS_ERROR_ALREADY_CONSUMED (UINT32_C(17))

/**
 * memory access violation
 */
#define __WASI_BUS_ERROR_MEMORY_ACCESS_VIOLATION (UINT32_C(18))

/**
 * unknown error
 */
#define __WASI_BUS_ERROR_UNKNOWN_ERROR (UINT32_C(19))

_Static_assert(sizeof(__wasi_bus_error_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_error_t) == 4, "witx calculated align");

/**
 * Type of stdio mode to run for the sub process
 */
typedef uint8_t __wasi_stdio_mode_t;

/**
 * The stdio will be piped
 */
#define __WASI_STDIO_MODE_PIPED (UINT8_C(0))

/**
 * The stdio will inherit from its parent
 */
#define __WASI_STDIO_MODE_INHERIT (UINT8_C(1))

/**
 * The stdio will be dumped to null
 */
#define __WASI_STDIO_MODE_NULL (UINT8_C(2))

/**
 * The stdio will be written to the log file
 */
#define __WASI_STDIO_MODE_LOG (UINT8_C(3))

_Static_assert(sizeof(__wasi_stdio_mode_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_stdio_mode_t) == 1, "witx calculated align");

/**
 * Process handles.
 */
typedef struct __wasi_process_handles_t {
    /**
     * Handle that represents the process
     */
    __wasi_bid_t bid;

    /**
     * File handle for STDIN
     */
    __wasi_option_fd_t stdin;

    /**
     * File handle for STDOUT
     */
    __wasi_option_fd_t stdout;

    /**
     * File handle for STDERR
     */
    __wasi_option_fd_t stderr;

} __wasi_process_handles_t;

_Static_assert(sizeof(__wasi_process_handles_t) == 28, "witx calculated size");
_Static_assert(_Alignof(__wasi_process_handles_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_process_handles_t, bid) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stdin) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stdout) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stderr) == 20, "witx calculated offset");

/**
 * Bus process event.
 */
typedef struct __wasi_bus_event_exit_t {
    /**
     * Handle of the bus process that has exited
     */
    __wasi_bid_t bid;

    /**
     * Exit code of the bus process that has exited
     */
    __wasi_exitcode_t rval;

} __wasi_bus_event_exit_t;

_Static_assert(sizeof(__wasi_bus_event_exit_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_exit_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_bus_event_exit_t, bid) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_exit_t, rval) == 4, "witx calculated offset");

/**
 * Serialization format of data on the bus
 */
typedef uint8_t __wasi_bus_data_format_t;

/**
 * Raw binary data
 */
#define __WASI_BUS_DATA_FORMAT_RAW (UINT8_C(0))

/**
 * Uses the bincode serializer
 */
#define __WASI_BUS_DATA_FORMAT_BINCODE (UINT8_C(1))

/**
 * Uses the message pack serializer
 */
#define __WASI_BUS_DATA_FORMAT_MESSAGE_PACK (UINT8_C(2))

/**
 * JSON
 */
#define __WASI_BUS_DATA_FORMAT_JSON (UINT8_C(3))

/**
 * YAML
 */
#define __WASI_BUS_DATA_FORMAT_YAML (UINT8_C(4))

/**
 * XML
 */
#define __WASI_BUS_DATA_FORMAT_XML (UINT8_C(5))

/**
 * RKYV
 */
#define __WASI_BUS_DATA_FORMAT_RKYV (UINT8_C(6))

_Static_assert(sizeof(__wasi_bus_data_format_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_data_format_t) == 1, "witx calculated align");

/**
 * Represents an invocation from a caller
 */
typedef struct __wasi_bus_event_call_t {
    /**
     * Parent handle if this is a part of a call stack
     */
    __wasi_option_cid_t parent;

    /**
     * Handle of the call
     */
    __wasi_cid_t cid;

    /**
     * Format of the data on the bus
     */
    __wasi_bus_data_format_t format;

    /**
     * Hash of the topic name that identifies this operation
     */
    __wasi_hash_t topic_hash;

    /**
     * File descriptor that holds the event data
     */
    __wasi_fd_t fd;

} __wasi_bus_event_call_t;

_Static_assert(sizeof(__wasi_bus_event_call_t) == 56, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_call_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_bus_event_call_t, parent) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_call_t, cid) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_call_t, format) == 24, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_call_t, topic_hash) == 32, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_call_t, fd) == 48, "witx calculated offset");

/**
 * Represents the completion of an invocation from a caller
 */
typedef struct __wasi_bus_event_result_t {
    /**
     * Format of the data on the bus
     */
    __wasi_bus_data_format_t format;

    /**
     * Handle of the call
     */
    __wasi_cid_t cid;

    /**
     * File descriptor that holds the event data
     */
    __wasi_fd_t fd;

} __wasi_bus_event_result_t;

_Static_assert(sizeof(__wasi_bus_event_result_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_result_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_bus_event_result_t, format) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_result_t, cid) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_result_t, fd) == 16, "witx calculated offset");

/**
 * Bus event when an error occurs on a call.
 */
typedef struct __wasi_bus_event_fault_t {
    /**
     * Handle of the call where this event occurs for
     */
    __wasi_cid_t cid;

    /**
     * Fault that was raised against this call
     */
    __wasi_bus_error_t fault;

} __wasi_bus_event_fault_t;

_Static_assert(sizeof(__wasi_bus_event_fault_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_fault_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_bus_event_fault_t, cid) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_bus_event_fault_t, fault) == 8, "witx calculated offset");

/**
 * Frees all the resources associated with a call.
 */
typedef struct __wasi_bus_event_close_t {
    /**
     * Handle of the call to release resources
     */
    __wasi_cid_t cid;

} __wasi_bus_event_close_t;

_Static_assert(sizeof(__wasi_bus_event_close_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_close_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_bus_event_close_t, cid) == 0, "witx calculated offset");

/**
 * Type of a subscription to an event or its occurrence.
 */
typedef uint8_t __wasi_bus_event_type_t;

/**
 * Nothing has happened
 */
#define __WASI_BUS_EVENT_TYPE_NOOP (UINT8_C(0))

/**
 * The bus process has exited
 */
#define __WASI_BUS_EVENT_TYPE_EXIT (UINT8_C(1))

/**
 * Someone has invoked something
 */
#define __WASI_BUS_EVENT_TYPE_CALL (UINT8_C(2))

/**
 * An invocation has completed and this is the result
 */
#define __WASI_BUS_EVENT_TYPE_RESULT (UINT8_C(3))

/**
 * Fault has occured on one of the calls
 */
#define __WASI_BUS_EVENT_TYPE_FAULT (UINT8_C(4))

/**
 * Frees all the resources on a call
 */
#define __WASI_BUS_EVENT_TYPE_CLOSE (UINT8_C(5))

_Static_assert(sizeof(__wasi_bus_event_type_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_type_t) == 1, "witx calculated align");

/**
 * The contents of a `subscription`.
 */
typedef union __wasi_bus_event_u_t {
    uint8_t noop;
    __wasi_bus_event_exit_t exit;
    __wasi_bus_event_call_t call;
    __wasi_bus_event_result_t result;
    __wasi_bus_event_fault_t fault;
    __wasi_bus_event_close_t close;
} __wasi_bus_event_u_t;
typedef struct __wasi_bus_event_t {
    uint8_t tag;
    __wasi_bus_event_u_t u;
} __wasi_bus_event_t;

_Static_assert(sizeof(__wasi_bus_event_t) == 64, "witx calculated size");
_Static_assert(_Alignof(__wasi_bus_event_t) == 8, "witx calculated align");

/**
 * Signal condition.
 */
typedef uint8_t __wasi_signal_t;

/**
 * No signal. Note that POSIX has special semantics for `kill(pid, 0)`,
 * so this value is reserved.
 */
#define __WASI_SIGNAL_NONE (UINT8_C(0))

/**
 * Hangup.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_HUP (UINT8_C(1))

/**
 * Terminate interrupt signal.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_INT (UINT8_C(2))

/**
 * Terminal quit signal.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_QUIT (UINT8_C(3))

/**
 * Illegal instruction.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_ILL (UINT8_C(4))

/**
 * Trace/breakpoint trap.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_TRAP (UINT8_C(5))

/**
 * Process abort signal.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_ABRT (UINT8_C(6))

/**
 * Access to an undefined portion of a memory object.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_BUS (UINT8_C(7))

/**
 * Erroneous arithmetic operation.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_FPE (UINT8_C(8))

/**
 * Kill.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_KILL (UINT8_C(9))

/**
 * User-defined signal 1.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_USR1 (UINT8_C(10))

/**
 * Invalid memory reference.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_SEGV (UINT8_C(11))

/**
 * User-defined signal 2.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_USR2 (UINT8_C(12))

/**
 * Write on a pipe with no one to read it.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_PIPE (UINT8_C(13))

/**
 * Alarm clock.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_ALRM (UINT8_C(14))

/**
 * Termination signal.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_TERM (UINT8_C(15))

/**
 * Child process terminated, stopped, or continued.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_CHLD (UINT8_C(16))

/**
 * Continue executing, if stopped.
 * Action: Continues executing, if stopped.
 */
#define __WASI_SIGNAL_CONT (UINT8_C(17))

/**
 * Stop executing.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_STOP (UINT8_C(18))

/**
 * Terminal stop signal.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TSTP (UINT8_C(19))

/**
 * Background process attempting read.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TTIN (UINT8_C(20))

/**
 * Background process attempting write.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TTOU (UINT8_C(21))

/**
 * High bandwidth data is available at a socket.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_URG (UINT8_C(22))

/**
 * CPU time limit exceeded.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_XCPU (UINT8_C(23))

/**
 * File size limit exceeded.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_XFSZ (UINT8_C(24))

/**
 * Virtual timer expired.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_VTALRM (UINT8_C(25))

/**
 * Profiling timer expired.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_PROF (UINT8_C(26))

/**
 * Window changed.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_WINCH (UINT8_C(27))

/**
 * I/O possible.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_POLL (UINT8_C(28))

/**
 * Power failure.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_PWR (UINT8_C(29))

/**
 * Bad system call.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_SYS (UINT8_C(30))

_Static_assert(sizeof(__wasi_signal_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_signal_t) == 1, "witx calculated align");

/**
 * Flags provided to `sock_recv`.
 */
typedef uint16_t __wasi_riflags_t;

/**
 * Returns the message without removing it from the socket's receive queue.
 */
#define __WASI_RIFLAGS_RECV_PEEK ((__wasi_riflags_t)(1 << 0))

/**
 * On byte-stream sockets, block until the full amount of data can be returned.
 */
#define __WASI_RIFLAGS_RECV_WAITALL ((__wasi_riflags_t)(1 << 1))

/**
 * Indicates if the packet should be truncated to the buffer size
 */
#define __WASI_RIFLAGS_RECV_DATA_TRUNCATED ((__wasi_riflags_t)(1 << 2))

/**
 * Flags returned by `sock_recv`.
 */
typedef uint16_t __wasi_roflags_t;

/**
 * Returned by `sock_recv`: Message data has been truncated.
 */
#define __WASI_ROFLAGS_RECV_DATA_TRUNCATED ((__wasi_roflags_t)(1 << 0))

/**
 * Socket type
 */
typedef uint8_t __wasi_sock_type_t;

/**
 * The file descriptor or file refers to a datagram socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_DGRAM (UINT8_C(0))

/**
 * The file descriptor or file refers to a byte-stream socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_STREAM (UINT8_C(1))

/**
 * The file descriptor or file refers to a raw socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_RAW (UINT8_C(2))

/**
 * The file descriptor or file refers to a sequential packet socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_SEQPACKET (UINT8_C(3))

_Static_assert(sizeof(__wasi_sock_type_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_sock_type_t) == 1, "witx calculated align");

/**
 * Socket protocol type
 */
typedef uint16_t __wasi_sock_proto_t;

#define __WASI_SOCK_PROTO_IP (UINT16_C(0))

#define __WASI_SOCK_PROTO_ICMP (UINT16_C(1))

#define __WASI_SOCK_PROTO_IGMP (UINT16_C(2))

#define __WASI_SOCK_PROTO_PROTO_3 (UINT16_C(3))

#define __WASI_SOCK_PROTO_IPIP (UINT16_C(4))

#define __WASI_SOCK_PROTO_PROTO_5 (UINT16_C(5))

#define __WASI_SOCK_PROTO_TCP (UINT16_C(6))

#define __WASI_SOCK_PROTO_PROTO_7 (UINT16_C(7))

#define __WASI_SOCK_PROTO_EGP (UINT16_C(8))

#define __WASI_SOCK_PROTO_PROTO_9 (UINT16_C(9))

#define __WASI_SOCK_PROTO_PROTO_10 (UINT16_C(10))

#define __WASI_SOCK_PROTO_PROTO_11 (UINT16_C(11))

#define __WASI_SOCK_PROTO_PUP (UINT16_C(12))

#define __WASI_SOCK_PROTO_PROTO_13 (UINT16_C(13))

#define __WASI_SOCK_PROTO_PROTO_14 (UINT16_C(14))

#define __WASI_SOCK_PROTO_PROTO_15 (UINT16_C(15))

#define __WASI_SOCK_PROTO_PROTO_16 (UINT16_C(16))

#define __WASI_SOCK_PROTO_UDP (UINT16_C(17))

#define __WASI_SOCK_PROTO_PROTO_18 (UINT16_C(18))

#define __WASI_SOCK_PROTO_PROTO_19 (UINT16_C(19))

#define __WASI_SOCK_PROTO_PROTO_20 (UINT16_C(20))

#define __WASI_SOCK_PROTO_PROTO_21 (UINT16_C(21))

#define __WASI_SOCK_PROTO_IDP (UINT16_C(22))

#define __WASI_SOCK_PROTO_PROTO_23 (UINT16_C(23))

#define __WASI_SOCK_PROTO_PROTO_24 (UINT16_C(24))

#define __WASI_SOCK_PROTO_PROTO_25 (UINT16_C(25))

#define __WASI_SOCK_PROTO_PROTO_26 (UINT16_C(26))

#define __WASI_SOCK_PROTO_PROTO_27 (UINT16_C(27))

#define __WASI_SOCK_PROTO_PROTO_28 (UINT16_C(28))

#define __WASI_SOCK_PROTO_PROTO_TP (UINT16_C(29))

#define __WASI_SOCK_PROTO_PROTO_30 (UINT16_C(30))

#define __WASI_SOCK_PROTO_PROTO_31 (UINT16_C(31))

#define __WASI_SOCK_PROTO_PROTO_32 (UINT16_C(32))

#define __WASI_SOCK_PROTO_DCCP (UINT16_C(33))

#define __WASI_SOCK_PROTO_PROTO_34 (UINT16_C(34))

#define __WASI_SOCK_PROTO_PROTO_35 (UINT16_C(35))

#define __WASI_SOCK_PROTO_PROTO_36 (UINT16_C(36))

#define __WASI_SOCK_PROTO_PROTO_37 (UINT16_C(37))

#define __WASI_SOCK_PROTO_PROTO_38 (UINT16_C(38))

#define __WASI_SOCK_PROTO_PROTO_39 (UINT16_C(39))

#define __WASI_SOCK_PROTO_PROTO_40 (UINT16_C(40))

#define __WASI_SOCK_PROTO_IPV6 (UINT16_C(41))

#define __WASI_SOCK_PROTO_PROTO_42 (UINT16_C(42))

#define __WASI_SOCK_PROTO_ROUTING (UINT16_C(43))

#define __WASI_SOCK_PROTO_FRAGMENT (UINT16_C(44))

#define __WASI_SOCK_PROTO_PROTO_45 (UINT16_C(45))

#define __WASI_SOCK_PROTO_RSVP (UINT16_C(46))

#define __WASI_SOCK_PROTO_GRE (UINT16_C(47))

#define __WASI_SOCK_PROTO_PROTO_48 (UINT16_C(48))

#define __WASI_SOCK_PROTO_PROTO_49 (UINT16_C(49))

#define __WASI_SOCK_PROTO_ESP (UINT16_C(50))

#define __WASI_SOCK_PROTO_AH (UINT16_C(51))

#define __WASI_SOCK_PROTO_PROTO_52 (UINT16_C(52))

#define __WASI_SOCK_PROTO_PROTO_53 (UINT16_C(53))

#define __WASI_SOCK_PROTO_PROTO_54 (UINT16_C(54))

#define __WASI_SOCK_PROTO_PROTO_55 (UINT16_C(55))

#define __WASI_SOCK_PROTO_PROTO_56 (UINT16_C(56))

#define __WASI_SOCK_PROTO_PROTO_57 (UINT16_C(57))

#define __WASI_SOCK_PROTO_ICMPV6 (UINT16_C(58))

#define __WASI_SOCK_PROTO_NONE (UINT16_C(59))

#define __WASI_SOCK_PROTO_DSTOPTS (UINT16_C(60))

#define __WASI_SOCK_PROTO_PROTO_61 (UINT16_C(61))

#define __WASI_SOCK_PROTO_PROTO_62 (UINT16_C(62))

#define __WASI_SOCK_PROTO_PROTO_63 (UINT16_C(63))

#define __WASI_SOCK_PROTO_PROTO_64 (UINT16_C(64))

#define __WASI_SOCK_PROTO_PROTO_65 (UINT16_C(65))

#define __WASI_SOCK_PROTO_PROTO_66 (UINT16_C(66))

#define __WASI_SOCK_PROTO_PROTO_67 (UINT16_C(67))

#define __WASI_SOCK_PROTO_PROTO_68 (UINT16_C(68))

#define __WASI_SOCK_PROTO_PROTO_69 (UINT16_C(69))

#define __WASI_SOCK_PROTO_PROTO_70 (UINT16_C(70))

#define __WASI_SOCK_PROTO_PROTO_71 (UINT16_C(71))

#define __WASI_SOCK_PROTO_PROTO_72 (UINT16_C(72))

#define __WASI_SOCK_PROTO_PROTO_73 (UINT16_C(73))

#define __WASI_SOCK_PROTO_PROTO_74 (UINT16_C(74))

#define __WASI_SOCK_PROTO_PROTO_75 (UINT16_C(75))

#define __WASI_SOCK_PROTO_PROTO_76 (UINT16_C(76))

#define __WASI_SOCK_PROTO_PROTO_77 (UINT16_C(77))

#define __WASI_SOCK_PROTO_PROTO_78 (UINT16_C(78))

#define __WASI_SOCK_PROTO_PROTO_79 (UINT16_C(79))

#define __WASI_SOCK_PROTO_PROTO_80 (UINT16_C(80))

#define __WASI_SOCK_PROTO_PROTO_81 (UINT16_C(81))

#define __WASI_SOCK_PROTO_PROTO_82 (UINT16_C(82))

#define __WASI_SOCK_PROTO_PROTO_83 (UINT16_C(83))

#define __WASI_SOCK_PROTO_PROTO_84 (UINT16_C(84))

#define __WASI_SOCK_PROTO_PROTO_85 (UINT16_C(85))

#define __WASI_SOCK_PROTO_PROTO_86 (UINT16_C(86))

#define __WASI_SOCK_PROTO_PROTO_87 (UINT16_C(87))

#define __WASI_SOCK_PROTO_PROTO_88 (UINT16_C(88))

#define __WASI_SOCK_PROTO_PROTO_89 (UINT16_C(89))

#define __WASI_SOCK_PROTO_PROTO_90 (UINT16_C(90))

#define __WASI_SOCK_PROTO_PROTO_91 (UINT16_C(91))

#define __WASI_SOCK_PROTO_MTP (UINT16_C(92))

#define __WASI_SOCK_PROTO_PROTO_93 (UINT16_C(93))

#define __WASI_SOCK_PROTO_BEETPH (UINT16_C(94))

#define __WASI_SOCK_PROTO_PROTO_95 (UINT16_C(95))

#define __WASI_SOCK_PROTO_PROTO_96 (UINT16_C(96))

#define __WASI_SOCK_PROTO_PROTO_97 (UINT16_C(97))

#define __WASI_SOCK_PROTO_ENCAP (UINT16_C(98))

#define __WASI_SOCK_PROTO_PROTO_99 (UINT16_C(99))

#define __WASI_SOCK_PROTO_PROTO_100 (UINT16_C(100))

#define __WASI_SOCK_PROTO_PROTO_101 (UINT16_C(101))

#define __WASI_SOCK_PROTO_PROTO_102 (UINT16_C(102))

#define __WASI_SOCK_PROTO_PIM (UINT16_C(103))

#define __WASI_SOCK_PROTO_PROTO_104 (UINT16_C(104))

#define __WASI_SOCK_PROTO_PROTO_105 (UINT16_C(105))

#define __WASI_SOCK_PROTO_PROTO_106 (UINT16_C(106))

#define __WASI_SOCK_PROTO_PROTO_107 (UINT16_C(107))

#define __WASI_SOCK_PROTO_COMP (UINT16_C(108))

#define __WASI_SOCK_PROTO_PROTO_109 (UINT16_C(109))

#define __WASI_SOCK_PROTO_PROTO_110 (UINT16_C(110))

#define __WASI_SOCK_PROTO_PROTO_111 (UINT16_C(111))

#define __WASI_SOCK_PROTO_PROTO_112 (UINT16_C(112))

#define __WASI_SOCK_PROTO_PROTO_113 (UINT16_C(113))

#define __WASI_SOCK_PROTO_PROTO_114 (UINT16_C(114))

#define __WASI_SOCK_PROTO_PROTO_115 (UINT16_C(115))

#define __WASI_SOCK_PROTO_PROTO_116 (UINT16_C(116))

#define __WASI_SOCK_PROTO_PROTO_117 (UINT16_C(117))

#define __WASI_SOCK_PROTO_PROTO_118 (UINT16_C(118))

#define __WASI_SOCK_PROTO_PROTO_119 (UINT16_C(119))

#define __WASI_SOCK_PROTO_PROTO_120 (UINT16_C(120))

#define __WASI_SOCK_PROTO_PROTO_121 (UINT16_C(121))

#define __WASI_SOCK_PROTO_PROTO_122 (UINT16_C(122))

#define __WASI_SOCK_PROTO_PROTO_123 (UINT16_C(123))

#define __WASI_SOCK_PROTO_PROTO_124 (UINT16_C(124))

#define __WASI_SOCK_PROTO_PROTO_125 (UINT16_C(125))

#define __WASI_SOCK_PROTO_PROTO_126 (UINT16_C(126))

#define __WASI_SOCK_PROTO_PROTO_127 (UINT16_C(127))

#define __WASI_SOCK_PROTO_PROTO_128 (UINT16_C(128))

#define __WASI_SOCK_PROTO_PROTO_129 (UINT16_C(129))

#define __WASI_SOCK_PROTO_PROTO_130 (UINT16_C(130))

#define __WASI_SOCK_PROTO_PROTO_131 (UINT16_C(131))

#define __WASI_SOCK_PROTO_SCTP (UINT16_C(132))

#define __WASI_SOCK_PROTO_PROTO_133 (UINT16_C(133))

#define __WASI_SOCK_PROTO_PROTO_134 (UINT16_C(134))

#define __WASI_SOCK_PROTO_MH (UINT16_C(135))

#define __WASI_SOCK_PROTO_UDPLITE (UINT16_C(136))

#define __WASI_SOCK_PROTO_MPLS (UINT16_C(137))

#define __WASI_SOCK_PROTO_PROTO_138 (UINT16_C(138))

#define __WASI_SOCK_PROTO_PROTO_139 (UINT16_C(139))

#define __WASI_SOCK_PROTO_PROTO_140 (UINT16_C(140))

#define __WASI_SOCK_PROTO_PROTO_141 (UINT16_C(141))

#define __WASI_SOCK_PROTO_PROTO_142 (UINT16_C(142))

#define __WASI_SOCK_PROTO_ETHERNET (UINT16_C(143))

#define __WASI_SOCK_PROTO_PROTO_144 (UINT16_C(144))

#define __WASI_SOCK_PROTO_PROTO_145 (UINT16_C(145))

#define __WASI_SOCK_PROTO_PROTO_146 (UINT16_C(146))

#define __WASI_SOCK_PROTO_PROTO_147 (UINT16_C(147))

#define __WASI_SOCK_PROTO_PROTO_148 (UINT16_C(148))

#define __WASI_SOCK_PROTO_PROTO_149 (UINT16_C(149))

#define __WASI_SOCK_PROTO_PROTO_150 (UINT16_C(150))

#define __WASI_SOCK_PROTO_PROTO_151 (UINT16_C(151))

#define __WASI_SOCK_PROTO_PROTO_152 (UINT16_C(152))

#define __WASI_SOCK_PROTO_PROTO_153 (UINT16_C(153))

#define __WASI_SOCK_PROTO_PROTO_154 (UINT16_C(154))

#define __WASI_SOCK_PROTO_PROTO_155 (UINT16_C(155))

#define __WASI_SOCK_PROTO_PROTO_156 (UINT16_C(156))

#define __WASI_SOCK_PROTO_PROTO_157 (UINT16_C(157))

#define __WASI_SOCK_PROTO_PROTO_158 (UINT16_C(158))

#define __WASI_SOCK_PROTO_PROTO_159 (UINT16_C(159))

#define __WASI_SOCK_PROTO_PROTO_160 (UINT16_C(160))

#define __WASI_SOCK_PROTO_PROTO_161 (UINT16_C(161))

#define __WASI_SOCK_PROTO_PROTO_162 (UINT16_C(162))

#define __WASI_SOCK_PROTO_PROTO_163 (UINT16_C(163))

#define __WASI_SOCK_PROTO_PROTO_164 (UINT16_C(164))

#define __WASI_SOCK_PROTO_PROTO_165 (UINT16_C(165))

#define __WASI_SOCK_PROTO_PROTO_166 (UINT16_C(166))

#define __WASI_SOCK_PROTO_PROTO_167 (UINT16_C(167))

#define __WASI_SOCK_PROTO_PROTO_168 (UINT16_C(168))

#define __WASI_SOCK_PROTO_PROTO_169 (UINT16_C(169))

#define __WASI_SOCK_PROTO_PROTO_170 (UINT16_C(170))

#define __WASI_SOCK_PROTO_PROTO_171 (UINT16_C(171))

#define __WASI_SOCK_PROTO_PROTO_172 (UINT16_C(172))

#define __WASI_SOCK_PROTO_PROTO_173 (UINT16_C(173))

#define __WASI_SOCK_PROTO_PROTO_174 (UINT16_C(174))

#define __WASI_SOCK_PROTO_PROTO_175 (UINT16_C(175))

#define __WASI_SOCK_PROTO_PROTO_176 (UINT16_C(176))

#define __WASI_SOCK_PROTO_PROTO_177 (UINT16_C(177))

#define __WASI_SOCK_PROTO_PROTO_178 (UINT16_C(178))

#define __WASI_SOCK_PROTO_PROTO_179 (UINT16_C(179))

#define __WASI_SOCK_PROTO_PROTO_180 (UINT16_C(180))

#define __WASI_SOCK_PROTO_PROTO_181 (UINT16_C(181))

#define __WASI_SOCK_PROTO_PROTO_182 (UINT16_C(182))

#define __WASI_SOCK_PROTO_PROTO_183 (UINT16_C(183))

#define __WASI_SOCK_PROTO_PROTO_184 (UINT16_C(184))

#define __WASI_SOCK_PROTO_PROTO_185 (UINT16_C(185))

#define __WASI_SOCK_PROTO_PROTO_186 (UINT16_C(186))

#define __WASI_SOCK_PROTO_PROTO_187 (UINT16_C(187))

#define __WASI_SOCK_PROTO_PROTO_188 (UINT16_C(188))

#define __WASI_SOCK_PROTO_PROTO_189 (UINT16_C(189))

#define __WASI_SOCK_PROTO_PROTO_190 (UINT16_C(190))

#define __WASI_SOCK_PROTO_PROTO_191 (UINT16_C(191))

#define __WASI_SOCK_PROTO_PROTO_192 (UINT16_C(192))

#define __WASI_SOCK_PROTO_PROTO_193 (UINT16_C(193))

#define __WASI_SOCK_PROTO_PROTO_194 (UINT16_C(194))

#define __WASI_SOCK_PROTO_PROTO_195 (UINT16_C(195))

#define __WASI_SOCK_PROTO_PROTO_196 (UINT16_C(196))

#define __WASI_SOCK_PROTO_PROTO_197 (UINT16_C(197))

#define __WASI_SOCK_PROTO_PROTO_198 (UINT16_C(198))

#define __WASI_SOCK_PROTO_PROTO_199 (UINT16_C(199))

#define __WASI_SOCK_PROTO_PROTO_200 (UINT16_C(200))

#define __WASI_SOCK_PROTO_PROTO_201 (UINT16_C(201))

#define __WASI_SOCK_PROTO_PROTO_202 (UINT16_C(202))

#define __WASI_SOCK_PROTO_PROTO_203 (UINT16_C(203))

#define __WASI_SOCK_PROTO_PROTO_204 (UINT16_C(204))

#define __WASI_SOCK_PROTO_PROTO_205 (UINT16_C(205))

#define __WASI_SOCK_PROTO_PROTO_206 (UINT16_C(206))

#define __WASI_SOCK_PROTO_PROTO_207 (UINT16_C(207))

#define __WASI_SOCK_PROTO_PROTO_208 (UINT16_C(208))

#define __WASI_SOCK_PROTO_PROTO_209 (UINT16_C(209))

#define __WASI_SOCK_PROTO_PROTO_210 (UINT16_C(210))

#define __WASI_SOCK_PROTO_PROTO_211 (UINT16_C(211))

#define __WASI_SOCK_PROTO_PROTO_212 (UINT16_C(212))

#define __WASI_SOCK_PROTO_PROTO_213 (UINT16_C(213))

#define __WASI_SOCK_PROTO_PROTO_214 (UINT16_C(214))

#define __WASI_SOCK_PROTO_PROTO_215 (UINT16_C(215))

#define __WASI_SOCK_PROTO_PROTO_216 (UINT16_C(216))

#define __WASI_SOCK_PROTO_PROTO_217 (UINT16_C(217))

#define __WASI_SOCK_PROTO_PROTO_218 (UINT16_C(218))

#define __WASI_SOCK_PROTO_PROTO_219 (UINT16_C(219))

#define __WASI_SOCK_PROTO_PROTO_220 (UINT16_C(220))

#define __WASI_SOCK_PROTO_PROTO_221 (UINT16_C(221))

#define __WASI_SOCK_PROTO_PROTO_222 (UINT16_C(222))

#define __WASI_SOCK_PROTO_PROTO_223 (UINT16_C(223))

#define __WASI_SOCK_PROTO_PROTO_224 (UINT16_C(224))

#define __WASI_SOCK_PROTO_PROTO_225 (UINT16_C(225))

#define __WASI_SOCK_PROTO_PROTO_226 (UINT16_C(226))

#define __WASI_SOCK_PROTO_PROTO_227 (UINT16_C(227))

#define __WASI_SOCK_PROTO_PROTO_228 (UINT16_C(228))

#define __WASI_SOCK_PROTO_PROTO_229 (UINT16_C(229))

#define __WASI_SOCK_PROTO_PROTO_230 (UINT16_C(230))

#define __WASI_SOCK_PROTO_PROTO_231 (UINT16_C(231))

#define __WASI_SOCK_PROTO_PROTO_232 (UINT16_C(232))

#define __WASI_SOCK_PROTO_PROTO_233 (UINT16_C(233))

#define __WASI_SOCK_PROTO_PROTO_234 (UINT16_C(234))

#define __WASI_SOCK_PROTO_PROTO_235 (UINT16_C(235))

#define __WASI_SOCK_PROTO_PROTO_236 (UINT16_C(236))

#define __WASI_SOCK_PROTO_PROTO_237 (UINT16_C(237))

#define __WASI_SOCK_PROTO_PROTO_238 (UINT16_C(238))

#define __WASI_SOCK_PROTO_PROTO_239 (UINT16_C(239))

#define __WASI_SOCK_PROTO_PROTO_240 (UINT16_C(240))

#define __WASI_SOCK_PROTO_PROTO_241 (UINT16_C(241))

#define __WASI_SOCK_PROTO_PROTO_242 (UINT16_C(242))

#define __WASI_SOCK_PROTO_PROTO_243 (UINT16_C(243))

#define __WASI_SOCK_PROTO_PROTO_244 (UINT16_C(244))

#define __WASI_SOCK_PROTO_PROTO_245 (UINT16_C(245))

#define __WASI_SOCK_PROTO_PROTO_246 (UINT16_C(246))

#define __WASI_SOCK_PROTO_PROTO_247 (UINT16_C(247))

#define __WASI_SOCK_PROTO_PROTO_248 (UINT16_C(248))

#define __WASI_SOCK_PROTO_PROTO_249 (UINT16_C(249))

#define __WASI_SOCK_PROTO_PROTO_250 (UINT16_C(250))

#define __WASI_SOCK_PROTO_PROTO_251 (UINT16_C(251))

#define __WASI_SOCK_PROTO_PROTO_252 (UINT16_C(252))

#define __WASI_SOCK_PROTO_PROTO_253 (UINT16_C(253))

#define __WASI_SOCK_PROTO_PROTO_254 (UINT16_C(254))

#define __WASI_SOCK_PROTO_PROTO_RAW (UINT16_C(255))

#define __WASI_SOCK_PROTO_PROTO_256 (UINT16_C(256))

#define __WASI_SOCK_PROTO_PROTO_257 (UINT16_C(257))

#define __WASI_SOCK_PROTO_PROTO_258 (UINT16_C(258))

#define __WASI_SOCK_PROTO_PROTO_259 (UINT16_C(259))

#define __WASI_SOCK_PROTO_PROTO_260 (UINT16_C(260))

#define __WASI_SOCK_PROTO_PROTO_261 (UINT16_C(261))

#define __WASI_SOCK_PROTO_MPTCP (UINT16_C(262))

#define __WASI_SOCK_PROTO_MAX (UINT16_C(263))

_Static_assert(sizeof(__wasi_sock_proto_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_sock_proto_t) == 2, "witx calculated align");

/**
 * Socket status
 */
typedef uint8_t __wasi_sock_status_t;

/**
 * The socket is still opening
 */
#define __WASI_SOCK_STATUS_OPENING (UINT8_C(0))

/**
 * The socket has fully opened
 */
#define __WASI_SOCK_STATUS_OPENED (UINT8_C(1))

/**
 * The socket has closed
 */
#define __WASI_SOCK_STATUS_CLOSED (UINT8_C(2))

/**
 * The socket has failed
 */
#define __WASI_SOCK_STATUS_FAILED (UINT8_C(3))

_Static_assert(sizeof(__wasi_sock_status_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_sock_status_t) == 1, "witx calculated align");

/**
 * Socket option
 */
typedef uint8_t __wasi_sock_option_t;

/**
 * Socket option that does nothing
 */
#define __WASI_SOCK_OPTION_NOOP (UINT8_C(0))

/**
 * Reuse Port
 */
#define __WASI_SOCK_OPTION_REUSE_PORT (UINT8_C(1))

/**
 * Reuse Address
 */
#define __WASI_SOCK_OPTION_REUSE_ADDR (UINT8_C(2))

/**
 * No delay
 */
#define __WASI_SOCK_OPTION_NO_DELAY (UINT8_C(3))

/**
 * Dont route
 */
#define __WASI_SOCK_OPTION_DONT_ROUTE (UINT8_C(4))

/**
 * Only accept IPv6
 */
#define __WASI_SOCK_OPTION_ONLY_V6 (UINT8_C(5))

/**
 * Broadcast
 */
#define __WASI_SOCK_OPTION_BROADCAST (UINT8_C(6))

/**
 * Multicast Loop IPv4
 */
#define __WASI_SOCK_OPTION_MULTICAST_LOOP_V4 (UINT8_C(7))

/**
 * Multicast Loop IPv6
 */
#define __WASI_SOCK_OPTION_MULTICAST_LOOP_V6 (UINT8_C(8))

/**
 * Promiscuous
 */
#define __WASI_SOCK_OPTION_PROMISCUOUS (UINT8_C(9))

/**
 * Socket is listening
 */
#define __WASI_SOCK_OPTION_LISTENING (UINT8_C(10))

/**
 * Last error
 */
#define __WASI_SOCK_OPTION_LAST_ERROR (UINT8_C(11))

/**
 * Keep alive
 */
#define __WASI_SOCK_OPTION_KEEP_ALIVE (UINT8_C(12))

/**
 * Linger time
 */
#define __WASI_SOCK_OPTION_LINGER (UINT8_C(13))

/**
 * Out-of-band inline
 */
#define __WASI_SOCK_OPTION_OOB_INLINE (UINT8_C(14))

/**
 * Receive buffer size
 */
#define __WASI_SOCK_OPTION_RECV_BUF_SIZE (UINT8_C(15))

/**
 * Send buffer size
 */
#define __WASI_SOCK_OPTION_SEND_BUF_SIZE (UINT8_C(16))

/**
 * Receive lowat
 */
#define __WASI_SOCK_OPTION_RECV_LOWAT (UINT8_C(17))

/**
 * Send lowat
 */
#define __WASI_SOCK_OPTION_SEND_LOWAT (UINT8_C(18))

/**
 * Receive timeout
 */
#define __WASI_SOCK_OPTION_RECV_TIMEOUT (UINT8_C(19))

/**
 * Send timeout
 */
#define __WASI_SOCK_OPTION_SEND_TIMEOUT (UINT8_C(20))

/**
 * Connect timeout
 */
#define __WASI_SOCK_OPTION_CONNECT_TIMEOUT (UINT8_C(21))

/**
 * Accept timeout
 */
#define __WASI_SOCK_OPTION_ACCEPT_TIMEOUT (UINT8_C(22))

/**
 * TTL of packets
 */
#define __WASI_SOCK_OPTION_TTL (UINT8_C(23))

/**
 * Multicast TTL for IPv4
 */
#define __WASI_SOCK_OPTION_MULTICAST_TTL_V4 (UINT8_C(24))

/**
 * Type of socket
 */
#define __WASI_SOCK_OPTION_TYPE (UINT8_C(25))

/**
 * Protocol of the socket
 */
#define __WASI_SOCK_OPTION_PROTO (UINT8_C(26))

_Static_assert(sizeof(__wasi_sock_option_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_sock_option_t) == 1, "witx calculated align");

/**
 * Level of security to allow to the streaming connection
 */
typedef uint8_t __wasi_stream_security_t;

/**
 * Unencrypted
 */
#define __WASI_STREAM_SECURITY_UNENCRYPTED ((__wasi_stream_security_t)(1 << 0))

/**
 * Any encryption
 */
#define __WASI_STREAM_SECURITY_ANY_ENCRYPTION ((__wasi_stream_security_t)(1 << 1))

/**
 * Classic encryption
 */
#define __WASI_STREAM_SECURITY_CLASSIC_ENCRYPTION ((__wasi_stream_security_t)(1 << 2))

/**
 * Double encryption
 */
#define __WASI_STREAM_SECURITY_DOUBLE_ENCRYPTION ((__wasi_stream_security_t)(1 << 3))

/**
 * Hardware address (MAC)
 */
typedef struct __wasi_hardware_address_t {
    uint8_t n0;

    uint8_t n1;

    uint8_t n2;

    uint8_t h0;

    uint8_t h1;

    uint8_t h2;

} __wasi_hardware_address_t;

_Static_assert(sizeof(__wasi_hardware_address_t) == 6, "witx calculated size");
_Static_assert(_Alignof(__wasi_hardware_address_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_hardware_address_t, n0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_hardware_address_t, n1) == 1, "witx calculated offset");
_Static_assert(offsetof(__wasi_hardware_address_t, n2) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_hardware_address_t, h0) == 3, "witx calculated offset");
_Static_assert(offsetof(__wasi_hardware_address_t, h1) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_hardware_address_t, h2) == 5, "witx calculated offset");

/**
 * IP port number
 */
typedef uint16_t __wasi_ip_port_t;

_Static_assert(sizeof(__wasi_ip_port_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_ip_port_t) == 2, "witx calculated align");

/**
 * Address family
 */
typedef uint8_t __wasi_address_family_t;

/**
 * Unspecified
 */
#define __WASI_ADDRESS_FAMILY_UNSPEC (UINT8_C(0))

/**
 * IP v4
 */
#define __WASI_ADDRESS_FAMILY_INET4 (UINT8_C(1))

/**
 * IP v6
 */
#define __WASI_ADDRESS_FAMILY_INET6 (UINT8_C(2))

/**
 * Unix
 */
#define __WASI_ADDRESS_FAMILY_UNIX (UINT8_C(3))

_Static_assert(sizeof(__wasi_address_family_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_address_family_t) == 1, "witx calculated align");

/**
 * Represents an unspecified address
 */
typedef struct __wasi_addr_unspec_t {
    uint8_t n0;

} __wasi_addr_unspec_t;

_Static_assert(sizeof(__wasi_addr_unspec_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unspec_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unspec_t, n0) == 0, "witx calculated offset");

/**
 * An unspecified address with a port
 */
typedef struct __wasi_addr_unspec_port_t {
    __wasi_ip_port_t port;

    __wasi_addr_unspec_t addr;

} __wasi_addr_unspec_port_t;

_Static_assert(sizeof(__wasi_addr_unspec_port_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unspec_port_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unspec_port_t, port) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unspec_port_t, addr) == 2, "witx calculated offset");

/**
 * An unspecified address with a cidr
 */
typedef struct __wasi_addr_unspec_cidr_t {
    __wasi_addr_unspec_t addr;

    uint8_t prefix;

} __wasi_addr_unspec_cidr_t;

_Static_assert(sizeof(__wasi_addr_unspec_cidr_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unspec_cidr_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unspec_cidr_t, addr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unspec_cidr_t, prefix) == 1, "witx calculated offset");

/**
 * An IPv4 address is a 32-bit number that uniquely identifies a network interface on a machine.
 */
typedef struct __wasi_addr_ip4_t {
    uint8_t n0;

    uint8_t n1;

    uint8_t h0;

    uint8_t h1;

} __wasi_addr_ip4_t;

_Static_assert(sizeof(__wasi_addr_ip4_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip4_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip4_t, n0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip4_t, n1) == 1, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip4_t, h0) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip4_t, h1) == 3, "witx calculated offset");

/**
 * An IPv4 address with a port number
 */
typedef struct __wasi_addr_ip4_port_t {
    __wasi_ip_port_t port;

    __wasi_addr_ip4_t addr;

} __wasi_addr_ip4_port_t;

_Static_assert(sizeof(__wasi_addr_ip4_port_t) == 6, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip4_port_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip4_port_t, port) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip4_port_t, addr) == 2, "witx calculated offset");

/**
 * An IPv4 address CIDR
 */
typedef struct __wasi_addr_ip4_cidr_t {
    __wasi_addr_ip4_t addr;

    uint8_t prefix;

} __wasi_addr_ip4_cidr_t;

_Static_assert(sizeof(__wasi_addr_ip4_cidr_t) == 5, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip4_cidr_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip4_cidr_t, addr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip4_cidr_t, prefix) == 4, "witx calculated offset");

/**
 * An IPv6 address is a 128-bit number that uniquely identifies a network interface on a machine.
 */
typedef struct __wasi_addr_ip6_t {
    uint16_t n0;

    uint16_t n1;

    uint16_t n2;

    uint16_t n3;

    uint16_t h0;

    uint16_t h1;

    uint16_t h2;

    uint16_t h3;

} __wasi_addr_ip6_t;

_Static_assert(sizeof(__wasi_addr_ip6_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip6_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip6_t, n0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, n1) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, n2) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, n3) == 6, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, h0) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, h1) == 10, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, h2) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_t, h3) == 14, "witx calculated offset");

/**
 * Unix socket that is bound to no more than 15 bytes
 */
typedef struct __wasi_addr_unix_t {
    uint8_t b0;

    uint8_t b1;

    uint8_t b2;

    uint8_t b3;

    uint8_t b4;

    uint8_t b5;

    uint8_t b6;

    uint8_t b7;

    uint8_t b8;

    uint8_t b9;

    uint8_t b10;

    uint8_t b11;

    uint8_t b12;

    uint8_t b13;

    uint8_t b14;

    uint8_t b15;

} __wasi_addr_unix_t;

_Static_assert(sizeof(__wasi_addr_unix_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unix_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unix_t, b0) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b1) == 1, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b2) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b3) == 3, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b4) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b5) == 5, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b6) == 6, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b7) == 7, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b8) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b9) == 9, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b10) == 10, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b11) == 11, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b12) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b13) == 13, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b14) == 14, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_t, b15) == 15, "witx calculated offset");

/**
 * A unix socket with a port
 */
typedef struct __wasi_addr_unix_port_t {
    __wasi_ip_port_t port;

    __wasi_addr_unix_t addr;

} __wasi_addr_unix_port_t;

_Static_assert(sizeof(__wasi_addr_unix_port_t) == 18, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unix_port_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unix_port_t, port) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_port_t, addr) == 2, "witx calculated offset");

/**
 * A unix socket with a cidr
 */
typedef struct __wasi_addr_unix_cidr_t {
    __wasi_addr_unix_t addr;

    uint8_t prefix;

} __wasi_addr_unix_cidr_t;

_Static_assert(sizeof(__wasi_addr_unix_cidr_t) == 17, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_unix_cidr_t) == 1, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_unix_cidr_t, addr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_unix_cidr_t, prefix) == 16, "witx calculated offset");

/**
 * An IPv6 address with a port number
 */
typedef struct __wasi_addr_ip6_port_t {
    __wasi_ip_port_t port;

    __wasi_addr_ip6_t addr;

} __wasi_addr_ip6_port_t;

_Static_assert(sizeof(__wasi_addr_ip6_port_t) == 18, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip6_port_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip6_port_t, port) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_port_t, addr) == 2, "witx calculated offset");

/**
 * An IPv6 address CIDR
 */
typedef struct __wasi_addr_ip6_cidr_t {
    __wasi_addr_ip6_t addr;

    uint8_t prefix;

} __wasi_addr_ip6_cidr_t;

_Static_assert(sizeof(__wasi_addr_ip6_cidr_t) == 18, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_ip6_cidr_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_addr_ip6_cidr_t, addr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_addr_ip6_cidr_t, prefix) == 16, "witx calculated offset");

/**
 * Union of all possible addresses type
 */
typedef union __wasi_addr_u_t {
    __wasi_addr_unspec_t unspec;
    __wasi_addr_ip4_t inet4;
    __wasi_addr_ip6_t inet6;
    __wasi_addr_unix_t unix;
} __wasi_addr_u_t;
typedef struct __wasi_addr_t {
    uint8_t tag;
    __wasi_addr_u_t u;
} __wasi_addr_t;

_Static_assert(sizeof(__wasi_addr_t) == 18, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_t) == 2, "witx calculated align");

/**
 * Union that makes a generic IP address and port
 */
typedef union __wasi_addr_port_u_t {
    __wasi_addr_unspec_port_t unspec;
    __wasi_addr_ip4_port_t inet4;
    __wasi_addr_ip6_port_t inet6;
    __wasi_addr_unix_port_t unix;
} __wasi_addr_port_u_t;
typedef struct __wasi_addr_port_t {
    uint8_t tag;
    __wasi_addr_port_u_t u;
} __wasi_addr_port_t;

_Static_assert(sizeof(__wasi_addr_port_t) == 20, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_port_t) == 2, "witx calculated align");

/**
 * Union that makes a generic IP address and prefix. a.k.a. CIDR
 */
typedef union __wasi_addr_cidr_u_t {
    __wasi_addr_unspec_cidr_t unspec;
    __wasi_addr_ip4_cidr_t inet4;
    __wasi_addr_ip6_cidr_t inet6;
    __wasi_addr_unix_cidr_t unix;
} __wasi_addr_cidr_u_t;
typedef struct __wasi_addr_cidr_t {
    uint8_t tag;
    __wasi_addr_cidr_u_t u;
} __wasi_addr_cidr_t;

_Static_assert(sizeof(__wasi_addr_cidr_t) == 20, "witx calculated size");
_Static_assert(_Alignof(__wasi_addr_cidr_t) == 2, "witx calculated align");

typedef struct __wasi_route_t {
    __wasi_addr_cidr_t cidr;

    __wasi_addr_t via_router;

    __wasi_option_timestamp_t preferred_until;

    __wasi_option_timestamp_t expires_at;

} __wasi_route_t;

_Static_assert(sizeof(__wasi_route_t) == 72, "witx calculated size");
_Static_assert(_Alignof(__wasi_route_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_route_t, cidr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_route_t, via_router) == 20, "witx calculated offset");
_Static_assert(offsetof(__wasi_route_t, preferred_until) == 40, "witx calculated offset");
_Static_assert(offsetof(__wasi_route_t, expires_at) == 56, "witx calculated offset");

/**
 * HTTP request handles used to send and receive data to the server
 */
typedef struct __wasi_http_handles_t {
    /**
     * File handle used to write the request data
     */
    __wasi_fd_t request;

    /**
     * File handle used to receive the response data
     */
    __wasi_fd_t response;

    /**
     * File handle used to read the response headers
     * (entries are separated by line feeds)
     */
    __wasi_fd_t headers;

} __wasi_http_handles_t;

_Static_assert(sizeof(__wasi_http_handles_t) == 12, "witx calculated size");
_Static_assert(_Alignof(__wasi_http_handles_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_http_handles_t, request) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_http_handles_t, response) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_http_handles_t, headers) == 8, "witx calculated offset");

/**
 * HTTP response status
 */
typedef struct __wasi_http_status_t {
    __wasi_bool_t ok;

    __wasi_bool_t redirected;

    /**
     * Size of the response
     */
    __wasi_filesize_t size;

    /**
     * HTTP status code for this response
     */
    uint16_t status;

} __wasi_http_status_t;

_Static_assert(sizeof(__wasi_http_status_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_http_status_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_http_status_t, ok) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_http_status_t, redirected) == 1, "witx calculated offset");
_Static_assert(offsetof(__wasi_http_status_t, size) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_http_status_t, status) == 16, "witx calculated offset");

/**
 * Flags provided to `sock_send`. As there are currently no flags
 * defined, it must be set to zero.
 */
typedef uint16_t __wasi_siflags_t;

_Static_assert(sizeof(__wasi_siflags_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_siflags_t) == 2, "witx calculated align");

/**
 * Which channels on a socket to shut down.
 */
typedef uint8_t __wasi_sdflags_t;

/**
 * Disables further receive operations.
 */
#define __WASI_SDFLAGS_RD ((__wasi_sdflags_t)(1 << 0))

/**
 * Disables further send operations.
 */
#define __WASI_SDFLAGS_WR ((__wasi_sdflags_t)(1 << 1))

/**
 * Identifiers for preopened capabilities.
 */
typedef uint8_t __wasi_preopentype_t;

/**
 * A pre-opened directory.
 */
#define __WASI_PREOPENTYPE_DIR (UINT8_C(0))

_Static_assert(sizeof(__wasi_preopentype_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_preopentype_t) == 1, "witx calculated align");

/**
 * The contents of a $prestat when type is `preopentype::dir`.
 */
typedef struct __wasi_prestat_dir_t {
    /**
     * The length of the directory name for use with `fd_prestat_dir_name`.
     */
    uint32_t pr_name_len;

} __wasi_prestat_dir_t;

_Static_assert(sizeof(__wasi_prestat_dir_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_prestat_dir_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_prestat_dir_t, pr_name_len) == 0, "witx calculated offset");

/**
 * Information about a pre-opened capability.
 */
typedef union __wasi_prestat_u_t {
    __wasi_prestat_dir_t dir;
} __wasi_prestat_u_t;
typedef struct __wasi_prestat_t {
    uint8_t tag;
    __wasi_prestat_u_t u;
} __wasi_prestat_t;

_Static_assert(sizeof(__wasi_prestat_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_prestat_t) == 4, "witx calculated align");

/**
 * @defgroup wasix_64v1
 * @{
 */

/**
 * Read command-line argument data.
 * The size of the array should match that returned by `args_sizes_get`.
 * Each argument is expected to be `\0` terminated.
 */
__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
) __attribute__((__warn_unused_result__));
/**
 * Return command-line argument data sizes.
 * @return
 * Returns the number of arguments and the size of the argument string
 * data, or an error.
 */
__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Read environment variable data.
 * The sizes of the buffers should match that returned by `environ_sizes_get`.
 * Key/value pairs are expected to be joined with `=`s, and terminated with `\0`s.
 */
__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
) __attribute__((__warn_unused_result__));
/**
 * Return environment variable data sizes.
 * @return
 * Returns the number of environment variable arguments and the size of the
 * environment variable data.
 */
__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Return the resolution of a clock.
 * Implementations are required to provide a non-zero value for supported clocks. For unsupported clocks,
 * return `errno::inval`.
 * Note: This is similar to `clock_getres` in POSIX.
 * @return
 * The resolution of the clock, or an error if one happened.
 */
__wasi_errno_t __wasi_clock_res_get(
    /**
     * The clock for which to return the resolution.
     */
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Return the time value of a clock.
 * Note: This is similar to `clock_gettime` in POSIX.
 * @return
 * The time value of the clock.
 */
__wasi_errno_t __wasi_clock_time_get(
    /**
     * The clock for which to return the time.
     */
    __wasi_clockid_t id,
    /**
     * The maximum lag (exclusive) that the returned time value may have, compared to its actual value.
     */
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sets the time value of a clock.
 * Note: This is similar to `clock_settime` in POSIX.
 * @return
 * The time value of the clock.
 */
__wasi_errno_t __wasi_clock_time_set(
    /**
     * The clock for which to set the time.
     */
    __wasi_clockid_t id,
    /**
     * The value of the time to be set.
     */
    __wasi_timestamp_t timestamp
) __attribute__((__warn_unused_result__));
/**
 * Provide file advisory information on a file descriptor.
 * Note: This is similar to `posix_fadvise` in POSIX.
 */
__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    /**
     * The offset within the file to which the advisory applies.
     */
    __wasi_filesize_t offset,
    /**
     * The length of the region to which the advisory applies.
     */
    __wasi_filesize_t len,
    /**
     * The advice.
     */
    __wasi_advice_t advice
) __attribute__((__warn_unused_result__));
/**
 * Force the allocation of space in a file.
 * Note: This is similar to `posix_fallocate` in POSIX.
 */
__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    /**
     * The offset at which to start the allocation.
     */
    __wasi_filesize_t offset,
    /**
     * The length of the area that is allocated.
     */
    __wasi_filesize_t len
) __attribute__((__warn_unused_result__));
/**
 * Close a file descriptor.
 * Note: This is similar to `close` in POSIX.
 */
__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));
/**
 * Synchronize the data of a file to disk.
 * Note: This is similar to `fdatasync` in POSIX.
 */
__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));
/**
 * Get the attributes of a file descriptor.
 * Note: This returns similar flags to `fsync(fd, F_GETFL)` in POSIX, as well as additional fields.
 * @return
 * The buffer where the file descriptor's attributes are stored.
 */
__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Adjust the flags associated with a file descriptor.
 * Note: This is similar to `fcntl(fd, F_SETFL, flags)` in POSIX.
 */
__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    /**
     * The desired values of the file descriptor flags.
     */
    __wasi_fdflags_t flags
) __attribute__((__warn_unused_result__));
/**
 * Adjust the rights associated with a file descriptor.
 * This can only be used to remove rights, and returns `errno::notcapable` if called in a way that would attempt to add rights
 */
__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    /**
     * The desired rights of the file descriptor.
     */
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
) __attribute__((__warn_unused_result__));
/**
 * Return the attributes of an open file.
 * @return
 * The buffer where the file's attributes are stored.
 */
__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Adjust the size of an open file. If this increases the file's size, the extra bytes are filled with zeros.
 * Note: This is similar to `ftruncate` in POSIX.
 */
__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    /**
     * The desired file size.
     */
    __wasi_filesize_t size
) __attribute__((__warn_unused_result__));
/**
 * Adjust the timestamps of an open file or directory.
 * Note: This is similar to `futimens` in POSIX.
 */
__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    /**
     * The desired values of the data access timestamp.
     */
    __wasi_timestamp_t atim,
    /**
     * The desired values of the data modification timestamp.
     */
    __wasi_timestamp_t mtim,
    /**
     * A bitmask indicating which timestamps to adjust.
     */
    __wasi_fstflags_t fst_flags
) __attribute__((__warn_unused_result__));
/**
 * Read from a file descriptor, without using and updating the file descriptor's offset.
 * Note: This is similar to `preadv` in POSIX.
 * @return
 * The number of bytes read.
 */
__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors in which to store data.
     */
    const __wasi_iovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    /**
     * The offset within the file at which to read.
     */
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Return a description of the given preopened file descriptor.
 * @return
 * The buffer where the description is stored.
 */
__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Return a description of the given preopened file descriptor.
 */
__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    /**
     * A buffer into which to write the preopened directory name.
     */
    uint8_t * path,
    __wasi_pointersize_t path_len
) __attribute__((__warn_unused_result__));
/**
 * Write to a file descriptor, without using and updating the file descriptor's offset.
 * Note: This is similar to `pwritev` in POSIX.
 * @return
 * The number of bytes written.
 */
__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors from which to retrieve data.
     */
    const __wasi_ciovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    /**
     * The offset within the file at which to write.
     */
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Read from a file descriptor.
 * Note: This is similar to `readv` in POSIX.
 * @return
 * The number of bytes read.
 */
__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to store data.
     */
    const __wasi_iovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Read directory entries from a directory.
 * When successful, the contents of the output buffer consist of a sequence of
 * directory entries. Each directory entry consists of a `dirent` object,
 * followed by `dirent::d_namlen` bytes holding the name of the directory
 * entry.
 * This function fills the output buffer as much as possible, potentially
 * truncating the last directory entry. This allows the caller to grow its
 * read buffer size in case it's too small to fit a single large directory
 * entry, or skip the oversized directory entry.
 * @return
 * The number of bytes stored in the read buffer. If less than the size of the read buffer, the end of the directory has been reached.
 */
__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    /**
     * The buffer where directory entries are stored
     */
    uint8_t * buf,
    __wasi_pointersize_t buf_len,
    /**
     * The location within the directory to start reading
     */
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Atomically replace a file descriptor by renumbering another file descriptor.
 * Due to the strong focus on thread safety, this environment does not provide
 * a mechanism to duplicate or renumber a file descriptor to an arbitrary
 * number, like `dup2()`. This would be prone to race conditions, as an actual
 * file descriptor with the same number could be allocated by a different
 * thread at the same time.
 * This function provides a way to atomically renumber file descriptors, which
 * would disappear if `dup2()` were to be removed entirely.
 */
__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    /**
     * The file descriptor to overwrite.
     */
    __wasi_fd_t to
) __attribute__((__warn_unused_result__));
/**
 * Atomically duplicate a file handle.
 */
__wasi_errno_t __wasi_fd_dup(
    __wasi_fd_t fd,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Creates a file handle for event notifications
 * 
 */
__wasi_errno_t __wasi_fd_event(
    uint64_t initial_val,
    __wasi_eventfdflags_t flags,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Move the offset of a file descriptor.
 * Note: This is similar to `lseek` in POSIX.
 * @return
 * The new offset of the file descriptor, relative to the start of the file.
 */
__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    /**
     * The number of bytes to move.
     */
    __wasi_filedelta_t offset,
    /**
     * The base from which the offset is relative.
     */
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Synchronize the data and metadata of a file to disk.
 * Note: This is similar to `fsync` in POSIX.
 */
__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
) __attribute__((__warn_unused_result__));
/**
 * Return the current offset of a file descriptor.
 * Note: This is similar to `lseek(fd, 0, SEEK_CUR)` in POSIX.
 * @return
 * The current offset of the file descriptor, relative to the start of the file.
 */
__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Write to a file descriptor.
 * Note: This is similar to `writev` in POSIX.
 */
__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors from which to retrieve data.
     */
    const __wasi_ciovec_t *iovs,
    /**
     * The length of the array pointed to by `iovs`.
     */
    size_t iovs_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Opens a pipe with two file handles
 * 
 * Pipes are bidirectional
 */
__wasi_errno_t __wasi_fd_pipe(
    __wasi_fd_t *retptr0,
    __wasi_fd_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Create a directory.
 * Note: This is similar to `mkdirat` in POSIX.
 */
__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    /**
     * The path at which to create the directory.
     */
    const char *path
) __attribute__((__warn_unused_result__));
/**
 * Return the attributes of a file or directory.
 * Note: This is similar to `stat` in POSIX.
 * @return
 * The buffer where the file's attributes are stored.
 */
__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t flags,
    /**
     * The path of the file or directory to inspect.
     */
    const char *path,
    __wasi_filestat_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Adjust the timestamps of a file or directory.
 * Note: This is similar to `utimensat` in POSIX.
 */
__wasi_errno_t __wasi_path_filestat_set_times(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t flags,
    /**
     * The path of the file or directory to operate on.
     */
    const char *path,
    /**
     * The desired values of the data access timestamp.
     */
    __wasi_timestamp_t atim,
    /**
     * The desired values of the data modification timestamp.
     */
    __wasi_timestamp_t mtim,
    /**
     * A bitmask indicating which timestamps to adjust.
     */
    __wasi_fstflags_t fst_flags
) __attribute__((__warn_unused_result__));
/**
 * Create a hard link.
 * Note: This is similar to `linkat` in POSIX.
 */
__wasi_errno_t __wasi_path_link(
    __wasi_fd_t old_fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t old_flags,
    /**
     * The source path from which to link.
     */
    const char *old_path,
    /**
     * The working directory at which the resolution of the new path starts.
     */
    __wasi_fd_t new_fd,
    /**
     * The destination path at which to create the hard link.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));
/**
 * Open a file or directory.
 * The returned file descriptor is not guaranteed to be the lowest-numbered
 * file descriptor not currently open; it is randomized to prevent
 * applications from depending on making assumptions about indexes, since this
 * is error-prone in multi-threaded contexts. The returned file descriptor is
 * guaranteed to be less than 2**31.
 * Note: This is similar to `openat` in POSIX.
 * @return
 * The file descriptor of the file that has been opened.
 */
__wasi_errno_t __wasi_path_open(
    __wasi_fd_t fd,
    /**
     * Flags determining the method of how the path is resolved.
     */
    __wasi_lookupflags_t dirflags,
    /**
     * The relative path of the file or directory to open, relative to the
     * `path_open::fd` directory.
     */
    const char *path,
    /**
     * The method by which to open the file.
     */
    __wasi_oflags_t oflags,
    /**
     * The initial rights of the newly created file descriptor. The
     * implementation is allowed to return a file descriptor with fewer rights
     * than specified, if and only if those rights do not apply to the type of
     * file being opened.
     * The *base* rights are rights that will apply to operations using the file
     * descriptor itself, while the *inheriting* rights are rights that apply to
     * file descriptors derived from it.
     */
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting,
    __wasi_fdflags_t fdflags,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Read the contents of a symbolic link.
 * Note: This is similar to `readlinkat` in POSIX.
 * @return
 * The number of bytes placed in the buffer.
 */
__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    /**
     * The path of the symbolic link from which to read.
     */
    const char *path,
    /**
     * The buffer to which to write the contents of the symbolic link.
     */
    uint8_t * buf,
    __wasi_pointersize_t buf_len,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Remove a directory.
 * Return `errno::notempty` if the directory is not empty.
 * Note: This is similar to `unlinkat(fd, path, AT_REMOVEDIR)` in POSIX.
 */
__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    /**
     * The path to a directory to remove.
     */
    const char *path
) __attribute__((__warn_unused_result__));
/**
 * Rename a file or directory.
 * Note: This is similar to `renameat` in POSIX.
 */
__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    /**
     * The source path of the file or directory to rename.
     */
    const char *old_path,
    /**
     * The working directory at which the resolution of the new path starts.
     */
    __wasi_fd_t new_fd,
    /**
     * The destination path to which to rename the file or directory.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));
/**
 * Create a symbolic link.
 * Note: This is similar to `symlinkat` in POSIX.
 */
__wasi_errno_t __wasi_path_symlink(
    /**
     * The contents of the symbolic link.
     */
    const char *old_path,
    __wasi_fd_t fd,
    /**
     * The destination path at which to create the symbolic link.
     */
    const char *new_path
) __attribute__((__warn_unused_result__));
/**
 * Unlink a file.
 * Return `errno::isdir` if the path refers to a directory.
 * Note: This is similar to `unlinkat(fd, path, 0)` in POSIX.
 */
__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    /**
     * The path to a file to unlink.
     */
    const char *path
) __attribute__((__warn_unused_result__));
/**
 * Concurrently poll for the occurrence of a set of events.
 * @return
 * The number of events stored.
 */
__wasi_errno_t __wasi_poll_oneoff(
    /**
     * The events to which to subscribe.
     */
    const __wasi_subscription_t * in,
    /**
     * The events that have occurred.
     */
    __wasi_event_t * out,
    /**
     * Both the number of subscriptions and events.
     */
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Temporarily yield execution of the calling thread.
 * Note: This is similar to `sched_yield` in POSIX.
 */
__wasi_errno_t __wasi_sched_yield(
    void
) __attribute__((__warn_unused_result__));
/**
 * Write high-quality random data into a buffer.
 * This function blocks when the implementation is unable to immediately
 * provide sufficient high-quality random data.
 * This function may execute slowly, so when large mounts of random data are
 * required, it's advisable to use this function to seed a pseudo-random
 * number generator, rather than to provide the random data directly.
 */
__wasi_errno_t __wasi_random_get(
    /**
     * The buffer to fill with random data.
     */
    uint8_t * buf,
    __wasi_pointersize_t buf_len
) __attribute__((__warn_unused_result__));
/**
 * Retrieves the current state of the TTY
 */
__wasi_errno_t __wasi_tty_get(
    __wasi_tty_t * state
) __attribute__((__warn_unused_result__));
/**
 * Updates the properties of the rect
 */
__wasi_errno_t __wasi_tty_set(
    __wasi_tty_t * state
) __attribute__((__warn_unused_result__));
/**
 * Returns the current working directory
 * If the path exceeds the size of the buffer then this function
 * will fill the path_len with the needed size and return EOVERFLOW
 */
__wasi_errno_t __wasi_getcwd(
    /**
     * The buffer where current directory is stored
     */
    uint8_t * path,
    __wasi_pointersize_t * path_len
) __attribute__((__warn_unused_result__));
/**
 * Sets the current working directory
 */
__wasi_errno_t __wasi_chdir(
    /**
     * Path to change the current working directory to
     */
    const char *path
) __attribute__((__warn_unused_result__));
/**
 * Registers a callback function for signals
 */
void __wasi_callback_signal(
    /**
     * Exported function that will be called back when the signal triggers
     * (must match the callback signature that takes the signal value)
     * (if this is not specified the default will be "_signal")
     */
    const char *callback
);
/**
 * Registers a callback function for new threads
 */
void __wasi_callback_thread(
    /**
     * Exported function that will be called back when a new thread is created
     * (must match the callback signature that takes the u64 user_data)
     * (if this is not specified the default will be "_start_thread")
     */
    const char *callback
);
/**
 * Registers a callback function for reactors
 */
void __wasi_callback_reactor(
    /**
     * Exported function that will be called back when the reactor is triggered
     * (must match the callback signature that takes the u64 user_data)
     * (if this is not specified the default will be "_reactor")
     */
    const char *callback
);
/**
 * Registers a callback function for destruction of thread locals
 */
void __wasi_callback_thread_local_destroy(
    /**
     * Exported function that will be called back when the reactor is triggered
     * (must match the callback signature that takes the u64 user_data)
     * (if this is not specified the default will be "_thread_local_destroy")
     */
    const char *callback
);
/**
 * Creates a new thread by spawning that shares the same
 * memory address space, file handles and main event loops.
 * The web assembly process must export function named '_start_thread'
 * @return
 * Returns the thread index of the newly created thread
 * (indices always start from zero)
 */
__wasi_errno_t __wasi_thread_spawn(
    /**
     * User data that will be supplied to the function when its called
     */
    uint64_t user_data,
    /**
     * The base address of the stack allocated for this thread
     */
    uint64_t stack_base,
    /**
     * The start address of the stack (where the memory is allocated)
     */
    uint64_t stack_start,
    /**
     * Indicates if the function will operate as a reactor or
     * as a normal thread. Reactors will be repeatable called
     * whenever IO work is available to be processed.
     */
    __wasi_bool_t reactor,
    __wasi_tid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sends the current thread to sleep for a period of time
 */
__wasi_errno_t __wasi_thread_sleep(
    /**
     * Amount of time that the thread should sleep
     */
    __wasi_timestamp_t duration
) __attribute__((__warn_unused_result__));
/**
 * Returns the index of the current thread
 * (threads indices are sequencial from zero)
 */
__wasi_errno_t __wasi_thread_id(
    __wasi_tid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Create a thread local variable
 * If The web assembly process exports function named '_thread_local_destroy'
 * then it will be invoked when the thread goes out of scope and dies.
 */
__wasi_errno_t __wasi_thread_local_create(
    /**
     * User data that will be passed to the destructor
     * when the thread variable goes out of scope
     */
    uint64_t user_data,
    __wasi_tl_key_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Destroys a thread local variable
 */
__wasi_errno_t __wasi_thread_local_destroy(
    /**
     * Thread key that was previously created
     */
    __wasi_tl_key_t key
) __attribute__((__warn_unused_result__));
/**
 * Sets the value of a thread local variable
 */
__wasi_errno_t __wasi_thread_local_set(
    /**
     * Thread key that this local variable will be associated with
     */
    __wasi_tl_key_t key,
    /**
     * Value to be set for the thread local variable
     */
    __wasi_tl_val_t val
) __attribute__((__warn_unused_result__));
/**
 * Gets the value of a thread local variable
 */
__wasi_errno_t __wasi_thread_local_get(
    /**
     * Thread key that this local variable that was previous set
     */
    __wasi_tl_key_t key,
    __wasi_tl_val_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Joins this thread with another thread, blocking this
 * one until the other finishes
 */
__wasi_errno_t __wasi_thread_join(
    /**
     * Handle of the thread to wait on
     */
    __wasi_tid_t tid
) __attribute__((__warn_unused_result__));
/**
 * Returns the available parallelism which is normally the
 * number of available cores that can run concurrently
 */
__wasi_errno_t __wasi_thread_parallelism(
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sends a signal to a thread
 */
__wasi_errno_t __wasi_thread_signal(
    /**
     * Handle of the thread to send a singal
     */
    __wasi_tid_t tid,
    /**
     * Signal to send to the thread
     */
    __wasi_signal_t signal
) __attribute__((__warn_unused_result__));
/**
 * Wait for a futex_wake operation to wake us.
 * Returns with EINVAL if the futex doesn't hold the expected value.
 * Returns false on timeout, and true in all other cases.
 */
__wasi_errno_t __wasi_futex_wait(
    /**
     * Memory location that holds the value that will be checked
     */
    uint32_t * futex,
    /**
     * Expected value that should be currently held at the memory location
     */
    uint32_t expected,
    /**
     * Timeout should the futex not be triggered in the allocated time
     */
    const __wasi_option_timestamp_t * timeout,
    __wasi_bool_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Wake up one thread that's blocked on futex_wait on this futex.
 * Returns true if this actually woke up such a thread,
 * or false if no thread was waiting on this futex.
 */
__wasi_errno_t __wasi_futex_wake(
    /**
     * Memory location that holds a futex that others may be waiting on
     */
    uint32_t * futex,
    __wasi_bool_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Wake up all threads that are waiting on futex_wait on this futex.
 */
__wasi_errno_t __wasi_futex_wake_all(
    /**
     * Memory location that holds a futex that others may be waiting on
     */
    uint32_t * futex,
    __wasi_bool_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Terminates the current running thread, if this is the last thread then
 * the process will also exit with the specified exit code. An exit code
 * of 0 indicates successful termination of the thread. The meanings of
 * other values is dependent on the environment.
 */
_Noreturn void __wasi_thread_exit(
    /**
     * The exit code returned by the process.
     */
    __wasi_exitcode_t rval
);
/**
 * Creates a checkpoint of the current stack which allows it to be restored
 * later using its stack hash. The value supplied will be returned upon
 * restoration (and hence must be none zero) - zero will be returned when
 * the stack is first recorded.
 * This function will read the __stack_pointer global
 * @return
 * Returns zero upon registration and the value when restored
 */
__wasi_errno_t __wasi_stack_checkpoint(
    /**
     * Reference to the stack snapshot that will be filled
     */
    __wasi_stack_snapshot_t * snapshot,
    __wasi_longsize_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Restores the current stack to a previous stack described by a supplied
 * stack snapshot.
 * This function will manipulate the __stack_pointer global
 */
_Noreturn void __wasi_stack_restore(
    /**
     * Reference to the stack snapshot that will be restored
     */
    const __wasi_stack_snapshot_t * snapshot,
    /**
     * Value to be returned when the stack is restored
     * (if zero this will change to one)
     */
    __wasi_longsize_t val
);
/**
 * Terminate the process normally. An exit code of 0 indicates successful
 * termination of the program. The meanings of other values is dependent on
 * the environment.
 */
_Noreturn void __wasi_proc_exit(
    /**
     * The exit code returned by the process.
     */
    __wasi_exitcode_t rval
);
/**
 * Send a signal to the process of the calling thread.
 * Note: This is similar to `raise` in POSIX.
 */
__wasi_errno_t __wasi_proc_raise(
    /**
     * The signal condition to trigger.
     */
    __wasi_signal_t sig
) __attribute__((__warn_unused_result__));
/**
 * Send a signal to the process of the calling thread on a regular basis
 * Note: This is similar to `setitimer` in POSIX.
 */
__wasi_errno_t __wasi_proc_raise_interval(
    /**
     * The signal condition to trigger.
     */
    __wasi_signal_t sig,
    /**
     * Time to wait before raising the signal
     * (zero here indicates the signal interval is cancelled)
     */
    __wasi_timestamp_t interval,
    /**
     * Flag that indicates if the signal will trigger indefinately
     */
    __wasi_bool_t repeat
) __attribute__((__warn_unused_result__));
/**
 * Forks the current process into a new subprocess. If the function
 * returns a zero then its the new subprocess. If it returns a positive
 * number then its the current process and the $pid represents the child.
 */
__wasi_errno_t __wasi_proc_fork(
    /**
     * Indicates if the memory will be copied into the new process
     * (if it is not copied this then becomes similar to `vfork` in
     *  that the current process pauses until `proc_exec` is called)
     */
    __wasi_bool_t copy_memory,
    __wasi_pid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * execve()  executes  the  program  referred to by pathname.  This causes the
 * program that is currently being run by the calling process to  be  replaced
 * with  a  new  program, with newly initialized stack, heap, and (initialized
 * and uninitialized) data segments
 * 
 * If the named process does not exist then the process will fail and terminate
 */
_Noreturn void __wasi_proc_exec(
    /**
     * Name of the process to be spawned
     */
    const char *name,
    /**
     * List of the arguments to pass the process
     * (entries are separated by line feeds)
     */
    const char *args
);
/**
 * Spawns a new process within the context of this machine
 * @return
 * Returns a bus process id that can be used to invoke calls
 */
__wasi_bus_error_t __wasi_proc_spawn(
    /**
     * Name of the process to be spawned
     */
    const char *name,
    /**
     * Indicates if the process will chroot or not
     */
    __wasi_bool_t chroot,
    /**
     * List of the arguments to pass the process
     * (entries are separated by line feeds)
     */
    const char *args,
    /**
     * List of the preopens for this process
     * (entries are separated by line feeds)
     */
    const char *preopen,
    /**
     * How will stdin be handled
     */
    __wasi_stdio_mode_t stdin,
    /**
     * How will stdout be handled
     */
    __wasi_stdio_mode_t stdout,
    /**
     * How will stderr be handled
     */
    __wasi_stdio_mode_t stderr,
    /**
     * Working directory where this process should run
     * (passing '.' will use the current directory)
     */
    const char *working_dir,
    __wasi_process_handles_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Returns the handle of the current process
 */
__wasi_errno_t __wasi_proc_id(
    __wasi_pid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Returns the parent handle of a particular process
 */
__wasi_errno_t __wasi_proc_parent(
    /**
     * Handle of the process to get the parent handle for
     */
    __wasi_pid_t pid,
    __wasi_pid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Wait for process to exit
 * @return
 * Returns the exit code of the process
 */
__wasi_errno_t __wasi_proc_join(
    /**
     * ID of the process to wait on
     */
    __wasi_pid_t * pid,
    __wasi_exitcode_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sends a signal to a process
 */
__wasi_errno_t __wasi_proc_signal(
    /**
     * ID of the process to send a singal
     */
    __wasi_pid_t pid,
    /**
     * Signal to send to the thread
     */
    __wasi_signal_t signal
) __attribute__((__warn_unused_result__));
/**
 * Spawns a new bus process for a particular web WebAssembly
 * binary that is referenced by its process name.
 * @return
 * Returns a bus process id that can be used to invoke calls
 */
__wasi_bus_error_t __wasi_bus_open_local(
    /**
     * Name of the process to be spawned
     */
    const char *name,
    /**
     * Indicates if the existing processes should be reused
     * if they are already running
     */
    __wasi_bool_t reuse,
    __wasi_bid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Spawns a new bus process for a particular web WebAssembly
 * binary that is referenced by its process name on a remote instance.
 * @return
 * Returns a bus process id that can be used to invoke calls
 */
__wasi_bus_error_t __wasi_bus_open_remote(
    /**
     * Name of the process to be spawned
     */
    const char *name,
    /**
     * Indicates if the existing processes should be reused
     * if they are already running
     */
    __wasi_bool_t reuse,
    /**
     * Instance identifier where this process will be spawned
     */
    const char *instance,
    /**
     * Acceess token used to authenticate with the instance
     */
    const char *token,
    __wasi_bid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Closes a bus process and releases all associated resources
 */
__wasi_bus_error_t __wasi_bus_close(
    /**
     * Handle of the bus process handle to be closed
     */
    __wasi_bid_t bid
) __attribute__((__warn_unused_result__));
/**
 * Invokes a call within a running bus process.
 */
__wasi_bus_error_t __wasi_bus_call(
    /**
     * Handle of the bus process to invoke the call within
     */
    __wasi_bid_t bid,
    /**
     * Topic hash that describes the type of call to made
     */
    const __wasi_hash_t * topic_hash,
    /**
     * Format of the data pushed onto the bus
     */
    __wasi_bus_data_format_t format,
    /**
     * The buffer where data to be transmitted is stored
     */
    const uint8_t *buf,
    /**
     * The length of the array pointed to by `buf`.
     */
    size_t buf_len,
    __wasi_cid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Invokes a call within the context of another call
 */
__wasi_bus_error_t __wasi_bus_subcall(
    /**
     * Parent bus call that this is related to
     */
    __wasi_cid_t parent,
    /**
     * Topic hash that describes the type of call to made
     */
    const __wasi_hash_t * topic_hash,
    /**
     * Format of the data pushed onto the bus
     */
    __wasi_bus_data_format_t format,
    /**
     * The buffer where data to be transmitted is stored
     */
    const uint8_t *buf,
    /**
     * The length of the array pointed to by `buf`.
     */
    size_t buf_len,
    __wasi_cid_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Polls for any outstanding events from a particular
 * bus process by its handle
 * @return
 * Returns the number of events that have occured
 */
__wasi_bus_error_t __wasi_bus_poll(
    /**
     * Timeout before the poll returns, if one passed 0
     * as the timeout then this call is non blocking.
     */
    __wasi_timestamp_t timeout,
    /**
     * An events buffer that will hold any received bus events
     */
    __wasi_bus_event_t * events,
    __wasi_size_t nevents,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Replies to a call that was made to this process
 * from another process; where 'cid' is the call context.
 * This will may also drop the handle and release any
 * associated resources (if keepalive is not set)
 */
__wasi_bus_error_t __wasi_call_reply(
    /**
     * Handle of the call to send a reply on
     */
    __wasi_cid_t cid,
    /**
     * Format of the data pushed onto the bus
     */
    __wasi_bus_data_format_t format,
    /**
     * The buffer where data to be transmitted is stored
     */
    const uint8_t *buf,
    /**
     * The length of the array pointed to by `buf`.
     */
    size_t buf_len
) __attribute__((__warn_unused_result__));
/**
 * Causes a fault on a particular call that was made
 * to this process from another process; where 'bid'
 * is the callering process context.
 */
void __wasi_call_fault(
    /**
     * Handle of the call to raise a fault on
     */
    __wasi_cid_t cid,
    /**
     * Fault to be raised on the bus
     */
    __wasi_bus_error_t fault
);
/**
 * Closes a bus call based on its bus call handle
 */
void __wasi_call_close(
    /**
     * Handle of the bus call handle to be dropped
     */
    __wasi_cid_t cid
);
/**
 * Connects to a websocket at a particular network URL
 * @return
 * Returns a socket handle which is used to send and receive data
 */
__wasi_errno_t __wasi_ws_connect(
    /**
     * URL of the web socket destination to connect to
     */
    const char *url,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Makes a HTTP request to a remote web resource and
 * returns a socket handles that are used to send and receive data
 * @return
 * The body of the response can be streamed from the returned
 * file handle
 */
__wasi_errno_t __wasi_http_request(
    /**
     * URL of the HTTP resource to connect to
     */
    const char *url,
    /**
     * HTTP method to be invoked
     */
    const char *method,
    /**
     * HTTP headers to attach to the request
     * (headers seperated by lines)
     */
    const char *headers,
    /**
     * Should the request body be compressed
     */
    __wasi_bool_t gzip,
    __wasi_http_handles_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Retrieves the status of a HTTP request
 */
__wasi_errno_t __wasi_http_status(
    /**
     * Handle of the HTTP request
     */
    __wasi_fd_t fd,
    /**
     * Pointer to a buffer that will be filled with the current
     * status of this HTTP request
     */
    __wasi_http_status_t * status
) __attribute__((__warn_unused_result__));
/**
 * Securely connects to a particular remote network
 */
__wasi_errno_t __wasi_port_bridge(
    /**
     * Fully qualified identifier for the network
     */
    const char *network,
    /**
     * Access token used to authenticate with the network
     */
    const char *token,
    /**
     * Level of encryption to encapsulate the network connection with
     */
    __wasi_stream_security_t security
) __attribute__((__warn_unused_result__));
/**
 * Disconnects from a remote network
 */
__wasi_errno_t __wasi_port_unbridge(
    void
) __attribute__((__warn_unused_result__));
/**
 * Acquires a set of addresses using DHCP
 */
__wasi_errno_t __wasi_port_dhcp_acquire(
    void
) __attribute__((__warn_unused_result__));
/**
 * Adds another static address to the local port
 */
__wasi_errno_t __wasi_port_addr_add(
    /**
     * Address to be added
     */
    const __wasi_addr_cidr_t * addr
) __attribute__((__warn_unused_result__));
/**
 * Removes an address from the local port
 */
__wasi_errno_t __wasi_port_addr_remove(
    /**
     * Address to be removed
     */
    const __wasi_addr_t * addr
) __attribute__((__warn_unused_result__));
/**
 * Clears all the addresses on the local port
 */
__wasi_errno_t __wasi_port_addr_clear(
    void
) __attribute__((__warn_unused_result__));
/**
 * Returns the MAC address of the local port
 */
__wasi_errno_t __wasi_port_mac(
    __wasi_hardware_address_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Returns a list of all the addresses owned by the local port
 * This function fills the output buffer as much as possible.
 * If the buffer is not big enough then the naddrs address will be
 * filled with the buffer size needed and the EOVERFLOW will be returned
 * @return
 * The number of addresses returned.
 */
__wasi_errno_t __wasi_port_addr_list(
    /**
     * The buffer where addresses will be stored
     */
    __wasi_addr_cidr_t * addrs,
    __wasi_size_t * naddrs
) __attribute__((__warn_unused_result__));
/**
 * Adds a default gateway to the port
 */
__wasi_errno_t __wasi_port_gateway_set(
    /**
     * Address of the default gateway
     */
    const __wasi_addr_t * addr
) __attribute__((__warn_unused_result__));
/**
 * Adds a new route to the local port
 */
__wasi_errno_t __wasi_port_route_add(
    const __wasi_addr_cidr_t * cidr,
    const __wasi_addr_t * via_router,
    const __wasi_option_timestamp_t * preferred_until,
    const __wasi_option_timestamp_t * expires_at
) __attribute__((__warn_unused_result__));
/**
 * Removes an existing route from the local port
 */
__wasi_errno_t __wasi_port_route_remove(
    const __wasi_addr_t * cidr
) __attribute__((__warn_unused_result__));
/**
 * Clears all the routes in the local port
 */
__wasi_errno_t __wasi_port_route_clear(
    void
) __attribute__((__warn_unused_result__));
/**
 * Returns a list of all the routes owned by the local port
 * This function fills the output buffer as much as possible.
 * If the buffer is too small this will return EOVERFLOW and
 * fill nroutes with the size of the buffer needed.
 */
__wasi_errno_t __wasi_port_route_list(
    /**
     * The buffer where routes will be stored
     */
    __wasi_route_t * routes,
    __wasi_size_t * nroutes
) __attribute__((__warn_unused_result__));
/**
 * Shut down socket send and receive channels.
 * Note: This is similar to `shutdown` in POSIX.
 */
__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    /**
     * Which channels on the socket to shut down.
     */
    __wasi_sdflags_t how
) __attribute__((__warn_unused_result__));
/**
 * Returns the current status of a socket
 */
__wasi_errno_t __wasi_sock_status(
    __wasi_fd_t fd,
    __wasi_sock_status_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Returns the local address to which the socket is bound.
 * 
 * Note: This is similar to `getsockname` in POSIX
 * 
 * When successful, the contents of the output buffer consist of an IP address,
 * either IP4 or IP6.
 */
__wasi_errno_t __wasi_sock_addr_local(
    /**
     * Socket that the address is bound to
     */
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Returns the remote address to which the socket is connected to.
 * 
 * Note: This is similar to `getpeername` in POSIX
 * 
 * When successful, the contents of the output buffer consist of an IP address,
 * either IP4 or IP6.
 */
__wasi_errno_t __wasi_sock_addr_peer(
    /**
     * Socket that the address is bound to
     */
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Create an endpoint for communication.
 * 
 * creates an endpoint for communication and returns a file descriptor
 * tor that refers to that endpoint. The file descriptor returned by a successful
 * call will be the lowest-numbered file descriptor not currently open
 * for the process.
 * 
 * Note: This is similar to `socket` in POSIX using PF_INET
 * @return
 * The file descriptor of the socket that has been opened.
 */
__wasi_errno_t __wasi_sock_open(
    /**
     * Address family
     */
    __wasi_address_family_t af,
    /**
     * Socket type, either datagram or stream
     */
    __wasi_sock_type_t socktype,
    /**
     * Socket protocol
     */
    __wasi_sock_proto_t sock_proto,
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sets a particular socket setting
 * Note: This is similar to `setsockopt` in POSIX for SO_REUSEADDR
 */
__wasi_errno_t __wasi_sock_set_opt_flag(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be set
     */
    __wasi_sock_option_t sockopt,
    /**
     * Value to set the option to
     */
    __wasi_bool_t flag
) __attribute__((__warn_unused_result__));
/**
 * Retrieve status of particular socket seting
 * Note: This is similar to `getsockopt` in POSIX for SO_REUSEADDR
 */
__wasi_errno_t __wasi_sock_get_opt_flag(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be retrieved
     */
    __wasi_sock_option_t sockopt,
    __wasi_bool_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sets one of the times the socket
 */
__wasi_errno_t __wasi_sock_set_opt_time(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be set
     */
    __wasi_sock_option_t sockopt,
    /**
     * Value to set the time to
     */
    const __wasi_option_timestamp_t * timeout
) __attribute__((__warn_unused_result__));
/**
 * Retrieve one of the times on the socket
 */
__wasi_errno_t __wasi_sock_get_opt_time(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be retrieved
     */
    __wasi_sock_option_t sockopt,
    __wasi_option_timestamp_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Set size of particular option for this socket
 * Note: This is similar to `setsockopt` in POSIX for SO_RCVBUF
 */
__wasi_errno_t __wasi_sock_set_opt_size(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be set
     */
    __wasi_sock_option_t sockopt,
    /**
     * Buffer size
     */
    __wasi_filesize_t size
) __attribute__((__warn_unused_result__));
/**
 * Retrieve the size of particular option for this socket
 * Note: This is similar to `getsockopt` in POSIX for SO_RCVBUF
 */
__wasi_errno_t __wasi_sock_get_opt_size(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Socket option to be retrieved
     */
    __wasi_sock_option_t sockopt,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Joins a particular multicast IPv4 group
 */
__wasi_errno_t __wasi_sock_join_multicast_v4(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Multicast group to joined
     */
    const __wasi_addr_ip4_t * multiaddr,
    /**
     * Interface that will join
     */
    const __wasi_addr_ip4_t * interface
) __attribute__((__warn_unused_result__));
/**
 * Leaves a particular multicast IPv4 group
 */
__wasi_errno_t __wasi_sock_leave_multicast_v4(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Multicast group to leave
     */
    const __wasi_addr_ip4_t * multiaddr,
    /**
     * Interface that will left
     */
    const __wasi_addr_ip4_t * interface
) __attribute__((__warn_unused_result__));
/**
 * Joins a particular multicast IPv6 group
 */
__wasi_errno_t __wasi_sock_join_multicast_v6(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Multicast group to joined
     */
    const __wasi_addr_ip6_t * multiaddr,
    /**
     * Interface that will join
     */
    uint32_t interface
) __attribute__((__warn_unused_result__));
/**
 * Leaves a particular multicast IPv6 group
 */
__wasi_errno_t __wasi_sock_leave_multicast_v6(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Multicast group to leave
     */
    const __wasi_addr_ip6_t * multiaddr,
    /**
     * Interface that will left
     */
    uint32_t interface
) __attribute__((__warn_unused_result__));
/**
 * Bind a socket
 * Note: This is similar to `bind` in POSIX using PF_INET
 */
__wasi_errno_t __wasi_sock_bind(
    /**
     * File descriptor of the socket to be bind
     */
    __wasi_fd_t fd,
    /**
     * Address to bind the socket to
     */
    const __wasi_addr_port_t * addr
) __attribute__((__warn_unused_result__));
/**
 * Listen for connections on a socket
 * 
 * Polling the socket handle will wait until a connection
 * attempt is made
 * 
 * Note: This is similar to `listen`
 */
__wasi_errno_t __wasi_sock_listen(
    /**
     * File descriptor of the socket to be bind
     */
    __wasi_fd_t fd,
    /**
     * Maximum size of the queue for pending connections
     */
    __wasi_size_t backlog
) __attribute__((__warn_unused_result__));
/**
 * Accept a new incoming connection.
 * Note: This is similar to `accept` in POSIX.
 * @return
 * New socket connection
 */
__wasi_errno_t __wasi_sock_accept(
    /**
     * The listening socket.
     */
    __wasi_fd_t fd,
    /**
     * The desired values of the file descriptor flags.
     */
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0,
    __wasi_addr_port_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Initiate a connection on a socket to the specified address
 * 
 * Polling the socket handle will wait for data to arrive or for
 * the socket status to change which can be queried via 'sock_status'
 * 
 * Note: This is similar to `connect` in POSIX
 */
__wasi_errno_t __wasi_sock_connect(
    /**
     * Socket descriptor
     */
    __wasi_fd_t fd,
    /**
     * Address of the socket to connect to
     */
    const __wasi_addr_port_t * addr
) __attribute__((__warn_unused_result__));
/**
 * Receive a message from a socket.
 * Note: This is similar to `recv` in POSIX, though it also supports reading
 * the data into multiple buffers in the manner of `readv`.
 * @return
 * Number of bytes stored in ri_data and message flags.
 */
__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to store data.
     */
    const __wasi_iovec_t *ri_data,
    /**
     * The length of the array pointed to by `ri_data`.
     */
    size_t ri_data_len,
    /**
     * Message flags.
     */
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Receive a message and its peer address from a socket.
 * Note: This is similar to `recvfrom` in POSIX, though it also supports reading
 * the data into multiple buffers in the manner of `readv`.
 * @return
 * Number of bytes stored in ri_data and message flags.
 */
__wasi_errno_t __wasi_sock_recv_from(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to store data.
     */
    const __wasi_iovec_t *ri_data,
    /**
     * The length of the array pointed to by `ri_data`.
     */
    size_t ri_data_len,
    /**
     * Message flags.
     */
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1,
    __wasi_addr_port_t *retptr2
) __attribute__((__warn_unused_result__));
/**
 * Send a message on a socket.
 * Note: This is similar to `send` in POSIX, though it also supports writing
 * the data from multiple buffers in the manner of `writev`.
 * @return
 * Number of bytes transmitted.
 */
__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to retrieve data
     */
    const __wasi_ciovec_t *si_data,
    /**
     * The length of the array pointed to by `si_data`.
     */
    size_t si_data_len,
    /**
     * Message flags.
     */
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Send a message on a socket to a specific address.
 * Note: This is similar to `sendto` in POSIX, though it also supports writing
 * the data from multiple buffers in the manner of `writev`.
 * @return
 * Number of bytes transmitted.
 */
__wasi_errno_t __wasi_sock_send_to(
    __wasi_fd_t fd,
    /**
     * List of scatter/gather vectors to which to retrieve data
     */
    const __wasi_ciovec_t *si_data,
    /**
     * The length of the array pointed to by `si_data`.
     */
    size_t si_data_len,
    /**
     * Message flags.
     */
    __wasi_siflags_t si_flags,
    /**
     * Address of the socket to send message to
     */
    const __wasi_addr_port_t * addr,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sends the entire contents of a file down a socket
 * @return
 * Number of bytes transmitted.
 */
__wasi_errno_t __wasi_sock_send_file(
    __wasi_fd_t out_fd,
    /**
     * Open file that has the data to be transmitted
     */
    __wasi_fd_t in_fd,
    /**
     * Offset into the file to start reading at
     */
    __wasi_filesize_t offset,
    /**
     * Number of bytes to be sent
     */
    __wasi_filesize_t count,
    __wasi_filesize_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Resolves a hostname and a port to one or more IP addresses.
 * 
 * Note: This is similar to `getaddrinfo` in POSIX
 * 
 * When successful, the contents of the output buffer consist of a sequence of
 * IPv4 and/or IPv6 addresses. Each address entry consists of a addr_t object.
 * This function fills the output buffer as much as possible.
 * @return
 * The number of IP addresses returned during the DNS resolution.
 */
__wasi_errno_t __wasi_resolve(
    /**
     * Host to resolve
     */
    const char *host,
    /**
     * Port hint (zero if no hint is supplied)
     */
    uint16_t port,
    /**
     * The buffer where addresses will be stored
     */
    __wasi_addr_t * addrs,
    __wasi_size_t naddrs,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/** @} */

#ifdef __cplusplus
}
#endif

#endif
