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

int32_t __imported_wasi_snapshot_preview1_path_rename(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_rename")
));

__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    const char *old_path,
    __wasi_fd_t new_fd,
    const char *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    int32_t ret = __imported_wasi_snapshot_preview1_path_rename((int32_t) fd, (int32_t) old_path, (int32_t) old_path_len, (int32_t) new_fd, (int32_t) new_path, (int32_t) new_path_len);
    return (uint16_t) ret;
}

