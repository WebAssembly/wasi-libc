#include <wasix/dyncall.h>
#include <errno.h>

int wasix_call_dynamic(
    wasix_function_pointer_t function_id,
    void *values,
    size_t values_len,
    void *results,
    size_t results_len)
{
    int err = __wasi_call_dynamic(
        function_id,
        (uint8_t *)values,
        values_len,
        (uint8_t *)results,
        results_len);

    if (err != __WASI_ERRNO_SUCCESS)
    {
        errno = err;
        return -1;
    }

    return 0;
}