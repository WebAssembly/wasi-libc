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

_Noreturn void __imported_wasi_snapshot_preview1_proc_exit(int32_t arg0) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("proc_exit")
));

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
){
    __imported_wasi_snapshot_preview1_proc_exit((int32_t) rval);
}

