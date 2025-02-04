#include <stdint.h>
#include <time.h>
#include <errno.h>

#define DEFINE_GLOBAL_GETTER(name, core_type, c_type) \
    static inline c_type name##_get(void) { \
        c_type val; \
        __asm__( \
            ".globaltype " #name ", " #core_type "\n" \
            "global.get " #name "\n" \
            "local.set %0\n" \
            : "=r"(val)); \
        return val; \
    }
#define DEFINE_GLOBAL_SETTER(name, core_type, c_type) \
    static inline void name##_set(c_type val) { \
        __asm__( \
            ".globaltype " #name ", " #core_type "\n" \
            "local.get %0\n" \
            "global.set " #name "\n" \
            : : "r"(val)); \
    }

#define DEFINE_RW_GLOBAL(name, core_type, c_type) \
    __asm__( \
        ".globaltype " #name ", " #core_type "\n" \
        ".global " #name "\n" \
        #name ":\n" \
    ); \
    DEFINE_GLOBAL_GETTER(name, core_type, c_type) \
    DEFINE_GLOBAL_SETTER(name, core_type, c_type)

DEFINE_RW_GLOBAL(__wasilibc_use_busy_futex, i32, int32_t)

void __wasilibc_enable_futex_busywait_on_current_thread(void)
{
    __wasilibc_use_busy_futex_set(1);
}

int __wasilibc_futex_wait_atomic_wait(volatile void *addr, int op, int val, int64_t max_wait_ns);

int __wasilibc_futex_wait_maybe_busy(volatile void *addr, int op, int val, int64_t max_wait_ns)
{
    // PLEASE NOTE THAT WE CANNOT CALL LIBC FUNCTIONS THAT USE FUTEXES HERE

    if (!__wasilibc_use_busy_futex_get()) {
        return __wasilibc_futex_wait_atomic_wait(addr, op, val, max_wait_ns);
    }

    struct timespec start;
    int r = clock_gettime(CLOCK_REALTIME, &start);

    // If we can't get the current time, we can't wait with a timeout.
    if (r) return r;

    while (1) {
        // Check timeout if it's a positive value
        if (max_wait_ns >= 0) {
            struct timespec now;
            r = clock_gettime(CLOCK_REALTIME, &now);
            if (r) return r;

            int64_t elapsed_ns = (now.tv_sec - start.tv_sec) * 1000000000 + now.tv_nsec - start.tv_nsec;
            if (elapsed_ns >= max_wait_ns) {
                return -ETIMEDOUT;
            }
        }

        if (__c11_atomic_load((_Atomic int *)addr, __ATOMIC_SEQ_CST) != val) {
            break;
        }
    }

    return 0;
} 
