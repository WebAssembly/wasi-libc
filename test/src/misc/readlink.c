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
        char tmp1[] = "testsuite-link";
	int fd;
        int flags = 0;

	TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
	TEST(write(fd, "hello", 6)==6);
        close(fd);

        TEST(symlink(tmp, tmp1)==0);

        TEST(readlink(tmp1, tmp, 16)==16);

        TEST(unlink(tmp1) != -1);
        TEST(unlink(tmp) != -1);

	return t_status;
}
