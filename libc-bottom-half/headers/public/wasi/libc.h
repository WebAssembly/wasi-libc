#ifndef __wasi_libc_h
#define __wasi_libc_h

#include <__struct_timespec.h>
#include <__typedef_clockid_t.h>
#include <__typedef_off_t.h>
#include <unistd.h>
#include <wasi/version.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stat;
struct timespec;

/// Populate libc's preopen tables. This is normally done automatically
/// just before it's needed, however if you call `__wasi_fd_renumber` or
/// `__wasi_fd_close` directly, and you need the preopens to be accurate
/// afterward, you should call this before doing so.
void __wasilibc_populate_preopens(void);

/// Clear the set of preopens (and, on p2+, any other open file descriptors).
///
/// This is appropriate to call just prior to snapshotting the guest state so
/// that it can be resumed on another runtime, in which case any
/// previously-opened handles will no longer be valid.
void __wasilibc_reset_preopens();

#ifndef __wasip2__
/// Register the given pre-opened file descriptor under the given path.
int __wasilibc_register_preopened_fd(int fd, const char *prefix);
#endif

/// Renumber `fd` to `newfd`; similar to `dup2` but does a move rather than a
/// copy.
int __wasilibc_fd_renumber(int fd, int newfd)
    __attribute__((__warn_unused_result__));

/// Like `unlinkat`, but without depending on `__wasi_path_remove_directory`.
int __wasilibc_unlinkat(int fd, const char *path)
    __attribute__((__warn_unused_result__));

/// An `*at` version of rmdir.
int __wasilibc_rmdirat(int fd, const char *path)
    __attribute__((__warn_unused_result__));

/// Like `open`, but without the varargs in the signature.
int __wasilibc_open_nomode(const char *path, int oflag);

/// Like `openat`, but without the varargs in the signature.
int __wasilibc_openat_nomode(int fd, const char *path, int oflag);

/// Return the current file offset. Like `lseek(fd, 0, SEEK_CUR)`, but without
/// depending on `lseek`.
off_t __wasilibc_tell(int fd) __attribute__((__warn_unused_result__));

/* Non-`at` forms of various `*at` functions. */
int __wasilibc_access(const char *pathname, int mode, int flags)
    __attribute__((__warn_unused_result__));
int __wasilibc_stat(const char *__restrict pathname,
                    struct stat *__restrict statbuf, int flags)
    __attribute__((__warn_unused_result__));
int __wasilibc_utimens(const char *pathname, const struct timespec times[2],
                       int flags) __attribute__((__warn_unused_result__));
int __wasilibc_link(const char *oldpath, const char *newpath, int flags)
    __attribute__((__warn_unused_result__));
int __wasilibc_link_oldat(int olddirfd, const char *oldpath,
                          const char *newpath, int flags)
    __attribute__((__warn_unused_result__));
int __wasilibc_link_newat(const char *oldpath, int newdirfd,
                          const char *newpath, int flags)
    __attribute__((__warn_unused_result__));
int __wasilibc_rename_oldat(int olddirfd, const char *oldpath,
                            const char *newpath)
    __attribute__((__warn_unused_result__));
int __wasilibc_rename_newat(const char *oldpath, int newdirfd,
                            const char *newpath)
    __attribute__((__warn_unused_result__));

/// Enable busywait in futex on current thread.
void __wasilibc_enable_futex_busywait_on_current_thread(void);

/// Fill a buffer with random bytes
int __wasilibc_random(void *buffer, size_t len)
    __attribute__((__warn_unused_result__));

enum {
  __WASILIBC_FUTEX_WAKE_ALL = -1,
  __WASILIBC_FUTEX_YIELD = 1,
};
/// Wait on a futex after first confirming `*addr == val`.
///
/// Returns 0 when woken, or a negated errno on failure:
/// - `-EWOULDBLOCK` if `*addr != val` at entry,
/// - `-ETIMEDOUT` if the deadline expires.
///
/// The `clock` argument selects the clock used for the absolute deadline `at`.
/// If `at` is NULL, this waits indefinitely.
/// The `flags` argument is currently unused and should be 0.
///
/// In single-threaded builds, calling this function with arguments that would
/// require waiting will trap.
int __wasilibc_futex_wait(volatile int *addr, int val, clockid_t clock,
                          const struct timespec *at, unsigned flags);

/// Wake up to `count` threads waiting on the futex at `addr`.
///
/// If `count` is `__WASILIBC_FUTEX_WAKE_ALL`, all waiters are woken.
/// `flags` may be `__WASILIBC_FUTEX_YIELD` in cooperative-threading builds
/// to yield to the woken thread(s) immediately. Otherwise, `flags` should be 0.
///
/// In single-threaded builds, this function is a no-op.
void __wasilibc_futex_wake(volatile int *addr, int count, unsigned flags);

#ifdef __cplusplus
}
#endif

#endif
