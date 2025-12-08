#ifndef WASI_STDIO_H
#define WASI_STDIO_H

#ifdef __wasilibc_use_wasip2

/// Initialization hook which adds stdin/stdout/stderr fds to the descriptor
/// table.
int __wasilibc_init_stdio(void);

#endif // __wasilibc_use_wasip2

#endif // WASI_STDIO_H
