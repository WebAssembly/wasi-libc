#ifndef __wasilibc___errno_h
#define __wasilibc___errno_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _BUILDING_WASI_LIBC
/* libc internal access */
extern _Thread_local int errno;
#define errno errno
#else
/* user exposed ABI */
__attribute__((__const__)) int *__errno_location(void);
#define errno (*__errno_location())
#endif

#ifdef __cplusplus
}
#endif
#endif
