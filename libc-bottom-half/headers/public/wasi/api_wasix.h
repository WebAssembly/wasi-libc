/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   wasix_v1.witx
 *
 * To regenerate this file execute:
 *
 *     cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
 */

#ifndef __wasix__
#define __wasix__ 1
#endif

#ifndef __wasix_api_h
#define __wasix_api_h

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
_Static_assert(_Alignof(intptr_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(uintptr_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(void*) == 4, "non-wasi data layout");
typedef int32_t __wasi_int_t;
typedef uint32_t __wasi_uint_t;
_Static_assert(_Alignof(__wasi_int_t) == 4, "non-wasi data layout");
_Static_assert(_Alignof(__wasi_uint_t) == 4, "non-wasi data layout");
#ifdef __cplusplus
extern "C" {
#endif

// TODO: Encoding this in witx.
#define __WASI_DIRCOOKIE_START (UINT64_C(0))
/**
 * Represents the length of a piece of data referenced by a pointer
 */
typedef __wasi_uint_t __wasi_pointersize_t;

_Static_assert(sizeof(__wasi_pointersize_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_pointersize_t) == 4, "witx calculated align");

/**
 * Represents an ID of a waker to be woken
 */
typedef uint64_t __wasi_waker_t;

_Static_assert(sizeof(__wasi_waker_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_waker_t) == 8, "witx calculated align");

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

/**
 * Memory access violation.
 */
#define __WASI_ERRNO_MEMVIOLATION (UINT16_C(78))

/**
 * Unknown error has occurred.
 */
#define __WASI_ERRNO_UNKNOWN (UINT16_C(79))

/**
 * Pending asynchronous wake.
 */
#define __WASI_ERRNO_PENDING (UINT16_C(80))

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
 * Stkflt
 * Action: Ignored.
 */
#define __WASI_SIGNAL_STKFLT (UINT8_C(16))

/**
 * Child process terminated, stopped, or continued.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_CHLD (UINT8_C(17))

/**
 * Continue executing, if stopped.
 * Action: Continues executing, if stopped.
 */
#define __WASI_SIGNAL_CONT (UINT8_C(18))

/**
 * Stop executing.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_STOP (UINT8_C(19))

/**
 * Terminal stop signal.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TSTP (UINT8_C(20))

/**
 * Background process attempting read.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TTIN (UINT8_C(21))

/**
 * Background process attempting write.
 * Action: Stops executing.
 */
#define __WASI_SIGNAL_TTOU (UINT8_C(22))

/**
 * High bandwidth data is available at a socket.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_URG (UINT8_C(23))

/**
 * CPU time limit exceeded.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_XCPU (UINT8_C(24))

/**
 * File size limit exceeded.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_XFSZ (UINT8_C(25))

/**
 * Virtual timer expired.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_VTALRM (UINT8_C(26))

/**
 * Profiling timer expired.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_PROF (UINT8_C(27))

/**
 * Window changed.
 * Action: Ignored.
 */
#define __WASI_SIGNAL_WINCH (UINT8_C(28))

/**
 * I/O possible.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_POLL (UINT8_C(29))

/**
 * Power failure.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_PWR (UINT8_C(30))

/**
 * Bad system call.
 * Action: Terminates the process.
 */
#define __WASI_SIGNAL_SYS (UINT8_C(31))

_Static_assert(sizeof(__wasi_signal_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_signal_t) == 1, "witx calculated align");

/**
 * A region of memory for scatter/gather reads.
 */
/**
*** Hidden type
typedef struct __wasi_iovec_t {
    uint8_t * buf;

    __wasi_pointersize_t buf_len;

} __wasi_iovec_t;

_Static_assert(sizeof(__wasi_iovec_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_iovec_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_iovec_t, buf) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_iovec_t, buf_len) == 4, "witx calculated offset");

 */
/**
 * A region of memory for scatter/gather writes.
 */
/**
*** Hidden type
typedef struct __wasi_ciovec_t {
    const uint8_t * buf;

    __wasi_pointersize_t buf_len;

} __wasi_ciovec_t;

_Static_assert(sizeof(__wasi_ciovec_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_ciovec_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_ciovec_t, buf) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_ciovec_t, buf_len) == 4, "witx calculated offset");

 */
/**
*** Hidden type
typedef uint32_t __wasi_size_t;

_Static_assert(sizeof(__wasi_size_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_size_t) == 4, "witx calculated align");

 */
/**
*** Hidden type
typedef uint64_t __wasi_filesize_t;

_Static_assert(sizeof(__wasi_filesize_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_filesize_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint64_t __wasi_timestamp_t;

_Static_assert(sizeof(__wasi_timestamp_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_timestamp_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint32_t __wasi_clockid_t;

#define __WASI_CLOCKID_HIDDEN (UINT32_C(0))

_Static_assert(sizeof(__wasi_clockid_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_clockid_t) == 4, "witx calculated align");

 */
/**
*** Hidden type
typedef int __wasi_fd_t;

_Static_assert(sizeof(__wasi_fd_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_fd_t) == 4, "witx calculated align");

 */
/**
*** Hidden type
 */
/**
*** Hidden type
 */
/**
*** Hidden type
typedef int64_t __wasi_filedelta_t;

_Static_assert(sizeof(__wasi_filedelta_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_filedelta_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint8_t __wasi_whence_t;

#define __WASI_WHENCE_HIDDEN (UINT8_C(0))

_Static_assert(sizeof(__wasi_whence_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_whence_t) == 1, "witx calculated align");

 */
/**
*** Hidden type
typedef uint64_t __wasi_dircookie_t;

_Static_assert(sizeof(__wasi_dircookie_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_dircookie_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint32_t __wasi_dirnamlen_t;

_Static_assert(sizeof(__wasi_dirnamlen_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_dirnamlen_t) == 4, "witx calculated align");

 */
/**
*** Hidden type
typedef uint64_t __wasi_inode_t;

_Static_assert(sizeof(__wasi_inode_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_inode_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef struct __wasi_dirent_t {
    __wasi_dircookie_t d_next;

    __wasi_inode_t d_ino;

    __wasi_dirnamlen_t d_namlen;

    __wasi_filetype_t d_type;

} __wasi_dirent_t;

_Static_assert(sizeof(__wasi_dirent_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_dirent_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_dirent_t, d_next) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_ino) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_namlen) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_dirent_t, d_type) == 20, "witx calculated offset");

 */
/**
*** Hidden type
typedef uint8_t __wasi_advice_t;

#define __WASI_ADVICE_HIDDEN (UINT8_C(0))

_Static_assert(sizeof(__wasi_advice_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_advice_t) == 1, "witx calculated align");

 */
/**
*** Hidden type
typedef uint16_t __wasi_fdflags_t;

#define __WASI_FDFLAGS_HIDDEN ((__wasi_fdflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef struct __wasi_fdstat_t {
    __wasi_filetype_t fs_filetype;

    __wasi_fdflags_t fs_flags;

    __wasi_rights_t fs_rights_base;

    __wasi_rights_t fs_rights_inheriting;

} __wasi_fdstat_t;

_Static_assert(sizeof(__wasi_fdstat_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_fdstat_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_fdstat_t, fs_filetype) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_flags) == 2, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_rights_base) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_fdstat_t, fs_rights_inheriting) == 16, "witx calculated offset");

 */
/**
*** Hidden type
typedef uint64_t __wasi_device_t;

_Static_assert(sizeof(__wasi_device_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_device_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint16_t __wasi_fstflags_t;

#define __WASI_FSTFLAGS_HIDDEN ((__wasi_fstflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef uint32_t __wasi_lookupflags_t;

#define __WASI_LOOKUPFLAGS_HIDDEN ((__wasi_lookupflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef uint16_t __wasi_oflags_t;

#define __WASI_OFLAGS_HIDDEN ((__wasi_oflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef uint64_t __wasi_linkcount_t;

_Static_assert(sizeof(__wasi_linkcount_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_linkcount_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef struct __wasi_filestat_t {
    __wasi_device_t dev;

    __wasi_inode_t ino;

    __wasi_filetype_t filetype;

    __wasi_linkcount_t nlink;

    __wasi_filesize_t size;

    __wasi_timestamp_t atim;

    __wasi_timestamp_t mtim;

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

 */
/**
*** Hidden type
typedef uint64_t __wasi_userdata_t;

_Static_assert(sizeof(__wasi_userdata_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_userdata_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef uint8_t __wasi_eventtype_t;

#define __WASI_EVENTTYPE_HIDDEN1 (UINT8_C(0))

#define __WASI_EVENTTYPE_HIDDEN2 (UINT8_C(1))

#define __WASI_EVENTTYPE_HIDDEN3 (UINT8_C(2))

_Static_assert(sizeof(__wasi_eventtype_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_eventtype_t) == 1, "witx calculated align");

 */
/**
*** Hidden type
typedef uint16_t __wasi_eventrwflags_t;

#define __WASI_EVENTRWFLAGS_HIDDEN ((__wasi_eventrwflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef struct __wasi_event_fd_readwrite_t {
    __wasi_filesize_t nbytes;

    __wasi_eventrwflags_t flags;

} __wasi_event_fd_readwrite_t;

_Static_assert(sizeof(__wasi_event_fd_readwrite_t) == 16, "witx calculated size");
_Static_assert(_Alignof(__wasi_event_fd_readwrite_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_event_fd_readwrite_t, nbytes) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_fd_readwrite_t, flags) == 8, "witx calculated offset");

 */
/**
*** Hidden type
typedef struct __wasi_event_t {
    __wasi_userdata_t userdata;

    __wasi_errno_t error;

    __wasi_eventtype_t type;

    __wasi_event_fd_readwrite_t fd_readwrite;

} __wasi_event_t;

_Static_assert(sizeof(__wasi_event_t) == 32, "witx calculated size");
_Static_assert(_Alignof(__wasi_event_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_event_t, userdata) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, error) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, type) == 10, "witx calculated offset");
_Static_assert(offsetof(__wasi_event_t, fd_readwrite) == 16, "witx calculated offset");

 */
/**
*** Hidden type
typedef uint16_t __wasi_subclockflags_t;

#define __WASI_SUBCLOCKFLAGS_HIDDEN ((__wasi_subclockflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef struct __wasi_subscription_clock_t {
    __wasi_clockid_t id;

    __wasi_timestamp_t timeout;

    __wasi_timestamp_t precision;

    __wasi_subclockflags_t flags;

} __wasi_subscription_clock_t;

_Static_assert(sizeof(__wasi_subscription_clock_t) == 32, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_clock_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_clock_t, id) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, timeout) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, precision) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_clock_t, flags) == 24, "witx calculated offset");

 */
/**
*** Hidden type
typedef struct __wasi_subscription_fd_readwrite_t {
    __wasi_fd_t file_descriptor;

} __wasi_subscription_fd_readwrite_t;

_Static_assert(sizeof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_fd_readwrite_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_fd_readwrite_t, file_descriptor) == 0, "witx calculated offset");

 */
/**
*** Hidden type
typedef union __wasi_subscription_u_u_t {
    __wasi_subscription_clock_t hidden1;
    __wasi_subscription_fd_readwrite_t hidden2;
    __wasi_subscription_fd_readwrite_t hidden3;
} __wasi_subscription_u_u_t;
typedef struct __wasi_subscription_u_t {
    uint8_t tag;
    __wasi_subscription_u_u_t u;
} __wasi_subscription_u_t;

_Static_assert(sizeof(__wasi_subscription_u_t) == 40, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_u_t) == 8, "witx calculated align");

 */
/**
*** Hidden type
typedef struct __wasi_subscription_t {
    __wasi_userdata_t userdata;

    __wasi_subscription_u_t u;

} __wasi_subscription_t;

_Static_assert(sizeof(__wasi_subscription_t) == 48, "witx calculated size");
_Static_assert(_Alignof(__wasi_subscription_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_subscription_t, userdata) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_subscription_t, u) == 8, "witx calculated offset");

 */
/**
*** Hidden type
typedef uint32_t __wasi_exitcode_t;

_Static_assert(sizeof(__wasi_exitcode_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_exitcode_t) == 4, "witx calculated align");

 */
/**
*** Hidden type
typedef uint16_t __wasi_roflags_t;

#define __WASI_ROFLAGS_HIDDEN ((__wasi_roflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef uint16_t __wasi_siflags_t;

_Static_assert(sizeof(__wasi_siflags_t) == 2, "witx calculated size");
_Static_assert(_Alignof(__wasi_siflags_t) == 2, "witx calculated align");

 */
/**
*** Hidden type
typedef uint8_t __wasi_sdflags_t;

#define __WASI_SDFLAGS_HIDDEN ((__wasi_sdflags_t)(1 << 0))

 */
/**
*** Hidden type
typedef uint8_t __wasi_preopentype_t;

#define __WASI_PREOPENTYPE_HIDDEN (UINT8_C(0))

_Static_assert(sizeof(__wasi_preopentype_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_preopentype_t) == 1, "witx calculated align");

 */
/**
*** Hidden type
typedef struct __wasi_prestat_dir_t {
    __wasi_size_t pr_name_len;

} __wasi_prestat_dir_t;

_Static_assert(sizeof(__wasi_prestat_dir_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_prestat_dir_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_prestat_dir_t, pr_name_len) == 0, "witx calculated offset");

 */
/**
*** Hidden type
typedef union __wasi_prestat_u_t {
    __wasi_prestat_dir_t hidden;
} __wasi_prestat_u_t;
typedef struct __wasi_prestat_t {
    uint8_t tag;
    __wasi_prestat_u_t u;
} __wasi_prestat_t;

_Static_assert(sizeof(__wasi_prestat_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_prestat_t) == 4, "witx calculated align");

 */
/**
 * Represents a large number of items
 */
typedef uint64_t __wasi_longsize_t;

_Static_assert(sizeof(__wasi_longsize_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_longsize_t) == 8, "witx calculated align");

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
 * Represents an optional process ID
 */
typedef union __wasi_option_pid_u_t {
    uint8_t none;
    __wasi_pid_t some;
} __wasi_option_pid_u_t;
typedef struct __wasi_option_pid_t {
    uint8_t tag;
    __wasi_option_pid_u_t u;
} __wasi_option_pid_t;

_Static_assert(sizeof(__wasi_option_pid_t) == 8, "witx calculated size");
_Static_assert(_Alignof(__wasi_option_pid_t) == 4, "witx calculated align");

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
 * Flags for the 'fd_event' call
 */
typedef uint16_t __wasi_eventfdflags_t;

/**
 * Indicates if this event file description will run as a semaphore
 */
#define __WASI_EVENTFDFLAGS_SEMAPHORE ((__wasi_eventfdflags_t)(1 << 0))

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

    /**
     * Indicates if line feeds are ignored on the input
     */
    __wasi_bool_t line_feeds;

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
_Static_assert(offsetof(__wasi_tty_t, line_feeds) == 21, "witx calculated offset");

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
    __wasi_pid_t pid;

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
_Static_assert(offsetof(__wasi_process_handles_t, pid) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stdin) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stdout) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_process_handles_t, stderr) == 20, "witx calculated offset");

