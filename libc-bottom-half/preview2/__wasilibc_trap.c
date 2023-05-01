/*
 * This is similar to __wasilibc_real.c, except the function bodies are all
 * replaced with `__builtin_trap()`. When everything is migrated to preview2
 * APIs, we can remove this file.
 */

#include <wasi/api.h>
#include <string.h>

__wasi_errno_t __wasi_args_get(
    uint8_t * * argv,
    uint8_t * argv_buf
){
    __builtin_trap();
}

__wasi_errno_t __wasi_args_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    __builtin_trap();
}

__wasi_errno_t __wasi_environ_get(
    uint8_t * * environ,
    uint8_t * environ_buf
){
    __builtin_trap();
}

__wasi_errno_t __wasi_environ_sizes_get(
    __wasi_size_t *retptr0,
    __wasi_size_t *retptr1
){
    __builtin_trap();
}

__wasi_errno_t __wasi_clock_res_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_clock_time_get(
    __wasi_clockid_t id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_advise(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len,
    __wasi_advice_t advice
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_allocate(
    __wasi_fd_t fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t len
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_close(
    __wasi_fd_t fd
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_datasync(
    __wasi_fd_t fd
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_fdstat_get(
    __wasi_fd_t fd,
    __wasi_fdstat_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_fdstat_set_flags(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_fdstat_set_rights(
    __wasi_fd_t fd,
    __wasi_rights_t fs_rights_base,
    __wasi_rights_t fs_rights_inheriting
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_filestat_get(
    __wasi_fd_t fd,
    __wasi_filestat_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_filestat_set_size(
    __wasi_fd_t fd,
    __wasi_filesize_t size
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_pread(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_prestat_get(
    __wasi_fd_t fd,
    __wasi_prestat_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_prestat_dir_name(
    __wasi_fd_t fd,
    uint8_t * path,
    __wasi_size_t path_len
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_pwrite(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    __wasi_filesize_t offset,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_read(
    __wasi_fd_t fd,
    const __wasi_iovec_t *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_readdir(
    __wasi_fd_t fd,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_dircookie_t cookie,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_renumber(
    __wasi_fd_t fd,
    __wasi_fd_t to
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_seek(
    __wasi_fd_t fd,
    __wasi_filedelta_t offset,
    __wasi_whence_t whence,
    __wasi_filesize_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_sync(
    __wasi_fd_t fd
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_tell(
    __wasi_fd_t fd,
    __wasi_filesize_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_fd_write(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *iovs,
    size_t iovs_len,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_path_create_directory(
    __wasi_fd_t fd,
    const char *path
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_filestat_get(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    __wasi_filestat_t *retptr0
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_filestat_set_times(
    __wasi_fd_t fd,
    __wasi_lookupflags_t flags,
    const char *path,
    __wasi_timestamp_t atim,
    __wasi_timestamp_t mtim,
    __wasi_fstflags_t fst_flags
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_link(
    __wasi_fd_t old_fd,
    __wasi_lookupflags_t old_flags,
    const char *old_path,
    __wasi_fd_t new_fd,
    const char *new_path
){
   __builtin_trap();
}

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
   __builtin_trap();
}

__wasi_errno_t __wasi_path_readlink(
    __wasi_fd_t fd,
    const char *path,
    uint8_t * buf,
    __wasi_size_t buf_len,
    __wasi_size_t *retptr0
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_remove_directory(
    __wasi_fd_t fd,
    const char *path
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_rename(
    __wasi_fd_t fd,
    const char *old_path,
    __wasi_fd_t new_fd,
    const char *new_path
){
    __builtin_trap();
}

__wasi_errno_t __wasi_path_symlink(
    const char *old_path,
    __wasi_fd_t fd,
    const char *new_path
){
   __builtin_trap();
}

__wasi_errno_t __wasi_path_unlink_file(
    __wasi_fd_t fd,
    const char *path
){
   __builtin_trap();
}

__wasi_errno_t __wasi_poll_oneoff(
    const __wasi_subscription_t * in,
    __wasi_event_t * out,
    __wasi_size_t nsubscriptions,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

_Noreturn void __wasi_proc_exit(
    __wasi_exitcode_t rval
){
    __builtin_trap();
}

__wasi_errno_t __wasi_sched_yield(
    void
){
    __builtin_trap();
}

__wasi_errno_t __wasi_random_get(
    uint8_t * buf,
    __wasi_size_t buf_len
){
    __builtin_trap();
}

__wasi_errno_t __wasi_sock_accept(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_sock_recv(
    __wasi_fd_t fd,
    const __wasi_iovec_t *ri_data,
    size_t ri_data_len,
    __wasi_riflags_t ri_flags,
    __wasi_size_t *retptr0,
    __wasi_roflags_t *retptr1
){
    __builtin_trap();
}

__wasi_errno_t __wasi_sock_send(
    __wasi_fd_t fd,
    const __wasi_ciovec_t *si_data,
    size_t si_data_len,
    __wasi_siflags_t si_flags,
    __wasi_size_t *retptr0
){
    __builtin_trap();
}

__wasi_errno_t __wasi_sock_shutdown(
    __wasi_fd_t fd,
    __wasi_sdflags_t how
){
    __builtin_trap();
}

#ifdef _REENTRANT
int32_t __wasi_thread_spawn(void* start_arg) {
    return __imported_wasi_thread_spawn((int32_t) start_arg);
}
#endif
