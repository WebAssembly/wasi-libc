#ifndef ERRNO_H
#define ERRNO_H

#include "../../include/errno.h"

#ifdef __wasm_musl_unmodified_upstream__
hidden int *___errno_location(void);

#undef errno
#define errno (*___errno_location())
#else
// Use the COWS libc errno.
#endif

#endif
