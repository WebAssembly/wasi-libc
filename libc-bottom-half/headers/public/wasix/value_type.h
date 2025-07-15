#ifndef wasix_value_type_h
#define wasix_value_type_h
// Dynamic types used by the wasix runtime types APIs

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

#define wasix_value_type __wasi_wasm_value_type_t

#ifdef __cplusplus
}
#endif

#endif