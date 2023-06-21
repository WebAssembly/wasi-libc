/**
 * THIS FILE IS AUTO-GENERATED from the following files:
 *   wasix_v1.witx
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

int32_t __imported_wasix_32v1_clock_time_set(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("clock_time_set")
));

__wasi_errno_t __wasi_clock_time_set(
    __wasi_clockid_t id,
    __wasi_timestamp_t timestamp
){
    int32_t ret = __imported_wasix_32v1_clock_time_set((int32_t) id, (int64_t) timestamp);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_fd_dup(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("fd_dup")
));

__wasi_errno_t __wasi_fd_dup(
    __wasi_fd_t fd,
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_fd_dup((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_fd_event(int64_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("fd_event")
));

__wasi_errno_t __wasi_fd_event(
    uint64_t initial_val,
    __wasi_eventfdflags_t flags,
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_fd_event((int64_t) initial_val, flags, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_fd_pipe(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("fd_pipe")
));

__wasi_errno_t __wasi_fd_pipe(
    __wasi_fd_t *retptr0,
    __wasi_fd_t *retptr1
){
    int32_t ret = __imported_wasix_32v1_fd_pipe((intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_tty_get(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("tty_get")
));

__wasi_errno_t __wasi_tty_get(
    __wasi_tty_t * state
){
    int32_t ret = __imported_wasix_32v1_tty_get((int32_t) state);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_tty_set(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("tty_set")
));

__wasi_errno_t __wasi_tty_set(
    __wasi_tty_t * state
){
    int32_t ret = __imported_wasix_32v1_tty_set((int32_t) state);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_getcwd(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("getcwd")
));

__wasi_errno_t __wasi_getcwd(
    uint8_t * path,
    __wasi_pointersize_t * path_len
){
    int32_t ret = __imported_wasix_32v1_getcwd((int32_t) path, (int32_t) path_len);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_chdir(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("chdir")
));

__wasi_errno_t __wasi_chdir(
    const char *path
){
    size_t path_len = strlen(path);
    int32_t ret = __imported_wasix_32v1_chdir((intptr_t) path, (intptr_t) path_len);
    return (uint16_t) ret;
}

void __imported_wasix_32v1_callback_signal(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("callback_signal")
));

void __wasi_callback_signal(
    const char *callback
){
    size_t callback_len = strlen(callback);
    __imported_wasix_32v1_callback_signal((intptr_t) callback, (intptr_t) callback_len);
}

int32_t __imported_wasix_32v1_thread_spawn_v2(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_spawn_v2")
));

__wasi_errno_t __wasi_thread_spawn_v2(
    __wasi_thread_start_t * args,
    __wasi_tid_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_thread_spawn_v2((int32_t) args, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_thread_sleep(int64_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_sleep")
));

__wasi_errno_t __wasi_thread_sleep(
    __wasi_timestamp_t duration
){
    int32_t ret = __imported_wasix_32v1_thread_sleep((int64_t) duration);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_thread_id(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_id")
));

__wasi_errno_t __wasi_thread_id(
    __wasi_tid_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_thread_id((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_thread_join(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_join")
));

__wasi_errno_t __wasi_thread_join(
    __wasi_tid_t tid
){
    int32_t ret = __imported_wasix_32v1_thread_join((int32_t) tid);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_thread_parallelism(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_parallelism")
));

__wasi_errno_t __wasi_thread_parallelism(
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_thread_parallelism((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_thread_signal(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_signal")
));

__wasi_errno_t __wasi_thread_signal(
    __wasi_tid_t tid,
    __wasi_signal_t signal
){
    int32_t ret = __imported_wasix_32v1_thread_signal((int32_t) tid, (int32_t) signal);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_futex_wait(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("futex_wait")
));

__wasi_errno_t __wasi_futex_wait(
    uint32_t * futex,
    uint32_t expected,
    const __wasi_option_timestamp_t * timeout,
    __wasi_bool_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_futex_wait((int32_t) futex, (int32_t) expected, (int32_t) timeout, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_futex_wake(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("futex_wake")
));

__wasi_errno_t __wasi_futex_wake(
    uint32_t * futex,
    __wasi_bool_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_futex_wake((int32_t) futex, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_futex_wake_all(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("futex_wake_all")
));

__wasi_errno_t __wasi_futex_wake_all(
    uint32_t * futex,
    __wasi_bool_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_futex_wake_all((int32_t) futex, (intptr_t) retptr0);
    return (uint16_t) ret;
}

_Noreturn void __imported_wasix_32v1_thread_exit(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("thread_exit")
));

_Noreturn void __wasi_thread_exit(
    __wasi_exitcode_t rval
){
    __imported_wasix_32v1_thread_exit((int32_t) rval);
}

int32_t __imported_wasix_32v1_stack_checkpoint(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("stack_checkpoint")
));

__wasi_errno_t __wasi_stack_checkpoint(
    __wasi_stack_snapshot_t * snapshot,
    __wasi_longsize_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_stack_checkpoint((int32_t) snapshot, (intptr_t) retptr0);
    return (uint16_t) ret;
}

_Noreturn void __imported_wasix_32v1_stack_restore(int32_t arg0, int64_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("stack_restore")
));

_Noreturn void __wasi_stack_restore(
    const __wasi_stack_snapshot_t * snapshot,
    __wasi_longsize_t val
){
    __imported_wasix_32v1_stack_restore((int32_t) snapshot, (int64_t) val);
}

int32_t __imported_wasix_32v1_proc_raise_interval(int32_t arg0, int64_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_raise_interval")
));

__wasi_errno_t __wasi_proc_raise_interval(
    __wasi_signal_t sig,
    __wasi_timestamp_t interval,
    __wasi_bool_t repeat
){
    int32_t ret = __imported_wasix_32v1_proc_raise_interval((int32_t) sig, (int64_t) interval, (int32_t) repeat);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_proc_fork(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_fork")
));

__wasi_errno_t __wasi_proc_fork(
    __wasi_bool_t copy_memory,
    __wasi_pid_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_proc_fork((int32_t) copy_memory, (intptr_t) retptr0);
    return (uint16_t) ret;
}

_Noreturn void __imported_wasix_32v1_proc_exec(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_exec")
));

_Noreturn void __wasi_proc_exec(
    const char *name,
    const char *args
){
    size_t name_len = strlen(name);
    size_t args_len = strlen(args);
    __imported_wasix_32v1_proc_exec((intptr_t) name, (intptr_t) name_len, (intptr_t) args, (intptr_t) args_len);
}

int32_t __imported_wasix_32v1_proc_spawn(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7, int32_t arg8, int32_t arg9, int32_t arg10, int32_t arg11, int32_t arg12) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_spawn")
));

__wasi_errno_t __wasi_proc_spawn(
    const char *name,
    __wasi_bool_t chroot,
    const char *args,
    const char *preopen,
    __wasi_stdio_mode_t stdin,
    __wasi_stdio_mode_t stdout,
    __wasi_stdio_mode_t stderr,
    const char *working_dir,
    __wasi_process_handles_t *retptr0
){
    size_t name_len = strlen(name);
    size_t args_len = strlen(args);
    size_t preopen_len = strlen(preopen);
    size_t working_dir_len = strlen(working_dir);
    int32_t ret = __imported_wasix_32v1_proc_spawn((intptr_t) name, (intptr_t) name_len, (int32_t) chroot, (intptr_t) args, (intptr_t) args_len, (intptr_t) preopen, (intptr_t) preopen_len, (int32_t) stdin, (int32_t) stdout, (int32_t) stderr, (intptr_t) working_dir, (intptr_t) working_dir_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_proc_id(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_id")
));

__wasi_errno_t __wasi_proc_id(
    __wasi_pid_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_proc_id((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_proc_parent(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_parent")
));

__wasi_errno_t __wasi_proc_parent(
    __wasi_pid_t pid,
    __wasi_pid_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_proc_parent((int32_t) pid, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_proc_join(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_join")
));

__wasi_errno_t __wasi_proc_join(
    __wasi_option_pid_t * pid,
    __wasi_join_flags_t flags,
    __wasi_join_status_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_proc_join((int32_t) pid, flags, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_proc_signal(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("proc_signal")
));

__wasi_errno_t __wasi_proc_signal(
    __wasi_pid_t pid,
    __wasi_signal_t signal
){
    int32_t ret = __imported_wasix_32v1_proc_signal((int32_t) pid, (int32_t) signal);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_ws_connect(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("ws_connect")
));

__wasi_errno_t __wasi_ws_connect(
    const char *url,
    __wasi_fd_t *retptr0
){
    size_t url_len = strlen(url);
    int32_t ret = __imported_wasix_32v1_ws_connect((intptr_t) url, (intptr_t) url_len, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_http_request(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6, int32_t arg7) __attribute__((
    __import_module__("wasix_32v1"),
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
    int32_t ret = __imported_wasix_32v1_http_request((intptr_t) url, (intptr_t) url_len, (intptr_t) method, (intptr_t) method_len, (intptr_t) headers, (intptr_t) headers_len, (int32_t) gzip, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_http_status(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("http_status")
));

__wasi_errno_t __wasi_http_status(
    __wasi_fd_t fd,
    __wasi_http_status_t * status
){
    int32_t ret = __imported_wasix_32v1_http_status((int32_t) fd, (int32_t) status);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_bridge(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_bridge")
));

__wasi_errno_t __wasi_port_bridge(
    const char *network,
    const char *token,
    __wasi_stream_security_t security
){
    size_t network_len = strlen(network);
    size_t token_len = strlen(token);
    int32_t ret = __imported_wasix_32v1_port_bridge((intptr_t) network, (intptr_t) network_len, (intptr_t) token, (intptr_t) token_len, security);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_unbridge() __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_unbridge")
));

__wasi_errno_t __wasi_port_unbridge(
    void
){
    int32_t ret = __imported_wasix_32v1_port_unbridge();
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_dhcp_acquire() __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_dhcp_acquire")
));

__wasi_errno_t __wasi_port_dhcp_acquire(
    void
){
    int32_t ret = __imported_wasix_32v1_port_dhcp_acquire();
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_addr_add(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_addr_add")
));

__wasi_errno_t __wasi_port_addr_add(
    const __wasi_addr_cidr_t * addr
){
    int32_t ret = __imported_wasix_32v1_port_addr_add((int32_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_addr_remove(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_addr_remove")
));

__wasi_errno_t __wasi_port_addr_remove(
    const __wasi_addr_t * addr
){
    int32_t ret = __imported_wasix_32v1_port_addr_remove((int32_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_addr_clear() __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_addr_clear")
));

__wasi_errno_t __wasi_port_addr_clear(
    void
){
    int32_t ret = __imported_wasix_32v1_port_addr_clear();
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_mac(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_mac")
));

__wasi_errno_t __wasi_port_mac(
    __wasi_hardware_address_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_port_mac((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_addr_list(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_addr_list")
));

__wasi_errno_t __wasi_port_addr_list(
    __wasi_addr_cidr_t * addrs,
    __wasi_size_t * naddrs
){
    int32_t ret = __imported_wasix_32v1_port_addr_list((int32_t) addrs, (int32_t) naddrs);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_gateway_set(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_gateway_set")
));

__wasi_errno_t __wasi_port_gateway_set(
    const __wasi_addr_t * addr
){
    int32_t ret = __imported_wasix_32v1_port_gateway_set((int32_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_route_add(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_route_add")
));

__wasi_errno_t __wasi_port_route_add(
    const __wasi_addr_cidr_t * cidr,
    const __wasi_addr_t * via_router,
    const __wasi_option_timestamp_t * preferred_until,
    const __wasi_option_timestamp_t * expires_at
){
    int32_t ret = __imported_wasix_32v1_port_route_add((int32_t) cidr, (int32_t) via_router, (int32_t) preferred_until, (int32_t) expires_at);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_route_remove(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_route_remove")
));

__wasi_errno_t __wasi_port_route_remove(
    const __wasi_addr_t * cidr
){
    int32_t ret = __imported_wasix_32v1_port_route_remove((int32_t) cidr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_route_clear() __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_route_clear")
));

__wasi_errno_t __wasi_port_route_clear(
    void
){
    int32_t ret = __imported_wasix_32v1_port_route_clear();
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_port_route_list(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("port_route_list")
));

__wasi_errno_t __wasi_port_route_list(
    __wasi_route_t * routes,
    __wasi_size_t * nroutes
){
    int32_t ret = __imported_wasix_32v1_port_route_list((int32_t) routes, (int32_t) nroutes);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_status(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_status")
));

__wasi_errno_t __wasi_sock_status(
    __wasi_fd_t fd,
    __wasi_sock_status_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_status((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_addr_local(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_addr_local")
));

__wasi_errno_t __wasi_sock_addr_local(
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_addr_local((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_addr_peer(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_addr_peer")
));

__wasi_errno_t __wasi_sock_addr_peer(
    __wasi_fd_t fd,
    __wasi_addr_port_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_addr_peer((int32_t) fd, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_open(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_open")
));

__wasi_errno_t __wasi_sock_open(
    __wasi_address_family_t af,
    __wasi_sock_type_t socktype,
    __wasi_sock_proto_t sock_proto,
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_open((int32_t) af, (int32_t) socktype, (int32_t) sock_proto, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_set_opt_flag(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_set_opt_flag")
));

__wasi_errno_t __wasi_sock_set_opt_flag(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_bool_t flag
){
    int32_t ret = __imported_wasix_32v1_sock_set_opt_flag((int32_t) fd, (int32_t) sockopt, (int32_t) flag);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_get_opt_flag(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_get_opt_flag")
));

__wasi_errno_t __wasi_sock_get_opt_flag(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_bool_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_get_opt_flag((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_set_opt_time(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_set_opt_time")
));

__wasi_errno_t __wasi_sock_set_opt_time(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    const __wasi_option_timestamp_t * timeout
){
    int32_t ret = __imported_wasix_32v1_sock_set_opt_time((int32_t) fd, (int32_t) sockopt, (int32_t) timeout);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_get_opt_time(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_get_opt_time")
));

__wasi_errno_t __wasi_sock_get_opt_time(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_option_timestamp_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_get_opt_time((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_set_opt_size(int32_t arg0, int32_t arg1, int64_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_set_opt_size")
));

__wasi_errno_t __wasi_sock_set_opt_size(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_filesize_t size
){
    int32_t ret = __imported_wasix_32v1_sock_set_opt_size((int32_t) fd, (int32_t) sockopt, (int64_t) size);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_get_opt_size(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_get_opt_size")
));

__wasi_errno_t __wasi_sock_get_opt_size(
    __wasi_fd_t fd,
    __wasi_sock_option_t sockopt,
    __wasi_filesize_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_get_opt_size((int32_t) fd, (int32_t) sockopt, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_join_multicast_v4(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_join_multicast_v4")
));

__wasi_errno_t __wasi_sock_join_multicast_v4(
    __wasi_fd_t fd,
    const __wasi_addr_ip4_t * multiaddr,
    const __wasi_addr_ip4_t * interface
){
    int32_t ret = __imported_wasix_32v1_sock_join_multicast_v4((int32_t) fd, (int32_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_leave_multicast_v4(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_leave_multicast_v4")
));

__wasi_errno_t __wasi_sock_leave_multicast_v4(
    __wasi_fd_t fd,
    const __wasi_addr_ip4_t * multiaddr,
    const __wasi_addr_ip4_t * interface
){
    int32_t ret = __imported_wasix_32v1_sock_leave_multicast_v4((int32_t) fd, (int32_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_join_multicast_v6(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_join_multicast_v6")
));

__wasi_errno_t __wasi_sock_join_multicast_v6(
    __wasi_fd_t fd,
    const __wasi_addr_ip6_t * multiaddr,
    uint32_t interface
){
    int32_t ret = __imported_wasix_32v1_sock_join_multicast_v6((int32_t) fd, (int32_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_leave_multicast_v6(int32_t arg0, int32_t arg1, int32_t arg2) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_leave_multicast_v6")
));

__wasi_errno_t __wasi_sock_leave_multicast_v6(
    __wasi_fd_t fd,
    const __wasi_addr_ip6_t * multiaddr,
    uint32_t interface
){
    int32_t ret = __imported_wasix_32v1_sock_leave_multicast_v6((int32_t) fd, (int32_t) multiaddr, (int32_t) interface);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_bind(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_bind")
));

__wasi_errno_t __wasi_sock_bind(
    __wasi_fd_t fd,
    const __wasi_addr_port_t * addr
){
    int32_t ret = __imported_wasix_32v1_sock_bind((int32_t) fd, (int32_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_listen(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_listen")
));

__wasi_errno_t __wasi_sock_listen(
    __wasi_fd_t fd,
    __wasi_size_t backlog
){
    int32_t ret = __imported_wasix_32v1_sock_listen((int32_t) fd, (int32_t) backlog);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_accept_v2(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_accept_v2")
));

__wasi_errno_t __wasi_sock_accept_v2(
    __wasi_fd_t fd,
    __wasi_fdflags_t flags,
    __wasi_fd_t *retptr0,
    __wasi_addr_port_t *retptr1
){
    int32_t ret = __imported_wasix_32v1_sock_accept_v2((int32_t) fd, flags, (intptr_t) retptr0, (intptr_t) retptr1);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_connect(int32_t arg0, int32_t arg1) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_connect")
));

__wasi_errno_t __wasi_sock_connect(
    __wasi_fd_t fd,
    const __wasi_addr_port_t * addr
){
    int32_t ret = __imported_wasix_32v1_sock_connect((int32_t) fd, (int32_t) addr);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_recv_from(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5, int32_t arg6) __attribute__((
    __import_module__("wasix_32v1"),
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
    int32_t ret = __imported_wasix_32v1_sock_recv_from((int32_t) fd, (intptr_t) ri_data, (intptr_t) ri_data_len, ri_flags, (intptr_t) retptr0, (intptr_t) retptr1, (intptr_t) retptr2);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_send_to(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((
    __import_module__("wasix_32v1"),
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
    int32_t ret = __imported_wasix_32v1_sock_send_to((int32_t) fd, (intptr_t) si_data, (intptr_t) si_data_len, (int32_t) si_flags, (int32_t) addr, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_sock_send_file(int32_t arg0, int32_t arg1, int64_t arg2, int64_t arg3, int32_t arg4) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("sock_send_file")
));

__wasi_errno_t __wasi_sock_send_file(
    __wasi_fd_t out_fd,
    __wasi_fd_t in_fd,
    __wasi_filesize_t offset,
    __wasi_filesize_t count,
    __wasi_filesize_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_sock_send_file((int32_t) out_fd, (int32_t) in_fd, (int64_t) offset, (int64_t) count, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_resolve(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4, int32_t arg5) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("resolve")
));

__wasi_errno_t __wasi_resolve(
    const char *host,
    uint16_t port,
    __wasi_addr_t * addrs,
    __wasi_size_t naddrs,
    __wasi_size_t *retptr0
){
    size_t host_len = strlen(host);
    int32_t ret = __imported_wasix_32v1_resolve((intptr_t) host, (intptr_t) host_len, (int32_t) port, (int32_t) addrs, (int32_t) naddrs, (intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_epoll_create(int32_t arg0) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("epoll_create")
));

__wasi_errno_t __wasi_epoll_create(
    __wasi_fd_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_epoll_create((intptr_t) retptr0);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_epoll_ctl(int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("epoll_ctl")
));

__wasi_errno_t __wasi_epoll_ctl(
    __wasi_fd_t epfd,
    __wasi_epoll_ctl_t op,
    __wasi_fd_t fd,
    const __wasi_epoll_event_t * event
){
    int32_t ret = __imported_wasix_32v1_epoll_ctl((int32_t) epfd, (int32_t) op, (int32_t) fd, (int32_t) event);
    return (uint16_t) ret;
}

int32_t __imported_wasix_32v1_epoll_wait(int32_t arg0, int32_t arg1, int32_t arg2, int64_t arg3, int32_t arg4) __attribute__((
    __import_module__("wasix_32v1"),
    __import_name__("epoll_wait")
));

__wasi_errno_t __wasi_epoll_wait(
    __wasi_fd_t epfd,
    __wasi_epoll_event_t * event,
    __wasi_size_t maxevents,
    __wasi_timestamp_t timeout,
    __wasi_size_t *retptr0
){
    int32_t ret = __imported_wasix_32v1_epoll_wait((int32_t) epfd, (int32_t) event, (int32_t) maxevents, (int64_t) timeout, (intptr_t) retptr0);
    return (uint16_t) ret;
}