/**
 * Socket type
 */
typedef uint8_t __wasi_sock_type_t;

/**
 * Unused enum variant to prevent $socket_dgram value of 0, which
 * conflicts with a macro defined in cpython which is used in a case arm
 * of a switch statement alongside C macros generated from this enum.
 *  ref: https://github.com/python/cpython/blob/2f369cafeeb4a4886b00396abd8a5f33e555e1c3/Modules/getaddrinfo.c#L68
 *  ref: https://github.com/python/cpython/blob/3f369cafeeb4a4886b00396abd8a5f33e555e1c3/Modules/getaddrinfo.c#L355-L368
 */
#define __WASI_SOCK_TYPE_SOCKET_UNUSED (UINT8_C(0))

/**
 * The file descriptor or file refers to a byte-stream socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_STREAM (UINT8_C(1))

/**
 * The file descriptor or file refers to a datagram socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_DGRAM (UINT8_C(2))

/**
 * The file descriptor or file refers to a raw socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_RAW (UINT8_C(3))

/**
 * The file descriptor or file refers to a sequential packet socket.
 */
#define __WASI_SOCK_TYPE_SOCKET_SEQPACKET (UINT8_C(4))

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
 * join flags.
 */
typedef uint32_t __wasi_join_flags_t;

/**
 * Non-blocking join on the process
 */
