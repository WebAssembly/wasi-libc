#ifndef __WASI_REPORT_ERROR_H
#define __WASI_REPORT_ERROR_H

#include <features.h>
#include <wasi/api.h>

// Reports an error using the system's typical error output.
hidden void __wasilibc_report_error(const char* message);

#ifdef __webp2__

// Flush all valid content in one of our linebuffers (leaving trailing
// multibyte content alone to be completed later).
void __wasilibc_webp2_fflush(int fd);

// Finalizes and flushes the contents of stdout & stderr in preparation for app
// exit. Optionally also runs `fflush` on musl's stdio streams first.
void __wasilibc_webp2_finish_and_flush_stdio(bool also_musl);

// Reports an error to the console using webp2 bindings.
void __wasilibc_webp2_report_error(const char* message);

#define WEBP2_UNSUPPORTED(name)                                    \
  do {                                                             \
    __wasilibc_report_error(name " is not supported on the web");  \
    __builtin_trap();                                              \
  } while (0)
#endif

#endif
