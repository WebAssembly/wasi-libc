#ifndef WASI_WEBP2_STDIO_H
#define WASI_WEBP2_STDIO_H

#include <wasi/version.h>

#ifdef __webp2__

#include <features.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/uio.h>

// Flush all valid content in one of our linebuffers (leaving trailing
// multibyte content alone to be completed later).
hidden void __wasilibc_webp2_fflush(int fd);

// Finalizes and flushes the contents of stdout & stderr in preparation for app
// exit. Optionally also runs `fflush` on musl's stdio streams first.
hidden void __wasilibc_webp2_flush_and_close_stdio(bool also_musl);

#endif // __webp2__

#endif // WASI_WEBP2_STDIO_H
