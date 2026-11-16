#ifndef __wasi_cwd_h
#define __wasi_cwd_h

#include <lock.h>
#include <stddef.h>
#include <wasi/version.h>

#if defined(__wasip2__) || defined(__wasip3__)
/// The cwd, defined in `getcwd.c`, and whether it was malloc'd.
DECLARE_WEAK_LOCK(__wasilibc_cwd_lock, extern);
extern char *__wasilibc_cwd;
extern int __wasilibc_cwd_mallocd;

/// Starts the cwd at `wasi:cli/environment.initial-cwd` the first time the cwd
/// is needed. Returns 0 on success, or -1 with `errno` set to `ENOMEM`, in
/// which case a later call tries again.
///
/// The caller must not hold `__wasilibc_cwd_lock`.
int __wasilibc_ensure_cwd(void);

/// Resets the cwd to `/` so that the next `__wasilibc_ensure_cwd` asks the host
/// again. Used by `__wasilibc_reset_preopens`.
///
/// The caller must not hold `__wasilibc_cwd_lock`.
void __wasilibc_reset_cwd(void);

/// Like `__wasilibc_find_relpath_alloc`, for when `chdir` isn't linked and so
/// the cwd is always the initial cwd. `*relative_path` points into `path` or
/// into `*buf`, which holds the absolute path if one had to be built.
int __wasilibc_find_relpath_initial_cwd(const char *path,
                                        const char **abs_prefix,
                                        const char **relative_path, char **buf,
                                        size_t *buf_len, int can_realloc);
#endif

#endif
