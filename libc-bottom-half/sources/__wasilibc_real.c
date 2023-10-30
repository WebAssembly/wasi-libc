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

uint32_t __imported_wasi_snapshot_preview1_args_get(uintptr_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("args_get")
));

__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_args_get((uintptr_t) argv, (uintptr_t) argv_buf);
}

uint32_t __imported_wasi_snapshot_preview1_args_sizes_get(uintptr_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("args_sizes_get")
));

__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_args_sizes_get((uintptr_t) retptr0, (uintptr_t) retptr1);
}

uint32_t __imported_wasi_snapshot_preview1_environ_get(uintptr_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("environ_get")
));

__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_environ_get((uintptr_t) environ, (uintptr_t) environ_buf);
}

uint32_t __imported_wasi_snapshot_preview1_environ_sizes_get(uintptr_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("environ_sizes_get")
));

__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_environ_sizes_get((uintptr_t) retptr0, (uintptr_t) retptr1);
}

uint32_t __imported_wasi_snapshot_preview1_clock_res_get(uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("clock_res_get")
));

__wasi_errno_t __wasi_clock_res_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_clock_res_get((uint32_t) id, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_clock_time_get(uint32_t, uint64_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("clock_time_get")
));

__wasi_errno_t __wasi_clock_time_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_clock_time_get((uint32_t) id, (uint64_t) precision, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_advise(uint32_t, uint64_t, uint64_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_advise")
));

__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_advise((uint32_t) fd, (uint64_t) offset, (uint64_t) len, (uint32_t) advice);
}

uint32_t __imported_wasi_snapshot_preview1_fd_allocate(uint32_t, uint64_t, uint64_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_allocate")
));

__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_allocate((uint32_t) fd, (uint64_t) offset, (uint64_t) len);
}

uint32_t __imported_wasi_snapshot_preview1_fd_close(uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_close")
));

__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_close((uint32_t) fd);
}

uint32_t __imported_wasi_snapshot_preview1_fd_datasync(uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_datasync")
));

__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_datasync((uint32_t) fd);
}

uint32_t __imported_wasi_snapshot_preview1_fd_fdstat_get(uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_fdstat_get")
));

__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_get((uint32_t) fd, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_fdstat_set_flags(uint32_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_fdstat_set_flags")
));

__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_set_flags((uint32_t) fd, (uint32_t) flags);
}

uint32_t __imported_wasi_snapshot_preview1_fd_fdstat_set_rights(uint32_t, uint64_t, uint64_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_fdstat_set_rights")
));

__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_fdstat_set_rights((uint32_t) fd, (uint64_t) fs_rights_base, (uint64_t) fs_rights_inheriting);
}

uint32_t __imported_wasi_snapshot_preview1_fd_filestat_get(uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_filestat_get")
));

__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_get((uint32_t) fd, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_filestat_set_size(uint32_t, uint64_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_filestat_set_size")
));

__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    __wasi_filesize_t size
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_set_size((uint32_t) fd, (uint64_t) size);
}

uint32_t __imported_wasi_snapshot_preview1_fd_filestat_set_times(uint32_t, uint64_t, uint64_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_filestat_set_times")
));

__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_filestat_set_times((uint32_t) fd, (uint64_t) atim, (uint64_t) mtim, (uint32_t) fst_flags);
}

uint32_t __imported_wasi_snapshot_preview1_fd_pread(uint32_t, uintptr_t, __wasi_size_t, uint64_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_pread")
));

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    __wasi_iovec_t const *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_pread((uint32_t) fd, (uintptr_t) iovs, (__wasi_size_t) iovs_len, (uint64_t) offset, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_prestat_get(uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_prestat_get")
));

__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_prestat_get((uint32_t) fd, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_prestat_dir_name(uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_prestat_dir_name")
));

__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    uint8_t * path,
    __wasi_size_t path_len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_prestat_dir_name((uint32_t) fd, (uintptr_t) path, (__wasi_size_t) path_len);
}

