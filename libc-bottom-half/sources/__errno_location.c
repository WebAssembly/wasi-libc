#include <errno.h>

// `<errno.h>` may define `errno` as a call to this function, so reach the
// underlying thread-local directly here to avoid recursing into ourselves.
#undef errno
extern _Thread_local int errno;

int *__errno_location(void) { return &errno; }
