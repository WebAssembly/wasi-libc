/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   wasi_snapshot_preview1.witx
 *
 * To regenerate this file execute:
 *
 *     cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
 *
 * Modifications to this file will cause CI to fail, the code generator tool
 * must be modified to change this file.
 */

#include <wasi/api.h>
#include <string.h>

int32_t __imported_wasi_snapshot_preview1_environ_sizes_get(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("environ_sizes_get")
));

__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    int32_t ret = __imported_wasi_snapshot_preview1_environ_sizes_get((int32_t) retptr0, (int32_t) retptr1);
    return (uint16_t) ret;
}

