#ifndef wasix_call_dynamic_h
#define wasix_call_dynamic_h

#include <wasi/api_wasi.h>
#include <wasi/api_wasix.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define WASIX_VALUE_TYPE_I32 __WASI_WASM_VALUE_TYPE_I32
#define WASIX_VALUE_TYPE_I64 __WASI_WASM_VALUE_TYPE_I64
#define WASIX_VALUE_TYPE_F32 __WASI_WASM_VALUE_TYPE_F32
#define WASIX_VALUE_TYPE_F64 __WASI_WASM_VALUE_TYPE_F64
#define WASIX_VALUE_TYPE_V128 __WASI_WASM_VALUE_TYPE_V128

#define wasix_value_type_t __wasi_wasm_value_type_t

#define wasix_function_pointer_t __wasi_function_pointer_t

int wasix_call_dynamic(
    wasix_function_pointer_t function_id,
    void *values,
    size_t values_len,
    void *results,
    size_t results_len);

int wasix_closure_allocate(wasix_function_pointer_t *ret_closure);

int wasix_closure_prepare(
    wasix_function_pointer_t backing_function_id,
    wasix_function_pointer_t closure_id,
    const wasix_value_type_t *argument_types,
    size_t argument_types_len,
    const wasix_value_type_t *result_types,
    size_t result_types_len,
    void *user_data_ptr);

int wasix_closure_free(wasix_function_pointer_t closure);

#ifdef __cplusplus
}
#endif

#endif