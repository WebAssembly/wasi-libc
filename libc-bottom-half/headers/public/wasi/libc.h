#ifndef __wasi_libc_h
#define __wasi_libc_h

#ifdef __cplusplus
extern "C" {
#endif

int __wasilibc_register_preopened_fd(int fd, const char *path);
int __wasilibc_fd_renumber(int fd, int newfd);
int __wasilibc_rmfileat(int fd, const char *path);
int __wasilibc_rmdirat(int fd, const char *path);

#ifdef __cplusplus
}
#endif

#endif
