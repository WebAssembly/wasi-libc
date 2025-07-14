#include <wasix/dyncall.h>
#include <errno.h>

int wasix_closure_allocate(wasix_function_pointer_t *ret_closure)
{
    int err = __wasi_closure_allocate(ret_closure);

    if (err != __WASI_ERRNO_SUCCESS)
    {
        errno = err;
        return -1;
    }

    return 0;
}