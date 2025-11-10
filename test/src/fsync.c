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

	TEST((fd = open(tmp, flags | O_WRONLY | O_CREAT | O_EXCL, 0600)) > 2);
	TEST(write(fd, "hello", 6)==6);

        // Test for success
        TEST(fsync(fd)==0);

        // Test with a nonexistent file descriptor
        int badfd = fd + 1;
        TEST(fsync(badfd)==-1);

        close(fd);
        TEST(unlink(tmp) != -1);

	return t_status;
}
