#include <wasix/closure.h>
#include <errno.h>

/**
 * Allocate a closure for use with the closure_prepare function.
 */
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