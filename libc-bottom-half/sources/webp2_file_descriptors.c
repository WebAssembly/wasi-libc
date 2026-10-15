// There is no way to create new file descriptors on the web.

#include "wasi/report-error.h"
#include <unistd.h>

int __dup3(int fd, int newfd, int flags) { WEBP2_UNSUPPORTED("__dup3"); }

int dup(int fd) { WEBP2_UNSUPPORTED("dup"); }

int dup2(int fd, int newfd) { WEBP2_UNSUPPORTED("dup2"); }

int dup3(int fd, int newfd, int flags) { WEBP2_UNSUPPORTED("dup3"); }

int pipe(int fd[2]) { WEBP2_UNSUPPORTED("pipe"); }

int pipe2(int fd[2], int flags) { WEBP2_UNSUPPORTED("pipe2"); }
