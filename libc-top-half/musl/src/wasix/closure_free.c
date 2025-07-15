#include <wasix/closure.h>
#include <errno.h>

int wasix_closure_free(wasix_function_pointer closure)
{
    int err = __wasi_closure_free(closure);

    if (err != __WASI_ERRNO_SUCCESS)
    {
        errno = err;
        return -1;
    }

    return 0;
}