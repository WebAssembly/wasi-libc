#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

uid_t getuid(void) {
    return 0;
}
