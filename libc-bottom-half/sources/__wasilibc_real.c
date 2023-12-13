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

#ifdef __wasm64__
#define IMPORT_NAME(x) __import_name__(x "-i64")
#else
#define IMPORT_NAME(x) __import_name__(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

int32_t __imported_wasi_snapshot_preview1_args_get(intptr_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("args_get")
));

__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_args_get((intptr_t) argv, (intptr_t) argv_buf);
}

int32_t __imported_wasi_snapshot_preview1_args_sizes_get(intptr_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("args_sizes_get")
));

__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_args_sizes_get((intptr_t) retptr0, (intptr_t) retptr1);
}

int32_t __imported_wasi_snapshot_preview1_environ_get(intptr_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("environ_get")
));

__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_environ_get((intptr_t) environ, (intptr_t) environ_buf);
}

int32_t __imported_wasi_snapshot_preview1_environ_sizes_get(intptr_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("environ_sizes_get")
));

__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_environ_sizes_get((intptr_t) retptr0, (intptr_t) retptr1);
}

int32_t __imported_wasi_snapshot_preview1_clock_res_get(int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("clock_res_get")
));

__wasi_errno_t __wasi_clock_res_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_clock_res_get((int32_t) id, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_clock_time_get(int32_t, int64_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("clock_time_get")
));

__wasi_errno_t __wasi_clock_time_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_clock_time_get((int32_t) id, (int64_t) precision, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_advise(int32_t, int64_t, int64_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_advise")
));

__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_advise((int32_t) fd, (int64_t) offset, (int64_t) len, (int32_t) advice);
}

int32_t __imported_wasi_snapshot_preview1_fd_allocate(int32_t, int64_t, int64_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_allocate")
));

__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_allocate((int32_t) fd, (int64_t) offset, (int64_t) len);
}

int32_t __imported_wasi_snapshot_preview1_fd_close(int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_close")
));

__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_close((int32_t) fd);
}

int32_t __imported_wasi_snapshot_preview1_fd_datasync(int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_datasync")
));

__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_datasync((int32_t) fd);
}

int32_t __imported_wasi_snapshot_preview1_fd_fdstat_get(int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_fdstat_get")
));

__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_get((int32_t) fd, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_fdstat_set_flags(int32_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_fdstat_set_flags")
));

__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_set_flags((int32_t) fd, (int32_t) flags);
}

int32_t __imported_wasi_snapshot_preview1_fd_fdstat_set_rights(int32_t, int64_t, int64_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_fdstat_set_rights")
));

__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_set_rights((int32_t) fd, (int64_t) fs_rights_base, (int64_t) fs_rights_inheriting);
}

int32_t __imported_wasi_snapshot_preview1_fd_filestat_get(int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_filestat_get")
));

__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_get((int32_t) fd, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_filestat_set_size(int32_t, int64_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_filestat_set_size")
));

__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    __wasi_filesize_t size
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_set_size((int32_t) fd, (int64_t) size);
}

int32_t __imported_wasi_snapshot_preview1_fd_filestat_set_times(int32_t, int64_t, int64_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_filestat_set_times")
));

__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_set_times((int32_t) fd, (int64_t) atim, (int64_t) mtim, (int32_t) fst_flags);
}

int32_t __imported_wasi_snapshot_preview1_fd_pread(int32_t, intptr_t, __wasi_size_t, int64_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_pread")
));

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    __wasi_iovec_t const *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_pread((int32_t) fd, (intptr_t) iovs, (__wasi_size_t) iovs_len, (int64_t) offset, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_prestat_get(int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_prestat_get")
));

__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_prestat_get((int32_t) fd, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_prestat_dir_name(int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_prestat_dir_name")
));

__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    uint8_t * path,
    __wasi_size_t path_len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_prestat_dir_name((int32_t) fd, (intptr_t) path, (__wasi_size_t) path_len);
}

int32_t __imported_wasi_snapshot_preview1_fd_pwrite(int32_t, intptr_t, __wasi_size_t, int64_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_pwrite")
));

