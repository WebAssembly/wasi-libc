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
        flags |= O_WRONLY | O_CREAT | O_EXCL;

	TEST((fd = open(tmp, flags, 0600)) > 2);

        int returned_flags = fcntl(fd, F_GETFL);
        TEST(returned_flags==O_WRONLY);

        // This can only change some GNU-specific flags,
        // so for now, just test that setting the same flags
        // doesn't change anything
        TEST(fcntl(fd, F_SETFL, returned_flags)==0);
        returned_flags = fcntl(fd, F_GETFL);
        TEST(returned_flags==O_WRONLY);

        // Test with a nonexistent file descriptor
        int badfd = fd + 1;
        TEST(fcntl(badfd, F_GETFL)==-1);
        TEST(fcntl(badfd, F_SETFL, returned_flags)==-1);

        close(fd);

        // Test with a closed file descriptor
        TEST(fcntl(fd, F_GETFL)==-1);

        TEST(unlink(tmp) != -1);

	return t_status;
}
