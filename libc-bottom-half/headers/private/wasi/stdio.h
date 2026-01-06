#ifndef WASI_STDIO_H
#define WASI_STDIO_H

#include <wasi/version.h>

#ifdef __wasip2__

/// Initialization hook which adds stdin/stdout/stderr fds to the descriptor
/// table.
int __wasilibc_init_stdio(void);

#endif // __wasip2__

#endif // WASI_STDIO_H
