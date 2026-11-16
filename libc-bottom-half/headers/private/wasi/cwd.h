#ifndef __wasi_cwd_h
#define __wasi_cwd_h

#include <wasi/version.h>

#if defined(__wasip2__) || defined(__wasip3__)
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
#endif

#endif
