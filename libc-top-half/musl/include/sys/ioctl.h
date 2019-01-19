#ifndef	_SYS_IOCTL_H
#define	_SYS_IOCTL_H
#ifdef __wasilibc_unmodified_upstream__
#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ioctl.h>

int ioctl (int, int, ...);

#ifdef __cplusplus
}
#endif
#else
#include <__wasilibc_sys_ioctl.h>
#endif
#endif
