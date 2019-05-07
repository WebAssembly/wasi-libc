#ifndef __wasi_libc_nocwd_h
#define __wasi_libc_nocwd_h

/*
 * In order to support AT_FDCWD, we need to wrap the *at functions to handle
 * it by calling back into the non-at versions which perform libpreopen
 * queries. These __wasilibc_nocwd_* forms are the underlying calls which
 * assume AT_FDCWD has already been resolved.
 */

#ifdef __cplusplus
extern "C" {
#endif

int __wasilibc_nocwd___wasilibc_unlinkat(int, const char *);
int __wasilibc_nocwd___wasilibc_rmdirat(int, const char *);
int __wasilibc_nocwd_linkat(int, const char *, int, const char *, int);
int __wasilibc_nocwd_symlinkat(const char *, int, const char *);
ssize_t __wasilibc_nocwd_readlinkat(int, const char *__restrict, char *__restrict, size_t);
int __wasilibc_nocwd_unlinkat(int, const char *, int);
int __wasilibc_nocwd_faccessat(int, const char *, int, int);
int __wasilibc_nocwd_renameat(int, const char *, int, const char *);
int __wasilibc_nocwd_openat(int, const char *, int, ...);
int __wasilibc_nocwd_fstatat(int, const char *__restrict, struct stat *__restrict, int);
int __wasilibc_nocwd_mkdirat(int, const char *, mode_t);
int __wasilibc_nocwd_utimensat(int, const char *, const struct timespec [2], int);
DIR *__wasilibc_nocwd_opendirat(int, const char *);
int __wasilibc_nocwd_scandirat(int, const char *, struct dirent ***,
                               int (*)(const struct dirent *),
                               int (*)(const struct dirent **, const struct dirent **));

#ifdef __cplusplus
}
#endif

#endif
