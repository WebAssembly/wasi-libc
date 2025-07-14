#ifndef wasix_call_dynamic_h
#define wasix_call_dynamic_h
// Wrappers for the dynamic call interface.
//
// Currently they are just aliases for the raw syscall, but this may change in the future if we need to
// add additional logic before or after the call is made.

#include <wasi/api_wasi.h>
#include <wasi/api_wasix.h>
#include <wasix/function_pointer.h>

#define wasix_call_dynamic __wasi_call_dynamic

#endif