#ifndef WASI_STDIO_H
#define WASI_STDIO_H

#include <wasi/version.h>

#ifndef __wasip1__

/// Initialization hook which adds stdin/stdout/stderr fds to the descriptor
/// table.
int __wasilibc_init_stdio(void);

#endif // not(__wasip1__)

#endif // WASI_STDIO_H