#define __WASI_JOIN_FLAGS_NON_BLOCKING ((__wasi_join_flags_t)(1 << 0))

/**
 * Return if a process is stopped
 */
#define __WASI_JOIN_FLAGS_WAKE_STOPPED ((__wasi_join_flags_t)(1 << 1))

/**
 * What has happened with the proccess when we joined on it
 */
typedef uint8_t __wasi_join_status_type_t;

/**
 * Nothing has happened
 */
#define __WASI_JOIN_STATUS_TYPE_NOTHING (UINT8_C(0))

/**
 * The process has exited by a normal exit code
 */
#define __WASI_JOIN_STATUS_TYPE_EXIT_NORMAL (UINT8_C(1))

/**
 * The process was terminated by a signal
 */
#define __WASI_JOIN_STATUS_TYPE_EXIT_SIGNAL (UINT8_C(2))

/**
 * The process was stopped by a signal and can be resumed with SIGCONT
 */
#define __WASI_JOIN_STATUS_TYPE_STOPPED (UINT8_C(3))

_Static_assert(sizeof(__wasi_join_status_type_t) == 1, "witx calculated size");
_Static_assert(_Alignof(__wasi_join_status_type_t) == 1, "witx calculated align");

/**
 * Represents an errno and a signal
 */
typedef struct __wasi_errno_signal_t {
    /**
     * The exit code that was returned
     */
    __wasi_errno_t exit_code;

    /**
     * The signal that was returned
     */
    __wasi_signal_t signal;

} __wasi_errno_signal_t;

