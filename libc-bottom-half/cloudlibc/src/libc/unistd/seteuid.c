#include <wasi/api.h>
#include <wasi/libc.h>
#include <errno.h>
#include <unistd.h>

int seteuid(uid_t uid) {
    return 0;
}
