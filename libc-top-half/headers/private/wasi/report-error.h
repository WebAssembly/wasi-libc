#ifndef __WASI_REPORT_ERROR_H
#define __WASI_REPORT_ERROR_H

#include <features.h>
#include <wasi/api.h>

// Reports an error using the system's typical error output.
hidden void __wasilibc_report_error(const char* message);

#ifdef __webp2__
// Flushes any remaining content in stdout/stderr, optionally also running
// `fflush` on musl's stdio streams to ensure that absolutely everything goes
// out to the console.
void __wasilibc_webp2_flush_stdio(bool also_musl);
void __wasilibc_webp2_fflush(int fd, bool also_musl);

// Reports an error to the console using webp2 bindings.
void __wasilibc_webp2_report_error(const char* message);

#define WEBP2_UNSUPPORTED(name)                                    \
  do {                                                             \
    __wasilibc_webp2_flush_stdio(true);                            \
    __wasilibc_report_error(name " is not supported on the web");  \
    __builtin_trap();                                              \
  } while (0)
#endif

#endif