_Static_assert(sizeof(__wasi_errno_signal_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_errno_signal_t) == 2, "witx calculated align");
_Static_assert(offsetof(__wasi_errno_signal_t, exit_code) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_errno_signal_t, signal) == 2, "witx calculated offset");

/**
 * join status.
 */
typedef union __wasi_join_status_u_t {
    uint8_t nothing;
    __wasi_errno_t exit_normal;
    __wasi_errno_signal_t exit_signal;
    __wasi_signal_t stopped;
} __wasi_join_status_u_t;
typedef struct __wasi_join_status_t {
    uint8_t tag;
    __wasi_join_status_u_t u;
} __wasi_join_status_t;

_Static_assert(sizeof(__wasi_join_status_t) == 6, "witx calculated size");
_Static_assert(_Alignof(__wasi_join_status_t) == 2, "witx calculated align");

/**
 * thread state flags.
 */
typedef uint16_t __wasi_thread_flags_t;

/**
 * tsd_used
 */
#define __WASI_THREAD_FLAGS_TSD_USED ((__wasi_thread_flags_t)(1 << 0))

/**
 * dlerror_flag
 */
#define __WASI_THREAD_FLAGS_DLERROR_FLAG ((__wasi_thread_flags_t)(1 << 1))

/**
 * represents the state of a thread
 */
