//! filter.py(TARGET_TRIPLE): wasm32-wasip1-threads
//! add-flags.py(CFLAGS): -I.
//! add-flags.py(LDFLAGS): -Wl,--import-memory,--export-memory,--shared-memory,--max-memory=1073741824
//! add-flags.py(RUN): --wasi threads
#include "build/download/libc-test/src/functional/pthread_tsd.c"

#include <wasi/libc-busywait.h>

__attribute__((constructor))
void __wasilibc_enable_busywait(void)
{
    __wasilibc_enable_futex_busywait_on_current_thread();
}
