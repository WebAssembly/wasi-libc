#ifndef __cowslibc_sys_select_h
#define __cowslibc_sys_select_h

#include <__typedef_sigset_t.h>
#include <__typedef_fd_set.h>
#include <__struct_timeval.h>
#include <__struct_timespec.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: POSIX/musl/glibc have the last argument as
// `const sigset_t *sigmask` rather than `...`.
int pselect(int, fd_set *, fd_set *, fd_set *, const struct timespec *, ...);

void FD_CLR(int, fd_set *);
int FD_ISSET(int, fd_set *);
void FD_SET(int, fd_set *);
void FD_ZERO(fd_set *);

#define FD_CLR(fd, set) FD_CLR((fd), (set))
#define FD_ISSET(fd, set) FD_ISSET((fd), (set))
#define FD_SET(fd, set) FD_SET((fd), (set))
#define FD_ZERO(set) FD_ZERO((set))

#ifdef __cplusplus
}
#endif

#endif