typedef struct __wasi_thread_state_t {
    __wasi_pointersize_t thread_self;

    __wasi_pointersize_t dtv;

    __wasi_pointersize_t thread_prev;

    __wasi_pointersize_t thread_next;

    __wasi_pointersize_t sysinfo;

    __wasi_pointersize_t canary;

    int32_t tid;

    int32_t errno_val;

    int32_t detach_state;

    int32_t cancel;

    uint8_t canceldisable;

    uint8_t cancelasync;

    __wasi_thread_flags_t flags;

    __wasi_pointersize_t map_base;

    __wasi_pointersize_t map_size;

    __wasi_pointersize_t stack;

    __wasi_pointersize_t stack_size;

    __wasi_pointersize_t guard_size;

    __wasi_pointersize_t result;

    __wasi_pointersize_t cancelbuf;

    __wasi_pointersize_t tsd;

    __wasi_pointersize_t robust_list_head;

    __wasi_pointersize_t robust_list_off;

    __wasi_pointersize_t robust_list_pending;

    int32_t h_errno_val;

    int32_t timer_id;

    __wasi_pointersize_t locale;

    int32_t killlock;

    __wasi_pointersize_t dlerror_buf;

    __wasi_pointersize_t stdio_locks;

} __wasi_thread_state_t;

