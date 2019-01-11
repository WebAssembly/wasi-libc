#ifndef __cowslibc_sys_select_h
#define __cowslibc_sys_select_h

#include <__fd_set.h>
#include <__struct_timespec.h>
#include <__struct_timeval.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * TODO: POSIX/musl/glibc have the last argument as
 * `const sigset_t *sigmask` rather than `...`.
 */
int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *, ...);

#ifdef __cplusplus
}
#endif

#endif
