#ifndef wasix_closure_h
#define wasix_closure_h
// Wrappers for the closures interface
//
// Currently they are just aliases for the raw syscall, but this may change in the future if we need to
// add additional logic before or after the call is made.

#include <wasi/api_wasi.h>
#include <wasi/api_wasix.h>
#include <wasix/function_pointer.h>
#include <wasix/value_type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Prepare a closure for execution.
 */
int wasix_closure_prepare(
    /**
     * A index into the indirect function table that points to the function that will be called when the closure is executed.
     *
     * That function needs to conform to the following signature:
     *   uint8_t* values - a pointer to a buffer containing the arguments. See call_dynamic for more details.
     *   uint8_t* results - a pointer to a buffer where the results will be written. See call_dynamic for more details.
     *   void* user_data_ptr - the user_data_ptr that was passed to closure_prepare
     */
    wasix_function_pointer backing_function_id,
    /**
     * An index into the indirect function table that was previously allocated with closure_alloc
     *
     * After closure_prepare the slot in the indirect function table will contain a funcref to a closure with the requested signature.
     * Every call to the closure will be translated to a call to the backing function.
     */
    wasix_function_pointer closure_id,
    /**
     * A list of types of the arguments that the closure will take.
     */
    const wasix_value_type *argument_types,
    /**
     * The length of the array pointed to by `argument_types`.
     */
    size_t argument_types_len,
    /**
     * A list of types that the closure will return.
     */
    const wasix_value_type *result_types,
    /**
     * The length of the array pointed to by `result_types`.
     */
    size_t result_types_len,
    /**
     * A pointer to a buffer that will be passed to the closure when it is executed.
     */
    void *user_data_ptr);

/**
 * Allocate a closure for use with the closure_prepare function.
 */
int wasix_closure_allocate(
    wasix_function_pointer *ret_closure);

/**
 * Free a closure that was previously allocated with closure_allocate.
 *
 * After this call it is undefined what happens when you call the funcref at the index specified by closure_id.
 */
int wasix_closure_free(
    /**
     * An index into the indirect function table that was previously allocated with closure_allocate
     */
    wasix_function_pointer closure);

#ifdef __cplusplus
}
#endif
#endif