_Static_assert(sizeof(__wasi_thread_state_t) == 112, "witx calculated size");
_Static_assert(_Alignof(__wasi_thread_state_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_thread_state_t, thread_self) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, dtv) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, thread_prev) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, thread_next) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, sysinfo) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, canary) == 20, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, tid) == 24, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, errno_val) == 28, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, detach_state) == 32, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, cancel) == 36, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, canceldisable) == 40, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, cancelasync) == 41, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, flags) == 42, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, map_base) == 44, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, map_size) == 48, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, stack) == 52, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, stack_size) == 56, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, guard_size) == 60, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, result) == 64, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, cancelbuf) == 68, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, tsd) == 72, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, robust_list_head) == 76, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, robust_list_off) == 80, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, robust_list_pending) == 84, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, h_errno_val) == 88, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, timer_id) == 92, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, locale) == 96, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, killlock) == 100, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, dlerror_buf) == 104, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_state_t, stdio_locks) == 108, "witx calculated offset");

/**
 * thread start type
 */
typedef struct __wasi_thread_start_t {
    /**
     * Address where the stack starts
     */
    __wasi_pointersize_t stack;

    /**
     * Address where the TLS starts
     */
    __wasi_pointersize_t tls_base;

    /**
     * Function that will be invoked when the thread starts
     */
    __wasi_pointersize_t start_funct;

    /**
     * Arguments to pass the callback function
     */
    __wasi_pointersize_t start_args;

    /**
     * Reserved for future WASI usage
     */
    __wasi_pointersize_t reserved0;

    __wasi_pointersize_t reserved1;

    __wasi_pointersize_t reserved2;

    __wasi_pointersize_t reserved3;

    __wasi_pointersize_t reserved4;

    __wasi_pointersize_t reserved5;

    __wasi_pointersize_t reserved6;

    __wasi_pointersize_t reserved7;

    __wasi_pointersize_t reserved8;

    __wasi_pointersize_t reserved9;

    /**
     * The size of the stack
     */
    __wasi_pointersize_t stack_size;

    /**
     * The size of the guards at the end of the stack
     */
    __wasi_pointersize_t guard_size;

} __wasi_thread_start_t;

_Static_assert(sizeof(__wasi_thread_start_t) == 64, "witx calculated size");
_Static_assert(_Alignof(__wasi_thread_start_t) == 4, "witx calculated align");
_Static_assert(offsetof(__wasi_thread_start_t, stack) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, tls_base) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, start_funct) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, start_args) == 12, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved0) == 16, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved1) == 20, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved2) == 24, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved3) == 28, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved4) == 32, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved5) == 36, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved6) == 40, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved7) == 44, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved8) == 48, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, reserved9) == 52, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, stack_size) == 56, "witx calculated offset");
_Static_assert(offsetof(__wasi_thread_start_t, guard_size) == 60, "witx calculated offset");

