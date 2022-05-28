/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   wasix_snapshot_preview1.witx
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

int32_t __imported_wasix_snapshot_preview1_args_get(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("args_get")
));

__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
){
    int32_t ret = __imported_wasix_snapshot_preview1_args_get((int64_t) argv, (int64_t) argv_buf);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_args_sizes_get(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("args_sizes_get")
));

__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    int32_t ret = __imported_wasix_snapshot_preview1_args_sizes_get((intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_environ_get(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("environ_get")
));

__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
){
    int32_t ret = __imported_wasix_snapshot_preview1_environ_get((int64_t) environ, (int64_t) environ_buf);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_environ_sizes_get(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("environ_sizes_get")
));

__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    int32_t ret = __imported_wasix_snapshot_preview1_environ_sizes_get((intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_clock_res_get(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("clock_res_get")
));

__wasi_errno_t __wasi_clock_res_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_clock_res_get((int32_t) id, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_clock_time_get(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("clock_time_get")
));

__wasi_errno_t __wasi_clock_time_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_clock_time_get((int32_t) id, (int64_t) precision, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_advise(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_advise")
));

__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_advise((int32_t) fd, (int64_t) offset, (int64_t) len, (int32_t) advice);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_allocate(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_allocate")
));

__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_allocate((int32_t) fd, (int64_t) offset, (int64_t) len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_close(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_close")
));

__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_close((int32_t) fd);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_datasync(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_datasync")
));

__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_datasync((int32_t) fd);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_fdstat_get(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_fdstat_get")
));

__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_fdstat_get((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_fdstat_set_flags(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_fdstat_set_flags")
));

__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_fdstat_set_flags((int32_t) fd, flags);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_fdstat_set_rights(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_fdstat_set_rights")
));

__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_fdstat_set_rights((int32_t) fd, fs_rights_base, fs_rights_inheriting);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_filestat_get(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_filestat_get")
));

__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_filestat_get((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_filestat_set_size(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_filestat_set_size")
));

__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    __wasi_filesize_t size
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_filestat_set_size((int32_t) fd, (int64_t) size);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_filestat_set_times(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_filestat_set_times")
));

__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_filestat_set_times((int32_t) fd, (int64_t) atim, (int64_t) mtim, fst_flags);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_pread(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_pread")
));

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_pread((int32_t) fd, (intptr_t) iovs, (intptr_t) iovs_len, (int64_t) offset, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_prestat_get(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_prestat_get")
));

