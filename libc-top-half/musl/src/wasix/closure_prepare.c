#include <wasix/closure.h>
#include <errno.h>

int wasix_closure_prepare(
    wasix_function_pointer backing_function_id,
    wasix_function_pointer closure_id,
    const wasix_value_type *argument_types,
    size_t argument_types_len,
    const wasix_value_type *result_types,
    size_t result_types_len,
    void *user_data_ptr)
{
    int err = __wasi_closure_prepare(
        backing_function_id,
        closure_id,
        argument_types,
        argument_types_len,
        result_types,
        result_types_len,
        (uint8_t *)user_data_ptr);

    if (err != __WASI_ERRNO_SUCCESS)
    {
        errno = err;
        return -1;
    }

    return 0;
}