/**
 * epoll type.
 */
typedef uint32_t __wasi_epoll_type_t;

#define __WASI_EPOLL_TYPE_EPOLLIN ((__wasi_epoll_type_t)(1 << 0))

#define __WASI_EPOLL_TYPE_EPOLLOUT ((__wasi_epoll_type_t)(1 << 1))

#define __WASI_EPOLL_TYPE_EPOLLRDHUP ((__wasi_epoll_type_t)(1 << 2))

#define __WASI_EPOLL_TYPE_EPOLLPRI ((__wasi_epoll_type_t)(1 << 3))

#define __WASI_EPOLL_TYPE_EPOLLERR ((__wasi_epoll_type_t)(1 << 4))

#define __WASI_EPOLL_TYPE_EPOLLHUP ((__wasi_epoll_type_t)(1 << 5))

#define __WASI_EPOLL_TYPE_EPOLLET ((__wasi_epoll_type_t)(1 << 6))

#define __WASI_EPOLL_TYPE_EPOLLONESHOT ((__wasi_epoll_type_t)(1 << 7))

/**
 * epoll ctl
 */
typedef uint32_t __wasi_epoll_ctl_t;

/**
 * Add an entry to the interest list of the epoll file descriptor, epfd.
 */
#define __WASI_EPOLL_CTL_ADD (UINT32_C(0))

/**
 * Change the settings associated with fd in the interest list to the new settings specified in event.
 */
#define __WASI_EPOLL_CTL_MOD (UINT32_C(1))

/**
 * Remove (deregister) the target file descriptor fd from the interest list.
 */
#define __WASI_EPOLL_CTL_DEL (UINT32_C(2))

_Static_assert(sizeof(__wasi_epoll_ctl_t) == 4, "witx calculated size");
_Static_assert(_Alignof(__wasi_epoll_ctl_t) == 4, "witx calculated align");

/**
 * epoll data
 */
typedef struct __wasi_epoll_data_t {
    /**
     * Pointer to some user defined data
     */
    __wasi_pointersize_t ptr;

    /**
     * The file descriptor of the event
     */
    __wasi_fd_t fd;

    /**
     * User data for the event
     */
    uint32_t data1;

    /**
     * User data for the event
     */
    uint64_t data2;

} __wasi_epoll_data_t;

_Static_assert(sizeof(__wasi_epoll_data_t) == 24, "witx calculated size");
_Static_assert(_Alignof(__wasi_epoll_data_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_epoll_data_t, ptr) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_epoll_data_t, fd) == 4, "witx calculated offset");
_Static_assert(offsetof(__wasi_epoll_data_t, data1) == 8, "witx calculated offset");
_Static_assert(offsetof(__wasi_epoll_data_t, data2) == 16, "witx calculated offset");

/**
 * epoll event and its data
 */
typedef struct __wasi_epoll_event_t {
    /**
     * The events that are triggered for this
     */
    __wasi_epoll_type_t events;

    /**
     * The data of the event
     */
    __wasi_epoll_data_t data;

} __wasi_epoll_event_t;

_Static_assert(sizeof(__wasi_epoll_event_t) == 32, "witx calculated size");
_Static_assert(_Alignof(__wasi_epoll_event_t) == 8, "witx calculated align");
_Static_assert(offsetof(__wasi_epoll_event_t, events) == 0, "witx calculated offset");
_Static_assert(offsetof(__wasi_epoll_event_t, data) == 8, "witx calculated offset");

/**
 * @defgroup wasix_32v1
 * @{
 */

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
 * Opens a pipe with two file handles
 * 
 * Pipes are bidirectional
 */
__wasi_errno_t __wasi_fd_pipe(
    __wasi_fd_t *retptr0,
    __wasi_fd_t *retptr1
) __attribute__((__warn_unused_result__));
/**
 * Retrieves the current state of the TTY
 */
