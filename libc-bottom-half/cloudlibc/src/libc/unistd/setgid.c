#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int setgid(gid_t gid) {
    return 0;
}
