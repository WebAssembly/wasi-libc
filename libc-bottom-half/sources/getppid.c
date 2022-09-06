#include <unistd.h>
#include <errno.h>
#include <string.h>

pid_t getppid(void)
{
    __wasi_pid_t pid = 0;
    __wasi_errno_t error = __wasi_proc_id(&pid);
    if (error != 0) {
      errno = error;
      return -1;
    }

    __wasi_pid_t parent = 0;
    error = __wasi_proc_parent(pid, &parent);
    if (error != 0) {
      errno = error;
      return -1;
    }

    return parent;
}