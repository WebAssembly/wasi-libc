#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

gid_t getegid(void) {
    return 0;
}
