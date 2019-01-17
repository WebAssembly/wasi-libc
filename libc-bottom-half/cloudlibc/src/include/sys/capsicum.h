// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// <sys/capsicum.h> - file descriptor access controls
//
// Extensions:
// - CAP_FDATASYNC, CAP_POSIX_FADVISE, CAP_POSIX_FALLOCATE, CAP_READDIR,
//   CAP_READLINKAT:
//   fdatasync(), posix_fadvise(), posix_fallocate(), readdir() and
//   readlink() can be controlled independently in this environment.
// - cap_rights_get_explicit() and cap_rights_limit_explicit():
//   Capabilities are expressed as a pair of base and inheriting rights
//   in this environment.
//
// Features missing:
// - CAP_FCHDIR:
//   Per-process working directory is not available. Use *at() instead.
// - CAP_FCHFLAGS, CAP_CHFLAGSAT, CAP_FCHMOD, CAP_FCHMODAT, CAP_FCHOWN
//   and CAP_FCHOWNAT:
//   Filesystem access control management not available.
// - CAP_FLOCK:
//   File locking not available.
// - CAP_FPATHCONF:
//   TODO(ed): Add.
// - CAP_FSCK:
//   Not applicable to this environment.
// - CAP_FSTATFS:
//   Filesystem-level statistics not available.
// - CAP_ACCEPT, CAP_BIND, CAP_BINDAT, CAP_CONNECT, CAP_CONNECTAT,
//   CAP_GETPEERNAME, CAP_GETSOCKNAME and CAP_LISTEN:
//   Only anonymous, addressless sockets are supported.
// - CAP_KQUEUE, CAP_KQUEUE_CHANGE and CAP_KQUEUE_EVENT:
//   BSD kqueue is not available.
// - CAP_MKFIFOAT:
//   Only anonymous pipes are supported.
// - CAP_MKNODAT:
//   Device nodes cannot be created.
// - CAP_GETSOCKOPT and CAP_SETSOCKOPT:
//   Socket parameters cannot be adjusted.
// - CAP_MAC_*:
//   Mandatory Access Control not available.
// - CAP_SEM_*:
//   Semaphores are not represented as file descriptors.
// - CAP_IOCTL and cap_ioctls_*():
//   ioctl() not available.
// - cap_fcntl_*():
//   fcntl() rights cannot be adjusted granularly.
// - CAP_TTYHOOK:
//   Terminal management is not available.
// - CAP_PDGETPID:
//   Process identifiers are not exposed.
// - CAP_PDKILL:
//   Explicit signal delivery is not supported.
// - CAP_EXTATTR_*:
//   Extended inode attributes not available.
// - CAP_ACL_*:
//   Access Control Lists not available.

#ifndef _SYS_CAPSICUM_H_
#define _SYS_CAPSICUM_H_

#include <_/limits.h>
#include <_/types.h>

typedef __uint64_t __cap_rights_bits_t;
typedef struct {
  __cap_rights_bits_t __value;
} cap_rights_t;

#define _CAP_BIT(f) (_UINT64_C(1) << (f))
#define _CAP_SENTINEL _UINT64_C(0)

// General file I/O.
#define CAP_CREATE (_CAP_BIT(10) | _CAP_BIT(14))
#define CAP_FCNTL _CAP_BIT(3)
#define CAP_FDATASYNC _CAP_BIT(0)
#define CAP_FEXECVE _CAP_BIT(32)
#define CAP_FSYNC _CAP_BIT(4)
#define CAP_FTRUNCATE _CAP_BIT(20)
#define CAP_MMAP _CAP_BIT(26)
#define CAP_MMAP_R (CAP_MMAP | CAP_READ)
#define CAP_MMAP_RW (CAP_MMAP_R | CAP_MMAP_W)
#define CAP_MMAP_RWX (CAP_MMAP_R | CAP_MMAP_W | CAP_MMAP_X)
#define CAP_MMAP_RX (CAP_MMAP_R | CAP_MMAP_X)
#define CAP_MMAP_W (CAP_MMAP | CAP_WRITE)
#define CAP_MMAP_WX (CAP_MMAP_W | CAP_MMAP_X)
#define CAP_MMAP_X (CAP_MMAP | _CAP_BIT(27))
#define CAP_POSIX_FADVISE _CAP_BIT(7)    // Extension.
#define CAP_POSIX_FALLOCATE _CAP_BIT(8)  // Extension.
#define CAP_PREAD (CAP_READ | _CAP_BIT(2))
#define CAP_PWRITE (CAP_WRITE | _CAP_BIT(2))
#define CAP_READ _CAP_BIT(1)
#define CAP_READDIR _CAP_BIT(15)  // Extension.
#define CAP_SEEK (CAP_SEEK_TELL | _CAP_BIT(2))
#define CAP_SEEK_TELL _CAP_BIT(5)
#define CAP_WRITE _CAP_BIT(6)

