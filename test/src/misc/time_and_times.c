//! add-flags.py(CFLAGS): -D_WASI_EMULATED_PROCESS_CLOCKS
//! add-flags.py(LDFLAGS): -lwasi-emulated-process-clocks
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

int main(void)
{
	time_t t;
        struct tms buffer;

        clock_t result = times(&buffer);
        TEST(result > 0);
        TEST(buffer.tms_utime > 0);
        TEST(buffer.tms_cutime == 0); // always 0 under WASI

        time_t t1 = time(&t);
        TEST(t==t1);
        TEST(t > 0);

	return t_status;
}
