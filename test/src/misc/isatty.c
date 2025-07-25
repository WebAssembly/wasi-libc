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
	char tmp[] = "testsuite-XXXXXX";
	int fd;
        int flags = 0;
        flags |= O_RDWR | O_CREAT | O_EXCL;

	TEST((fd = open(tmp, flags, 0200)) > 2);
        TEST(!isatty(fd));
        close(fd);

        TEST(unlink(tmp) != -1);

	return t_status;
}
