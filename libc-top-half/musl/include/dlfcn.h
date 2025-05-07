#ifndef	_DLFCN_H
#define	_DLFCN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <features.h>
#include <wasi/api.h>

#define RTLD_LAZY   __WASI_DL_FLAGS_LAZY
#define RTLD_NOW    __WASI_DL_FLAGS_NOW
#define RTLD_NOLOAD __WASI_DL_FLAGS_NOLOAD
#define RTLD_NODELETE __WASI_DL_FLAGS_NODELETE
#define RTLD_GLOBAL __WASI_DL_FLAGS_GLOBAL
#define RTLD_LOCAL  0

// #define RTLD_NEXT    ((void *)-1)
#define RTLD_DEFAULT ((void *)0)

// #define RTLD_DI_LINKMAP 2

int    dlclose(void *);
char  *dlerror(void);
void  *dlopen(const char *, int);
void  *dlsym(void *__restrict, const char *__restrict);

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
typedef struct {
	const char *dli_fname;
	void *dli_fbase;
	const char *dli_sname;
	void *dli_saddr;
} Dl_info;
int dladdr(const void *, Dl_info *);
int dlinfo(void *, int, void *);
#endif

#if _REDIR_TIME64
__REDIR(dlsym, __dlsym_time64);
#endif

#ifdef __cplusplus
}
#endif

#endif
