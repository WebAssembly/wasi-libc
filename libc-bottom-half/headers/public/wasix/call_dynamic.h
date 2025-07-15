#ifndef wasix_call_dynamic_h
#define wasix_call_dynamic_h
// Wrappers for the dynamic call interface.
//
// Currently they are just aliases for the raw syscall, but this may change in the future if we need to
// add additional logic before or after the call is made.

#include <wasi/api_wasi.h>
#include <wasi/api_wasix.h>
#include <wasix/function_pointer.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// TODO: Document how parameter mismatch is handled vs internal errors.
/**
 * Call a function pointer with dynamic parameters.
 * 
 * Returns:
 *  0 on success
 *  -1 on error, with errno set to the error code.
 */
int wasix_call_dynamic(
    /**
     * An index into the __indirect_function_table
     *
     * Your module needs to either import or export the table to be able
     * to call this function.
     */
    wasix_function_pointer function_id,
    /**
     * A buffer with the parameters to pass to the function.
     * This buffer is expected to contain all parameters sequentially
     *
     * For example if the function takes an i32 and an i64, the
     * buffer will be 12 bytes long, with the first 4 bytes
     * being the i32 and the next 8 bytes being the i64.
     */
    void *values,
    /**
     * The length of the array pointed to by `values`.
     */
    size_t values_len,
    /**
     * A pointer to a buffer for the results of the function call.
     *
     * In most cases this will be a single value, but it could also
     * contain multiple values. The same rules apply as for the
     * parameters, i.e. the buffer needs to be large enough
     * to hold all the results sequentially.
     */
    void *results,
    /**
     * The length of the array pointed to by `results`.
     */
    size_t results_len,
    /**
     * If this is set to true, the function will return an error if the
     * length and types of the parameters and results do not match the
     * function signature.
     *
     * If this is set to false, the function will not perform any checks.
     * Any missing bytes will be assumed to be zero and any extra bytes
     * will be ignored.
     */
    bool strict);

#ifdef __cplusplus
}
#endif
#endif