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

int32_t __imported_wasi_snapshot_preview1_fd_pread(int32_t arg0, int32_t arg1, int32_t arg2, int64_t arg3, int32_t arg4) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_pread")
));

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasi_snapshot_preview1_fd_pread((int32_t) fd, (int32_t) iovs, (int32_t) iovs_len, (int64_t) offset, (int32_t) retptr0);
    return (uint16_t) ret;
}

