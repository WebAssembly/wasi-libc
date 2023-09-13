#ifndef __wasilibc___header_sys_ioctl_h
#define __wasilibc___header_sys_ioctl_h

#define FIONREAD 1
#define FIONBIO 2

#define TIOCGWINSZ 0x101

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int, int, ...);

#ifdef __cplusplus
}
#endif

#endif
