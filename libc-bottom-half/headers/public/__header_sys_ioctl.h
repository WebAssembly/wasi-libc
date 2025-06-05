#ifndef __wasilibc___header_sys_ioctl_h
#define __wasilibc___header_sys_ioctl_h

#define FIONREAD 1
#define FIONBIO 2

#define TIOCGWINSZ 0x101
#define TIOCSWINSZ 0x102

#ifdef __cplusplus
extern "C" {
#endif

/* MUSL includes these defines */
#define __NEED_struct_winsize
#include <bits/alltypes.h>
#include <bits/ioctl.h>
/* glibc also includes this one */
#include <sys/ttydefaults.h>

int ioctl(int, int, ...);

#ifdef __cplusplus
}
#endif

#endif
