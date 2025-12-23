#ifndef __wasilibc___header_fcntl_h
#define __wasilibc___header_fcntl_h

#include <__mode_t.h>
#include <__seek.h>

#define O_APPEND 0x0001
#define O_DSYNC 0x0002
#define O_NONBLOCK 0x0004
#define O_RSYNC 0x0008
#define O_SYNC 0x0010
#define O_CREAT (0x0001 << 12)
#define O_DIRECTORY (0x0002 << 12)
#define O_EXCL (0x0004 << 12)
#define O_TRUNC (0x0008 << 12)

#define O_NOFOLLOW (0x01000000)
#define O_EXEC (0x02000000)
#define O_RDONLY (0x04000000)
#define O_SEARCH (0x08000000)
#define O_WRONLY (0x10000000)

/*
 * O_CLOEXEC is defined to be zero, as WASI has no exec-style functions.
 */
#define O_CLOEXEC (0)

/*
 * O_TTY_INIT is defined to be zero, meaning that WASI implementations are
 * expected to always initialize a terminal the first time it's opened.
 */
#define O_TTY_INIT (0)

#define O_NOCTTY (0)

#define O_RDWR (O_RDONLY | O_WRONLY)
#define O_ACCMODE (O_EXEC | O_RDWR | O_SEARCH)

#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_SEQUENTIAL 1
#define POSIX_FADV_RANDOM 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5

#define F_GETFD (1)
#define F_SETFD (2)
#define F_GETFL (3)
#define F_SETFL (4)

#define FD_CLOEXEC (1)

#define AT_EACCESS (0x0)
#define AT_SYMLINK_NOFOLLOW (0x1)
#define AT_SYMLINK_FOLLOW (0x2)
#define AT_REMOVEDIR (0x4)

#define AT_FDCWD (-2)

#endif