// VFS methods.
#define CAP_FSTAT _CAP_BIT(19)
#define CAP_FSTATAT _CAP_BIT(22)
#define CAP_FUTIMES _CAP_BIT(21)
#define CAP_FUTIMESAT _CAP_BIT(23)
#define CAP_LINKAT_SOURCE _CAP_BIT(12)
#define CAP_LINKAT_TARGET _CAP_BIT(13)
#define CAP_LOOKUP _CAP_BIT(14)
#define CAP_MKDIRAT _CAP_BIT(9)
#define CAP_READLINKAT _CAP_BIT(16)  // Extension.
#define CAP_RENAMEAT_SOURCE _CAP_BIT(17)
#define CAP_RENAMEAT_TARGET _CAP_BIT(18)
#define CAP_SYMLINKAT _CAP_BIT(24)
#define CAP_UNLINKAT _CAP_BIT(25)

// Socket operations.
#define CAP_RECV CAP_READ
#define CAP_SEND CAP_WRITE
#define CAP_SHUTDOWN _CAP_BIT(39)

// Commonly used socket operations.
#define CAP_SOCK_CLIENT (CAP_RECV | CAP_SEND | CAP_SHUTDOWN)
#define CAP_SOCK_SERVER (CAP_RECV | CAP_SEND | CAP_SHUTDOWN)

// Polling.
#define CAP_EVENT _CAP_BIT(28)

// Process descriptors.
#define CAP_PDWAIT _CAP_BIT(30)

#define cap_rights_clear(...) __cap_rights_clear(__VA_ARGS__, _CAP_SENTINEL)
#define cap_rights_init(...) __cap_rights_init(__VA_ARGS__, _CAP_SENTINEL)
#define cap_rights_is_set(...) __cap_rights_is_set(__VA_ARGS__, _CAP_SENTINEL)
#define cap_rights_set(...) __cap_rights_set(__VA_ARGS__, _CAP_SENTINEL)

__BEGIN_DECLS
void CAP_ALL(cap_rights_t *);
void CAP_NONE(cap_rights_t *);
cap_rights_t *__cap_rights_clear(cap_rights_t *, ...);
cap_rights_t *__cap_rights_init(cap_rights_t *, ...);
_Bool __cap_rights_is_set(const cap_rights_t *, ...);
cap_rights_t *__cap_rights_set(cap_rights_t *, ...);
int cap_enter(void);
int cap_getmode(unsigned int *);
_Bool cap_rights_contains(const cap_rights_t *, const cap_rights_t *);
int cap_rights_get(int, cap_rights_t *);
int cap_rights_get_explicit(int, cap_rights_t *, cap_rights_t *);
int cap_rights_limit(int, const cap_rights_t *);
int cap_rights_limit_explicit(int, const cap_rights_t *, const cap_rights_t *);
cap_rights_t *cap_rights_merge(cap_rights_t *, const cap_rights_t *);
cap_rights_t *cap_rights_remove(cap_rights_t *, const cap_rights_t *);
_Bool cap_sandboxed(void);
__END_DECLS

#if _CLOUDLIBC_INLINE_FUNCTIONS
static __inline void _CAP_ALL(cap_rights_t *__rights) {
  __rights->__value =
      CAP_CREATE | CAP_EVENT | CAP_FCNTL | CAP_FDATASYNC | CAP_FEXECVE |
      CAP_FSTAT | CAP_FSTATAT | CAP_FSYNC | CAP_FTRUNCATE | CAP_FUTIMES |
      CAP_FUTIMESAT | CAP_LINKAT_SOURCE | CAP_LINKAT_TARGET | CAP_LOOKUP |
      CAP_MKDIRAT | CAP_MMAP | CAP_MMAP_X | CAP_PDWAIT | CAP_POSIX_FADVISE |
      CAP_POSIX_FALLOCATE | CAP_PREAD | CAP_PWRITE | CAP_READ | CAP_READDIR |
      CAP_READLINKAT | CAP_RENAMEAT_SOURCE | CAP_RENAMEAT_TARGET | CAP_SEEK |
      CAP_SEEK_TELL | CAP_SHUTDOWN | CAP_SYMLINKAT | CAP_UNLINKAT | CAP_WRITE;
}
#define CAP_ALL(rights) _CAP_ALL(rights)

static __inline void _CAP_NONE(cap_rights_t *__rights) {
  __rights->__value = 0;
}
#define CAP_NONE(rights) _CAP_NONE(rights)
#endif

#endif
