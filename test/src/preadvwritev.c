#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
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

        // NOTE: wasip1 pwritev() only writes the first non-empty iov
        struct iovec iov;
        iov.iov_base = malloc(5);
        iov.iov_len = 5;
        memcpy(iov.iov_base, "hello", 5);

	TEST((fd = open(tmp, flags, 0600)) > 2);
        TEST(pwritev(fd, &iov, 1, 1)==5);

        free(iov.iov_base);

        iov.iov_base = malloc(5);
        iov.iov_len = 5;

        TEST(preadv(fd, &iov, 1, 1)==5);

        // Negative offset is an error
        TEST(pwritev(fd, &iov, 1, -1)==-1);
        TEST(preadv(fd, &iov, 1, -1)==-1);

        // 0-length vector should work
        free(iov.iov_base);
        iov.iov_len = 0;
        iov.iov_base = NULL;
        TEST(pwritev(fd, &iov, 1, 0)==0);
        TEST(preadv(fd, &iov, 1, 0)==0);

        close(fd);

        TEST(unlink(tmp) != -1);

	return t_status;
}
