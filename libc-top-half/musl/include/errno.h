#ifndef	_ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>

#ifdef __wasilibc_unmodified_upstream /* Use alternate WASI libc headers */
#include <bits/errno.h>
#else
#include <__errno_values.h>
#endif

#ifdef __GNUC__
__attribute__((const))
#endif
int *__errno_location(void);

// Cross-module TLS does not work in WASIp3, so declaring an `extern` to some
// data doesn't work for errno. On other platforms though that's been the
// historical default so that's left in place.
#if !defined(__wasip1__) && !defined(__wasip2__)
#define errno (*__errno_location())
#else
extern _Thread_local int errno;
#define errno errno
#endif

#ifdef _GNU_SOURCE
extern char *program_invocation_short_name, *program_invocation_name;
#endif

#ifdef __cplusplus
}
#endif

#endif