uint32_t __imported_wasi_snapshot_preview1_fd_pwrite(uint32_t, uintptr_t, __wasi_size_t, uint64_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_pwrite")
));

__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_pwrite((uint32_t) fd, (uintptr_t) iovs, (__wasi_size_t) iovs_len, (uint64_t) offset, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_read(uint32_t, uintptr_t, __wasi_size_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_read")
));

__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    __wasi_iovec_t const *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_read((uint32_t) fd, (uintptr_t) iovs, (__wasi_size_t) iovs_len, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_readdir(uint32_t, uintptr_t, __wasi_size_t, uint64_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_readdir")
));

__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_readdir((uint32_t) fd, (uintptr_t) buf, (__wasi_size_t) buf_len, (uint64_t) cookie, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_renumber(uint32_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_renumber")
));

__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    __wasi_fd_t to
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_renumber((uint32_t) fd, (uint32_t) to);
}

uint32_t __imported_wasi_snapshot_preview1_fd_seek(uint32_t, uint64_t, uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_seek")
));

__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_seek((uint32_t) fd, (uint64_t) offset, (uint32_t) whence, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_sync(uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_sync")
));

__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_sync((uint32_t) fd);
}

uint32_t __imported_wasi_snapshot_preview1_fd_tell(uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_tell")
));

__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_tell((uint32_t) fd, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_fd_write(uint32_t, uintptr_t, __wasi_size_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("fd_write")
));

__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_fd_write((uint32_t) fd, (uintptr_t) iovs, (__wasi_size_t) iovs_len, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_path_create_directory(uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_create_directory")
));

__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_create_directory((uint32_t) fd, (uintptr_t) path, (__wasi_size_t) path_len);
}

uint32_t __imported_wasi_snapshot_preview1_path_filestat_get(uint32_t, uint32_t, uintptr_t, __wasi_size_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_filestat_get")
));

__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    char const *path,
    __wasi_filestat_t *retptr0
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_filestat_get((uint32_t) fd, (uint32_t) flags, (uintptr_t) path, (__wasi_size_t) path_len, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_path_filestat_set_times(uint32_t, uint32_t, uintptr_t, __wasi_size_t, uint64_t, uint64_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_filestat_set_times")
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
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_filestat_set_times((uint32_t) fd, (uint32_t) flags, (uintptr_t) path, (__wasi_size_t) path_len, (uint64_t) atim, (uint64_t) mtim, (uint32_t) fst_flags);
}

uint32_t __imported_wasi_snapshot_preview1_path_link(uint32_t, uint32_t, uintptr_t, __wasi_size_t, uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_link")
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
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_link((uint32_t) old_fd, (uint32_t) old_flags, (uintptr_t) old_path, (__wasi_size_t) old_path_len, (uint32_t) new_fd, (uintptr_t) new_path, (__wasi_size_t) new_path_len);
}

uint32_t __imported_wasi_snapshot_preview1_path_open(uint32_t, uint32_t, uintptr_t, __wasi_size_t, uint32_t, uint64_t, uint64_t, uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_open")
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
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_open((uint32_t) fd, (uint32_t) dirflags, (uintptr_t) path, (__wasi_size_t) path_len, (uint32_t) oflags, (uint64_t) fs_rights_base, (uint64_t) fs_rights_inheriting, (uint32_t) fdflags, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_path_readlink(uint32_t, uintptr_t, __wasi_size_t, uintptr_t, __wasi_size_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_readlink")
));

__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    char const *path,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_size_t *retptr0
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_readlink((uint32_t) fd, (uintptr_t) path, (__wasi_size_t) path_len, (uintptr_t) buf, (__wasi_size_t) buf_len, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_path_remove_directory(uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_remove_directory")
));

__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_remove_directory((uint32_t) fd, (uintptr_t) path, (__wasi_size_t) path_len);
}

uint32_t __imported_wasi_snapshot_preview1_path_rename(uint32_t, uintptr_t, __wasi_size_t, uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_rename")
));