__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_prestat_get((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_prestat_dir_name(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_prestat_dir_name")
));

__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    uint8_t * path,
    __wasi_pointersize_t path_len
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_prestat_dir_name((int32_t) fd, (int64_t) path, (int64_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_pwrite(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_pwrite")
));

__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_pwrite((int32_t) fd, (intptr_t) iovs, (intptr_t) iovs_len, (int64_t) offset, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_read(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_read")
));

__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_read((int32_t) fd, (intptr_t) iovs, (intptr_t) iovs_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_readdir(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_readdir")
));

__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    uint8_t * buf,
    __wasi_pointersize_t buf_len,
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_readdir((int32_t) fd, (int64_t) buf, (int64_t) buf_len, (int64_t) cookie, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_renumber(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_renumber")
));

__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    __wasi_fd_t to
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_renumber((int32_t) fd, (int32_t) to);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_dup(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_dup")
));

__wasi_errno_t __wasi_fd_dup(
    __wasi_fd_t fd,
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_dup((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_seek(int32_t arg0, int64_t arg1, int32_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_seek")
));

__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_seek((int32_t) fd, offset, (int32_t) whence, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_sync(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_sync")
));

__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_sync((int32_t) fd);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_tell(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_tell")
));

__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_tell((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_fd_write(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("fd_write")
));

__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_fd_write((int32_t) fd, (intptr_t) iovs, (intptr_t) iovs_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_pipe(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("pipe")
));

__wasi_errno_t __wasi_pipe(
    __wasi_fd_t *retptr0,
    __wasi_fd_t *retptr1
){
    int32_t ret = __imported_wasix_snapshot_preview1_pipe((intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_create_directory(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_create_directory")
));

__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    const char *path
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_create_directory((int32_t) fd, (intptr_t) path, (intptr_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_filestat_get(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_filestat_get")
));

__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    __wasi_filestat_t *retptr0
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_filestat_get((int32_t) fd, flags, (intptr_t) path, (intptr_t) path_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_filestat_set_times(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int32_t arg6) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
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
    int32_t ret = __imported_wasix_snapshot_preview1_path_filestat_set_times((int32_t) fd, flags, (intptr_t) path, (intptr_t) path_len, (int64_t) atim, (int64_t) mtim, fst_flags);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_link(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3, int32_t arg4, int64_t arg5, int64_t arg6) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_link")
));

__wasi_errno_t __wasi_path_link(
    __wasi_fd_t old_fd,
    __wasi_lookupflags_t old_flags,
    const char *old_path,
    __wasi_fd_t new_fd,
    const char *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_link((int32_t) old_fd, old_flags, (intptr_t) old_path, (intptr_t) old_path_len, (int32_t) new_fd, (intptr_t) new_path, (intptr_t) new_path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_open(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3, int32_t arg4, int64_t arg5, int64_t arg6, int32_t arg7, int64_t arg8) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_open")
));

__wasi_errno_t __wasi_path_open(
    __wasi_fd_t fd,
    __wasi_lookupflags_t dirflags,
    const char *path,
    __wasi_oflags_t oflags,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting,
    __wasi_fdflags_t fdflags,
    __wasi_fd_t *retptr0
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_open((int32_t) fd, dirflags, (intptr_t) path, (intptr_t) path_len, oflags, fs_rights_base, fs_rights_inheriting, fdflags, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_readlink(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_readlink")
));

__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    const char *path,
    uint8_t * buf,
    __wasi_pointersize_t buf_len,
    __wasi_size_t *retptr0
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_readlink((int32_t) fd, (intptr_t) path, (intptr_t) path_len, (int64_t) buf, (int64_t) buf_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_remove_directory(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_remove_directory")
));

__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    const char *path
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_remove_directory((int32_t) fd, (intptr_t) path, (intptr_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_rename(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
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
    int32_t ret = __imported_wasix_snapshot_preview1_path_rename((int32_t) fd, (intptr_t) old_path, (intptr_t) old_path_len, (int32_t) new_fd, (intptr_t) new_path, (intptr_t) new_path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_symlink(int64_t arg0, int64_t arg1, int32_t arg2, int64_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_symlink")
));

__wasi_errno_t __wasi_path_symlink(
    const char *old_path,
    __wasi_fd_t fd,
    const char *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_symlink((intptr_t) old_path, (intptr_t) old_path_len, (int32_t) fd, (intptr_t) new_path, (intptr_t) new_path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_path_unlink_file(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("path_unlink_file")
));

__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    const char *path
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_path_unlink_file((int32_t) fd, (intptr_t) path, (intptr_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_poll_oneoff(int64_t arg0, int64_t arg1, int32_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("poll_oneoff")
));

__wasi_errno_t __wasi_poll_oneoff(
    const __wasi_subscription_t * in,
    __wasi_event_t * out,
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_poll_oneoff((int64_t) in, (int64_t) out, (int32_t) nsubscriptions, (intptr_t) retptr0);
    return (uint16_t) ret;
}

_Noreturn void __imported_wasix_snapshot_preview1_proc_exit(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("proc_exit")
));

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
){
    __imported_wasix_snapshot_preview1_proc_exit((int32_t) rval);
}

int32_t __imported_wasix_snapshot_preview1_proc_raise(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("proc_raise")
));

__wasi_errno_t __wasi_proc_raise(
    __wasi_signal_t sig
){
    int32_t ret = __imported_wasix_snapshot_preview1_proc_raise((int32_t) sig);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sched_yield() __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sched_yield")
));

__wasi_errno_t __wasi_sched_yield(
    void
){
    int32_t ret = __imported_wasix_snapshot_preview1_sched_yield();
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_random_get(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("random_get")
));

__wasi_errno_t __wasi_random_get(
    uint8_t * buf,
    __wasi_pointersize_t buf_len
){
    int32_t ret = __imported_wasix_snapshot_preview1_random_get((int64_t) buf, (int64_t) buf_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_tty_get(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("tty_get")
));

__wasi_errno_t __wasi_tty_get(
    __wasi_tty_t * state
){
    int32_t ret = __imported_wasix_snapshot_preview1_tty_get((int64_t) state);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_tty_set(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("tty_set")
));

__wasi_errno_t __wasi_tty_set(
    __wasi_tty_t * state
){
    int32_t ret = __imported_wasix_snapshot_preview1_tty_set((int64_t) state);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_getcwd(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("getcwd")
));

__wasi_errno_t __wasi_getcwd(
    uint8_t * path,
    __wasi_pointersize_t * path_len
){
    int32_t ret = __imported_wasix_snapshot_preview1_getcwd((int64_t) path, (int64_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_chdir(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("chdir")
));

__wasi_errno_t __wasi_chdir(
    const char *path
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_snapshot_preview1_chdir((intptr_t) path, (intptr_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_thread_spawn(int64_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_spawn")
));

__wasi_errno_t __wasi_thread_spawn(
    const char *name,
    uint64_t user_data,
    __wasi_bool_t reactor,
    __wasi_tid_t *retptr0
){
    size_t name_len = strlen(name);
    int32_t ret = __imported_wasix_snapshot_preview1_thread_spawn((intptr_t) name, (intptr_t) name_len, (int64_t) user_data, (int32_t) reactor, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_thread_sleep(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_sleep")
));

__wasi_errno_t __wasi_thread_sleep(
    __wasi_timestamp_t duration
){
    int32_t ret = __imported_wasix_snapshot_preview1_thread_sleep((int64_t) duration);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_thread_id(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_id")
));

__wasi_errno_t __wasi_thread_id(
    __wasi_tid_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_thread_id((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_thread_join(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_join")
));

__wasi_errno_t __wasi_thread_join(
    __wasi_tid_t tid
){
    int32_t ret = __imported_wasix_snapshot_preview1_thread_join((int32_t) tid);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_thread_parallelism(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_parallelism")
));

__wasi_errno_t __wasi_thread_parallelism(
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_thread_parallelism((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_getpid(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("getpid")
));

__wasi_errno_t __wasi_getpid(
    __wasi_pid_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_getpid((intptr_t) retptr0);
    return (uint16_t) ret;
}

_Noreturn void __imported_wasix_snapshot_preview1_thread_exit(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("thread_exit")
));

_Noreturn void __wasi_thread_exit(
    __wasi_exitcode_t rval
){
    __imported_wasix_snapshot_preview1_thread_exit((int32_t) rval);
}

int32_t __imported_wasix_snapshot_preview1_bus_spawn_local(int64_t arg0, int64_t arg1, int32_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6, int32_t arg7, int32_t arg8, int32_t arg9, int64_t arg10, int64_t arg11, int64_t arg12) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_spawn_local")
));

__wasi_bus_error_t __wasi_bus_spawn_local(
    const char *name,
    __wasi_bool_t chroot,
    const char *args,
    const char *preopen,
    __wasi_stdio_mode_t stdin,
    __wasi_stdio_mode_t stdout,
    __wasi_stdio_mode_t stderr,
    const char *working_dir,
    __wasi_bus_handles_t *retptr0
){
    size_t name_len = strlen(name);
    size_t args_len = strlen(args);
    size_t preopen_len = strlen(preopen);
    size_t working_dir_len = strlen(working_dir);
    int32_t ret = __imported_wasix_snapshot_preview1_bus_spawn_local((intptr_t) name, (intptr_t) name_len, (int32_t) chroot, (intptr_t) args, (intptr_t) args_len, (intptr_t) preopen, (intptr_t) preopen_len, (int32_t) stdin, (int32_t) stdout, (int32_t) stderr, (intptr_t) working_dir, (intptr_t) working_dir_len, (intptr_t) retptr0);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_spawn_remote(int64_t arg0, int64_t arg1, int32_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6, int64_t arg7, int64_t arg8, int32_t arg9, int32_t arg10, int32_t arg11, int64_t arg12, int64_t arg13, int64_t arg14, int64_t arg15, int64_t arg16) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_spawn_remote")
));

__wasi_bus_error_t __wasi_bus_spawn_remote(
    const char *name,
    __wasi_bool_t chroot,
    const char *args,
    const char *preopen,
    const char *working_dir,
    __wasi_stdio_mode_t stdin,
    __wasi_stdio_mode_t stdout,
    __wasi_stdio_mode_t stderr,
    const char *instance,
    const char *token,
    __wasi_bus_handles_t *retptr0
){
    size_t name_len = strlen(name);
    size_t args_len = strlen(args);
    size_t preopen_len = strlen(preopen);
    size_t working_dir_len = strlen(working_dir);
    size_t instance_len = strlen(instance);
    size_t token_len = strlen(token);
    int32_t ret = __imported_wasix_snapshot_preview1_bus_spawn_remote((intptr_t) name, (intptr_t) name_len, (int32_t) chroot, (intptr_t) args, (intptr_t) args_len, (intptr_t) preopen, (intptr_t) preopen_len, (intptr_t) working_dir, (intptr_t) working_dir_len, (int32_t) stdin, (int32_t) stdout, (int32_t) stderr, (intptr_t) instance, (intptr_t) instance_len, (intptr_t) token, (intptr_t) token_len, (intptr_t) retptr0);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_close(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_close")
));

__wasi_bus_error_t __wasi_bus_close(
    __wasi_bid_t bid
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_close((int32_t) bid);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_invoke(int32_t arg0, int64_t arg1, int32_t arg2, int64_t arg3, int64_t arg4, int32_t arg5, int64_t arg6, int64_t arg7, int64_t arg8) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_invoke")
));

__wasi_bus_error_t __wasi_bus_invoke(
    __wasi_bid_t bid,
    const __wasi_option_cid_t * parent,
    __wasi_bool_t keep_alive,
    const char *topic,
    __wasi_bus_data_format_t format,
    const uint8_t *buf,
    size_t buf_len,
    __wasi_cid_t *retptr0
){
    size_t topic_len = strlen(topic);
    int32_t ret = __imported_wasix_snapshot_preview1_bus_invoke((int32_t) bid, (int64_t) parent, (int32_t) keep_alive, (intptr_t) topic, (intptr_t) topic_len, (int32_t) format, (intptr_t) buf, (intptr_t) buf_len, (intptr_t) retptr0);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_fault(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_fault")
));

__wasi_bus_error_t __wasi_bus_fault(
    __wasi_cid_t cid,
    __wasi_bus_error_t fault
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_fault((int32_t) cid, (int32_t) fault);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_drop(int32_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_drop")
));

__wasi_bus_error_t __wasi_bus_drop(
    __wasi_cid_t cid
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_drop((int32_t) cid);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_reply(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_reply")
));

__wasi_bus_error_t __wasi_bus_reply(
    __wasi_cid_t cid,
    __wasi_bus_data_format_t format,
    const uint8_t *buf,
    size_t buf_len
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_reply((int32_t) cid, (int32_t) format, (intptr_t) buf, (intptr_t) buf_len);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_callback(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_callback")
));

__wasi_bus_error_t __wasi_bus_callback(
    __wasi_cid_t cid,
    const char *topic,
    __wasi_bus_data_format_t format,
    const uint8_t *buf,
    size_t buf_len
){
    size_t topic_len = strlen(topic);
    int32_t ret = __imported_wasix_snapshot_preview1_bus_callback((int32_t) cid, (intptr_t) topic, (intptr_t) topic_len, (int32_t) format, (intptr_t) buf, (intptr_t) buf_len);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_listen(int64_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_listen")
));

__wasi_bus_error_t __wasi_bus_listen(
    const __wasi_option_cid_t * parent,
    const char *topic
){
    size_t topic_len = strlen(topic);
    int32_t ret = __imported_wasix_snapshot_preview1_bus_listen((int64_t) parent, (intptr_t) topic, (intptr_t) topic_len);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_poll(int64_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_poll")
));

__wasi_bus_error_t __wasi_bus_poll(
    const __wasi_option_bid_t * bid,
    __wasi_timestamp_t timeout,
    __wasi_bus_event_t * events,
    __wasi_size_t nevents,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_poll((int64_t) bid, (int64_t) timeout, (int64_t) events, (int32_t) nevents, (intptr_t) retptr0);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_bus_poll_data(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int64_t arg6) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("bus_poll_data")
));

__wasi_bus_error_t __wasi_bus_poll_data(
    const __wasi_option_bid_t * bid,
    __wasi_timestamp_t timeout,
    uint8_t * topic,
    __wasi_pointersize_t topic_len,
    uint8_t * buf,
    __wasi_pointersize_t buf_len,
    __wasi_bus_event_data_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_bus_poll_data((int64_t) bid, (int64_t) timeout, (int64_t) topic, (int64_t) topic_len, (int64_t) buf, (int64_t) buf_len, (intptr_t) retptr0);
    return (uint32_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_ws_connect(int64_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("ws_connect")
));

__wasi_errno_t __wasi_ws_connect(
    const char *url,
    __wasi_fd_t *retptr0
){
    size_t url_len = strlen(url);
    int32_t ret = __imported_wasix_snapshot_preview1_ws_connect((intptr_t) url, (intptr_t) url_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_http_request(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5, int32_t arg6, int64_t arg7) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("http_request")
));

__wasi_errno_t __wasi_http_request(
    const char *url,
    const char *method,
    const char *headers,
    __wasi_bool_t gzip,
    __wasi_http_handles_t *retptr0
){
    size_t url_len = strlen(url);
    size_t method_len = strlen(method);
    size_t headers_len = strlen(headers);
    int32_t ret = __imported_wasix_snapshot_preview1_http_request((intptr_t) url, (intptr_t) url_len, (intptr_t) method, (intptr_t) method_len, (intptr_t) headers, (intptr_t) headers_len, (int32_t) gzip, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_http_status(int32_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("http_status")
));

__wasi_errno_t __wasi_http_status(
    __wasi_fd_t fd,
    __wasi_http_status_t * status,
    uint8_t * status_text,
    __wasi_pointersize_t * status_text_len,
    uint8_t * headers,
    __wasi_pointersize_t * headers_len
){
    int32_t ret = __imported_wasix_snapshot_preview1_http_status((int32_t) fd, (int64_t) status, (int64_t) status_text, (int64_t) status_text_len, (int64_t) headers, (int64_t) headers_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_bridge(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3, int32_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_bridge")
));

__wasi_errno_t __wasi_port_bridge(
    const char *network,
    const char *token,
    __wasi_stream_security_t security
){
    size_t network_len = strlen(network);
    size_t token_len = strlen(token);
    int32_t ret = __imported_wasix_snapshot_preview1_port_bridge((intptr_t) network, (intptr_t) network_len, (intptr_t) token, (intptr_t) token_len, security);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_unbridge() __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_unbridge")
));

__wasi_errno_t __wasi_port_unbridge(
    void
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_unbridge();
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_dhcp_acquire() __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_dhcp_acquire")
));

__wasi_errno_t __wasi_port_dhcp_acquire(
    void
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_dhcp_acquire();
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_ip_add(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_ip_add")
));

__wasi_errno_t __wasi_port_ip_add(
    const __wasi_addr_cidr_t * ip
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_ip_add((int64_t) ip);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_ip_remove(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_ip_remove")
));

__wasi_errno_t __wasi_port_ip_remove(
    const __wasi_addr_ip_t * ip
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_ip_remove((int64_t) ip);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_ip_clear() __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_ip_clear")
));

__wasi_errno_t __wasi_port_ip_clear(
    void
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_ip_clear();
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_mac(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_mac")
));

__wasi_errno_t __wasi_port_mac(
    __wasi_hardware_address_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_mac((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_ip_list(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_ip_list")
));

__wasi_errno_t __wasi_port_ip_list(
    __wasi_addr_cidr_t * ips,
    __wasi_size_t * nips
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_ip_list((int64_t) ips, (int64_t) nips);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_gateway_set(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_gateway_set")
));

__wasi_errno_t __wasi_port_gateway_set(
    const __wasi_addr_ip_t * ip
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_gateway_set((int64_t) ip);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_route_add(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_route_add")
));

__wasi_errno_t __wasi_port_route_add(
    const __wasi_addr_cidr_t * cidr,
    const __wasi_addr_ip_t * via_router,
    const __wasi_option_timestamp_t * preferred_until,
    const __wasi_option_timestamp_t * expires_at
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_route_add((int64_t) cidr, (int64_t) via_router, (int64_t) preferred_until, (int64_t) expires_at);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_route_remove(int64_t arg0) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_route_remove")
));

__wasi_errno_t __wasi_port_route_remove(
    const __wasi_addr_ip_t * cidr
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_route_remove((int64_t) cidr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_route_clear() __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_route_clear")
));

__wasi_errno_t __wasi_port_route_clear(
    void
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_route_clear();
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_port_route_list(int64_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("port_route_list")
));

__wasi_errno_t __wasi_port_route_list(
    __wasi_route_t * routes,
    __wasi_size_t * nroutes
){
    int32_t ret = __imported_wasix_snapshot_preview1_port_route_list((int64_t) routes, (int64_t) nroutes);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_shutdown(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_shutdown")
));

__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    __wasi_sdflags_t how
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_shutdown((int32_t) fd, how);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_status(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_status")
));

__wasi_errno_t __wasi_sock_status(
    __wasi_fd_t fd,
    __wasi_sock_status_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_status((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_addr_local(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_addr_local")
));

__wasi_errno_t __wasi_sock_addr_local(
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_addr_local((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_addr_peer(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_addr_peer")
));

__wasi_errno_t __wasi_sock_addr_peer(
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_addr_peer((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_open(int32_t arg0, int32_t arg1, int32_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_open")
));

__wasi_errno_t __wasi_sock_open(
    __wasi_address_family_t af,
    __wasi_sock_type_t socktype,
    __wasi_sock_proto_t sock_proto,
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_open((int32_t) af, (int32_t) socktype, (int32_t) sock_proto, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_set_opt_flag(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_set_opt_flag")
));

__wasi_errno_t __wasi_sock_set_opt_flag(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_bool_t flag
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_set_opt_flag((int32_t) fd, (int32_t) sockopt, (int32_t) flag);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_get_opt_flag(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_get_opt_flag")
));

__wasi_errno_t __wasi_sock_get_opt_flag(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_bool_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_get_opt_flag((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_set_opt_time(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_set_opt_time")
));

__wasi_errno_t __wasi_sock_set_opt_time(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    const __wasi_option_timestamp_t * timeout
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_set_opt_time((int32_t) fd, (int32_t) sockopt, (int64_t) timeout);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_get_opt_time(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_get_opt_time")
));

__wasi_errno_t __wasi_sock_get_opt_time(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_option_timestamp_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_get_opt_time((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_set_opt_size(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_set_opt_size")
));

__wasi_errno_t __wasi_sock_set_opt_size(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_filesize_t size
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_set_opt_size((int32_t) fd, (int32_t) sockopt, (int64_t) size);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_get_opt_size(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_get_opt_size")
));

__wasi_errno_t __wasi_sock_get_opt_size(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_filesize_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_get_opt_size((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_join_multicast_v4(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_join_multicast_v4")
));

__wasi_errno_t __wasi_sock_join_multicast_v4(
    __wasi_fd_t fd,
    const __wasi_addr_ip4_t * multiaddr,
    const __wasi_addr_ip4_t * interface
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_join_multicast_v4((int32_t) fd, (int64_t) multiaddr, (int64_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_leave_multicast_v4(int32_t arg0, int64_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_leave_multicast_v4")
));

__wasi_errno_t __wasi_sock_leave_multicast_v4(
    __wasi_fd_t fd,
    const __wasi_addr_ip4_t * multiaddr,
    const __wasi_addr_ip4_t * interface
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_leave_multicast_v4((int32_t) fd, (int64_t) multiaddr, (int64_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_join_multicast_v6(int32_t arg0, int64_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_join_multicast_v6")
));

__wasi_errno_t __wasi_sock_join_multicast_v6(
    __wasi_fd_t fd,
    const __wasi_addr_ip6_t * multiaddr,
    uint32_t interface
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_join_multicast_v6((int32_t) fd, (int64_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_leave_multicast_v6(int32_t arg0, int64_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_leave_multicast_v6")
));

__wasi_errno_t __wasi_sock_leave_multicast_v6(
    __wasi_fd_t fd,
    const __wasi_addr_ip6_t * multiaddr,
    uint32_t interface
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_leave_multicast_v6((int32_t) fd, (int64_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_bind(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_bind")
));

__wasi_errno_t __wasi_sock_bind(
    __wasi_fd_t fd,
    const __wasi_addr_port_t * addr
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_bind((int32_t) fd, (int64_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_listen(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_listen")
));

__wasi_errno_t __wasi_sock_listen(
    __wasi_fd_t fd,
    __wasi_size_t backlog
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_listen((int32_t) fd, (int32_t) backlog);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_accept(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_accept")
));

__wasi_errno_t __wasi_sock_accept(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0,
    __wasi_addr_port_t *retptr1
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_accept((int32_t) fd, flags, (intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_connect(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_connect")
));

__wasi_errno_t __wasi_sock_connect(
    __wasi_fd_t fd,
    const __wasi_addr_port_t * addr
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_connect((int32_t) fd, (int64_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_recv(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_recv")
));

__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    const __wasi_iovec_t *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_recv((int32_t) fd, (intptr_t) ri_data, (intptr_t) ri_data_len, ri_flags, (intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_recv_from(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4, int64_t arg5, int64_t arg6) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_recv_from")
));

__wasi_errno_t __wasi_sock_recv_from(
    __wasi_fd_t fd,
    const __wasi_iovec_t *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1,
    __wasi_addr_port_t *retptr2
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_recv_from((int32_t) fd, (intptr_t) ri_data, (intptr_t) ri_data_len, ri_flags, (intptr_t) retptr0, (intptr_t) retptr1, (intptr_t) retptr2);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_send(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_send")
));

__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_send((int32_t) fd, (intptr_t) si_data, (intptr_t) si_data_len, (int32_t) si_flags, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_sock_send_to(int32_t arg0, int64_t arg1, int64_t arg2, int32_t arg3, int64_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("sock_send_to")
));

__wasi_errno_t __wasi_sock_send_to(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    const __wasi_addr_port_t * addr,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_snapshot_preview1_sock_send_to((int32_t) fd, (intptr_t) si_data, (intptr_t) si_data_len, (int32_t) si_flags, (int64_t) addr, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_snapshot_preview1_resolve(int64_t arg0, int64_t arg1, int32_t arg2, int64_t arg3, int32_t arg4, int64_t arg5) __attribute__((
    __import_module__("wasix_snapshot_preview1"),
    __import_name__("resolve")
));

__wasi_errno_t __wasi_resolve(
    const char *host,
    uint16_t port,
    __wasi_addr_ip_t * ips,
    __wasi_size_t nips,
    __wasi_size_t *retptr0
){
    size_t host_len = strlen(host);
    int32_t ret = __imported_wasix_snapshot_preview1_resolve((intptr_t) host, (intptr_t) host_len, (int32_t) port, (int64_t) ips, (int32_t) nips, (intptr_t) retptr0);
    return (uint16_t) ret;
}

