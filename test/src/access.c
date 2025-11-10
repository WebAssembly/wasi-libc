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
	int fd, dirfd;
        int flags = 0;
        flags |= O_WRONLY | O_CREAT | O_EXCL;

	TEST((fd = open(tmp, flags, 0200)) > 2);
        TEST(write(fd, "hello", 6)==6);
        close(fd);

        TEST(access(tmp, F_OK)==0);
        TEST(access(tmp, W_OK)==0);
        // Even though the file was created as write-only,
        // WASI returns 0 because the parent directory
        // has read/write permissions
        TEST(access(tmp, R_OK)==0);

	TEST((dirfd = open(".", O_RDONLY, 0600)) > 2);

        TEST(faccessat(dirfd, tmp, F_OK, 0)==0);
        TEST(faccessat(dirfd, tmp, W_OK, 0)==-1);
        TEST(faccessat(dirfd, tmp, R_OK, 0)==0);

        close(dirfd);

        // Test with a closed file descriptor
        TEST(faccessat(dirfd, tmp, F_OK, 0)==-1);

        // Test with a nonexistent file
        TEST(access("bogus", F_OK)==-1);

        TEST(unlink(tmp) != -1);

	return t_status;
}
