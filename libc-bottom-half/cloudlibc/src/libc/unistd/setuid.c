#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int setuid(uid_t uid) {
    return 0;
}