__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_pwrite((int32_t) fd, (intptr_t) iovs, (__wasi_size_t) iovs_len, (int64_t) offset, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_read(int32_t, intptr_t, __wasi_size_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_read")
));

__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    __wasi_iovec_t const *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_read((int32_t) fd, (intptr_t) iovs, (__wasi_size_t) iovs_len, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_readdir(int32_t, intptr_t, __wasi_size_t, int64_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_readdir")
));

__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_readdir((int32_t) fd, (intptr_t) buf, (__wasi_size_t) buf_len, (int64_t) cookie, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_renumber(int32_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_renumber")
));

__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    __wasi_fd_t to
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_renumber((int32_t) fd, (int32_t) to);
}

int32_t __imported_wasi_snapshot_preview1_fd_seek(int32_t, int64_t, int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_seek")
));

__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_seek((int32_t) fd, (int64_t) offset, (int32_t) whence, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_sync(int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_sync")
));

__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_sync((int32_t) fd);
}

int32_t __imported_wasi_snapshot_preview1_fd_tell(int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_tell")
));

__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_tell((int32_t) fd, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_fd_write(int32_t, intptr_t, __wasi_size_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("fd_write")
));

__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_write((int32_t) fd, (intptr_t) iovs, (__wasi_size_t) iovs_len, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_path_create_directory(int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_create_directory")
));

__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_create_directory((int32_t) fd, (intptr_t) path, (__wasi_size_t) path_len);
}

int32_t __imported_wasi_snapshot_preview1_path_filestat_get(int32_t, int32_t, intptr_t, __wasi_size_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_filestat_get")
));

__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    char const *path,
    __wasi_filestat_t *retptr0
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_filestat_get((int32_t) fd, (int32_t) flags, (intptr_t) path, (__wasi_size_t) path_len, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_path_filestat_set_times(int32_t, int32_t, intptr_t, __wasi_size_t, int64_t, int64_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_filestat_set_times")
));

__wasi_errno_t __wasi_path_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    char const *path,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_filestat_set_times((int32_t) fd, (int32_t) flags, (intptr_t) path, (__wasi_size_t) path_len, (int64_t) atim, (int64_t) mtim, (int32_t) fst_flags);
}

int32_t __imported_wasi_snapshot_preview1_path_link(int32_t, int32_t, intptr_t, __wasi_size_t, int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_link")
));

__wasi_errno_t __wasi_path_link(
    __wasi_fd_t old_fd,
    __wasi_lookupflags_t old_flags,
    char const *old_path,
    __wasi_fd_t new_fd,
    char const *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_link((int32_t) old_fd, (int32_t) old_flags, (intptr_t) old_path, (__wasi_size_t) old_path_len, (int32_t) new_fd, (intptr_t) new_path, (__wasi_size_t) new_path_len);
}

int32_t __imported_wasi_snapshot_preview1_path_open(int32_t, int32_t, intptr_t, __wasi_size_t, int32_t, int64_t, int64_t, int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_open")
));

__wasi_errno_t __wasi_path_open(
    __wasi_fd_t fd,
    __wasi_lookupflags_t dirflags,
    char const *path,
    __wasi_oflags_t oflags,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting,
    __wasi_fdflags_t fdflags,
    __wasi_fd_t *retptr0
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_open((int32_t) fd, (int32_t) dirflags, (intptr_t) path, (__wasi_size_t) path_len, (int32_t) oflags, (int64_t) fs_rights_base, (int64_t) fs_rights_inheriting, (int32_t) fdflags, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_path_readlink(int32_t, intptr_t, __wasi_size_t, intptr_t, __wasi_size_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_readlink")
));

__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    char const *path,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_size_t *retptr0
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_readlink((int32_t) fd, (intptr_t) path, (__wasi_size_t) path_len, (intptr_t) buf, (__wasi_size_t) buf_len, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_path_remove_directory(int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_remove_directory")
));

__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_remove_directory((int32_t) fd, (intptr_t) path, (__wasi_size_t) path_len);
}

