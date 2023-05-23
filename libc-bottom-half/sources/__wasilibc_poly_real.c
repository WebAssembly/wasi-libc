#include <wasi/api.h>
#include <string.h>

#ifdef _REENTRANT
size_t __imported_wasi_thread_spawn(size_t arg0) __attribute__((
    __import_module__("wasi"),
    __import_name__("thread-spawn")
));

size_t __wasi_thread_spawn(void* start_arg) {
    return __imported_wasi_thread_spawn((size_t) start_arg);
}
#endif