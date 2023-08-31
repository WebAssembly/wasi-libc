#ifdef __wasilibc_unmodified_upstream
#else

#include <setjmp.h>
#include <wasi/libc.h>

int setjmp (jmp_buf buf) {
    return __wasilibc_setjmp(buf);
}

// ignoring signal masking for now
int sigsetjmp(jmp_buf  buf, int savesigs) {
    return __wasilibc_setjmp(buf);
}

#endif