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

int32_t __imported_wasi_snapshot_preview1_path_filestat_set_times(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int64_t arg4, int64_t arg5, int32_t arg6) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_filestat_set_times")
));

__wasi_errno_t __wasi_path_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasi_snapshot_preview1_path_filestat_set_times((int32_t) fd, flags, (int32_t) path, (int32_t) path_len, (int64_t) atim, (int64_t) mtim, fst_flags);
    return (uint16_t) ret;
}

