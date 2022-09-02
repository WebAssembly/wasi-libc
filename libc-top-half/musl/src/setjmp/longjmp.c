#ifdef __wasilibc_unmodified_upstream
#else

#include <setjmp.h>
#include <wasi/libc.h>

_Noreturn void longjmp (jmp_buf buf, int val) {
    __wasilibc_longjmp(buf, val);
}

#endif