int32_t __imported_wasi_snapshot_preview1_path_rename(int32_t, intptr_t, __wasi_size_t, int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_rename")
));

__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    char const *old_path,
    __wasi_fd_t new_fd,
    char const *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_rename((int32_t) fd, (intptr_t) old_path, (__wasi_size_t) old_path_len, (int32_t) new_fd, (intptr_t) new_path, (__wasi_size_t) new_path_len);
}

int32_t __imported_wasi_snapshot_preview1_path_symlink(intptr_t, __wasi_size_t, int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_symlink")
));

__wasi_errno_t __wasi_path_symlink(
    char const *old_path,
    __wasi_fd_t fd,
    char const *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_symlink((intptr_t) old_path, (__wasi_size_t) old_path_len, (int32_t) fd, (intptr_t) new_path, (__wasi_size_t) new_path_len);
}

int32_t __imported_wasi_snapshot_preview1_path_unlink_file(int32_t, intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("path_unlink_file")
));

__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_unlink_file((int32_t) fd, (intptr_t) path, (__wasi_size_t) path_len);
}

int32_t __imported_wasi_snapshot_preview1_poll_oneoff(intptr_t, intptr_t, int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("poll_oneoff")
));

__wasi_errno_t __wasi_poll_oneoff(
    __wasi_subscription_t const * in,
    __wasi_event_t * out,
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_poll_oneoff((intptr_t) in, (intptr_t) out, (int32_t) nsubscriptions, (intptr_t) retptr0);
}

_Noreturn void __imported_wasi_snapshot_preview1_proc_exit(int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("proc_exit")
));

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
){
    __imported_wasi_snapshot_preview1_proc_exit((int32_t) rval);
}

int32_t __imported_wasi_snapshot_preview1_sched_yield() __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("sched_yield")
));

__wasi_errno_t __wasi_sched_yield(
    void
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sched_yield();
}

int32_t __imported_wasi_snapshot_preview1_random_get(intptr_t, __wasi_size_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("random_get")
));

__wasi_errno_t __wasi_random_get(
    uint8_t * buf,
    __wasi_size_t buf_len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_random_get((intptr_t) buf, (__wasi_size_t) buf_len);
}

int32_t __imported_wasi_snapshot_preview1_sock_accept(int32_t, int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("sock_accept")
));

__wasi_errno_t __wasi_sock_accept(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_accept((int32_t) fd, (int32_t) flags, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_sock_recv(int32_t, intptr_t, __wasi_size_t, int32_t, intptr_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("sock_recv")
));

__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    __wasi_iovec_t const *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_recv((int32_t) fd, (intptr_t) ri_data, (__wasi_size_t) ri_data_len, (int32_t) ri_flags, (intptr_t) retptr0, (intptr_t) retptr1);
}

int32_t __imported_wasi_snapshot_preview1_sock_send(int32_t, intptr_t, __wasi_size_t, int32_t, intptr_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("sock_send")
));

__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_send((int32_t) fd, (intptr_t) si_data, (__wasi_size_t) si_data_len, (int32_t) si_flags, (intptr_t) retptr0);
}

int32_t __imported_wasi_snapshot_preview1_sock_shutdown(int32_t, int32_t) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    IMPORT_NAME("sock_shutdown")
));

__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    __wasi_sdflags_t how
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_shutdown((int32_t) fd, (int32_t) how);
}

#ifdef _REENTRANT
uint32_t __imported_wasi_thread_spawn(intptr_t arg0) __WASI_NOEXCEPT __attribute__((
    __import_module__("wasi"),
    IMPORT_NAME("thread-spawn")
));

int32_t __wasi_thread_spawn(void* start_arg) __WASI_NOEXCEPT {
    return (int32_t) __imported_wasi_thread_spawn((intptr_t) start_arg);
}
#endif

#ifdef __cplusplus
}
#endif
