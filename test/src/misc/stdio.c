//! add-flags.py(RUN): --dir fs::/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

int main(void)
{
        // Test that closing stdin/stdout/stderr is an error

        TEST(close(0) == -1);
        TEST(close(1) == -1);
        TEST(close(2) == -1);

	return t_status;
}
