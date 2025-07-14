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

#define wasix_closure_prepare __wasi_closure_prepare
#define wasix_closure_allocate __wasi_closure_allocate
#define wasix_closure_free __wasi_closure_free

#endif