__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    char const *old_path,
    __wasi_fd_t new_fd,
    char const *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_rename((uint32_t) fd, (uintptr_t) old_path, (__wasi_size_t) old_path_len, (uint32_t) new_fd, (uintptr_t) new_path, (__wasi_size_t) new_path_len);
}

uint32_t __imported_wasi_snapshot_preview1_path_symlink(uintptr_t, __wasi_size_t, uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_symlink")
));

__wasi_errno_t __wasi_path_symlink(
    char const *old_path,
    __wasi_fd_t fd,
    char const *new_path
){
    size_t old_path_len = strlen(old_path);
    size_t new_path_len = strlen(new_path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_symlink((uintptr_t) old_path, (__wasi_size_t) old_path_len, (uint32_t) fd, (uintptr_t) new_path, (__wasi_size_t) new_path_len);
}

uint32_t __imported_wasi_snapshot_preview1_path_unlink_file(uint32_t, uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("path_unlink_file")
));

__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    char const *path
){
    size_t path_len = strlen(path);
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_path_unlink_file((uint32_t) fd, (uintptr_t) path, (__wasi_size_t) path_len);
}

uint32_t __imported_wasi_snapshot_preview1_poll_oneoff(uintptr_t, uintptr_t, uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("poll_oneoff")
));

__wasi_errno_t __wasi_poll_oneoff(
    __wasi_subscription_t const * in,
    __wasi_event_t * out,
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_poll_oneoff((uintptr_t) in, (uintptr_t) out, (uint32_t) nsubscriptions, (uintptr_t) retptr0);
}

_Noreturn void __imported_wasi_snapshot_preview1_proc_exit(uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("proc_exit")
));

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
){
    __imported_wasi_snapshot_preview1_proc_exit((uint32_t) rval);
}

uint32_t __imported_wasi_snapshot_preview1_sched_yield() __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("sched_yield")
));

__wasi_errno_t __wasi_sched_yield(
    void
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sched_yield();
}

uint32_t __imported_wasi_snapshot_preview1_random_get(uintptr_t, __wasi_size_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("random_get")
));

__wasi_errno_t __wasi_random_get(
    uint8_t * buf,
    __wasi_size_t buf_len
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_random_get((uintptr_t) buf, (__wasi_size_t) buf_len);
}

uint32_t __imported_wasi_snapshot_preview1_sock_accept(uint32_t, uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("sock_accept")
));

__wasi_errno_t __wasi_sock_accept(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_accept((uint32_t) fd, (uint32_t) flags, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_sock_recv(uint32_t, uintptr_t, __wasi_size_t, uint32_t, uintptr_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("sock_recv")
));

__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    __wasi_iovec_t const *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_recv((uint32_t) fd, (uintptr_t) ri_data, (__wasi_size_t) ri_data_len, (uint32_t) ri_flags, (uintptr_t) retptr0, (uintptr_t) retptr1);
}

uint32_t __imported_wasi_snapshot_preview1_sock_send(uint32_t, uintptr_t, __wasi_size_t, uint32_t, uintptr_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("sock_send")
));

__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    __wasi_ciovec_t const *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_send((uint32_t) fd, (uintptr_t) si_data, (__wasi_size_t) si_data_len, (uint32_t) si_flags, (uintptr_t) retptr0);
}

uint32_t __imported_wasi_snapshot_preview1_sock_shutdown(uint32_t, uint32_t) __attribute__((
    __import_module__("wasi_snapshot_preview1"),
    __import_name__("sock_shutdown")
));

__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    __wasi_sdflags_t how
){
    return (__wasi_errno_t) __imported_wasi_snapshot_preview1_sock_shutdown((uint32_t) fd, (uint32_t) how);
}

#ifdef _REENTRANT
uint32_t __imported_wasi_snapshot_preview2_thread_spawn(uintptr_t arg0) __attribute__((
    __import_module__("wasi_snapshot_preview2"),
    __import_name__("thread_spawn")
));

int32_t __wasi_thread_spawn(void* start_arg) {
    return (int32_t) __imported_wasi_snapshot_preview2_thread_spawn((uintptr_t) start_arg);
}
#endif
