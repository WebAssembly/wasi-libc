// There is no way to create new file descriptors on the web.

#include "wasi/report-error.h"
#include <unistd.h>

int pipe(int fd[2]) { WEBP2_UNSUPPORTED("pipe"); }

int pipe2(int fd[2], int flags) { WEBP2_UNSUPPORTED("pipe2"); }
