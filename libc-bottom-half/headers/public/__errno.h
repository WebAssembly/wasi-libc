#ifndef __wasilibc___errno_h
#define __wasilibc___errno_h

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
extern thread_local int __errno;
#else
extern _Thread_local int __errno;
#endif

int *__errno_location(void);

#define errno (*__errno_location())

#ifdef __cplusplus
}
#endif

#endif
