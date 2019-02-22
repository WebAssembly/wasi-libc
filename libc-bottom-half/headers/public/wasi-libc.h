#ifndef __wasilibc_wasilibc_h
#define __wasilibc_wasilibc_h

#ifdef __cplusplus
extern "C" {
#endif

int __wasilibc_register_preopened_fd(int fd, const char *path);
int __wasilibc_fd_renumber(int fd, int newfd);

#ifdef __cplusplus
}
#endif

#endif