__wasi_errno_t __wasi_tty_get(
    __wasi_tty_t * state
) __attribute__((__warn_unused_result__));
/**
 * Updates the properties of the the TTY
 */
__wasi_errno_t __wasi_tty_set(
    __wasi_tty_t * state
) __attribute__((__warn_unused_result__));
/**
 * Returns the current working directory
 * 
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
 * Creates a new thread by spawning that shares the same
 * memory address space, file handles and main event loops.
 * The web assembly process must export function named 'wasi_thread_start'
 * @return
 * Returns the thread index of the newly created thread
 * (indices always start from zero)
 */
__wasi_errno_t __wasi_thread_spawn_v2(
    /**
     * Pointer to the structure the describes the thread
     * that is being spawened
     */
    __wasi_thread_start_t * args,
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
 * (threads indices are sequential from zero while the
 *  main thread ID equals the process ID)
 */
__wasi_errno_t __wasi_thread_id(
    __wasi_tid_t *retptr0
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
 * Sends a signal to a specific thread
 */
__wasi_errno_t __wasi_thread_signal(
    /**
     * Handle of the thread to send a signal
     */
    __wasi_tid_t tid,
    /**
     * Signal to send to the thread
     */
    __wasi_signal_t signal
) __attribute__((__warn_unused_result__));
/**
 * Wait for a futex_wake operation to wake us.
 * 
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
 * 
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
 * 
 * This is used by `longjmp` and `setjmp`
 * 
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
 * Restores the current stack to a previous stack described by supplying
 * stack snapshot.
 * 
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
 * Spawns a new process within the context of the parent process
 * (i.e. this process). It inherits the filesystem and sandbox
 * permissions but runs standalone.
 * @return
 * Returns a bus process id that can be used to invoke calls
 */
__wasi_errno_t __wasi_proc_spawn(
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
 * 
 * Passing none to PID will mean that the call will wait
 * for any subprocess to exit. PID will be populated with
 * the process that exited.
 * @return
 * Returns the status of the process
 */
__wasi_errno_t __wasi_proc_join(
    /**
     * ID of the process to wait on
     */
    __wasi_option_pid_t * pid,
    /**
     * Flags that determine how the join behaves
     */
    __wasi_join_flags_t flags,
    __wasi_join_status_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Sends a signal to another process
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
 * 
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
 * Adds a default gateway to the local port
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
__wasi_errno_t __wasi_sock_accept_v2(
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
/**
 * Create an epoll interest list
 * 
 * @return
 * The file descriptor for this epoll interest list
 */
__wasi_errno_t __wasi_epoll_create(
    __wasi_fd_t *retptr0
) __attribute__((__warn_unused_result__));
/**
 * Modifies an epoll interest list
 * 
 * @return
 * The number of bytes written.
 */
__wasi_errno_t __wasi_epoll_ctl(
    /**
     * File descriptor of the epoll interest list
     */
    __wasi_fd_t epfd,
    /**
     * Operation to be made on the list
     */
    __wasi_epoll_ctl_t op,
    /**
     * File descriptor to be added, deleted or modified
     */
    __wasi_fd_t fd,
    /**
     * Reference to the event to be added, deleted or modified
     */
    const __wasi_epoll_event_t * event
) __attribute__((__warn_unused_result__));
/**
 * wait for an I/O event on an epoll file descriptor
 * 
 * @return
 * The number of events returned.
 */
__wasi_errno_t __wasi_epoll_wait(
    /**
     * File descriptor of the epoll interest list
     */
    __wasi_fd_t epfd,
    /**
     * Reference to the array of events
     */
    __wasi_epoll_event_t * event,
    /**
     * Maximum number of events that will be returned in the array
     */
    __wasi_size_t maxevents,
    /**
     * Timeout for the wait event
     */
    __wasi_timestamp_t timeout,
    __wasi_size_t *retptr0
) __attribute__((__warn_unused_result__));
/** @} */

#ifdef __cplusplus
}
#endif

#endif
