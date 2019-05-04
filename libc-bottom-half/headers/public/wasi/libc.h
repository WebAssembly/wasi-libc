#ifndef __wasi_libc_h
#define __wasi_libc_h

#include <__typedef_off_t.h>

#ifdef __cplusplus
extern "C" {
#endif

void __wasilibc_init_preopen(void);
int __wasilibc_register_preopened_fd(int fd, const char *path);
int __wasilibc_fd_renumber(int fd, int newfd);
int __wasilibc_unlinkat(int fd, const char *path);
int __wasilibc_rmdirat(int fd, const char *path);
off_t __wasilibc_tell(int fd);

#ifdef __cplusplus
}
#endif

#endif
