#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "test.h"

#define TEST(c) do { \
	errno = 0; \
	if (!(c)) \
		t_error("%s failed (errno = %d)\n", #c, errno); \
} while(0)

int main(void)
{
    // closing a valid fd returns 0
    int fd;
    TEST((fd = open("/a", O_RDWR | O_CREAT, 0755)) > 2);
    TEST(close(fd) == 0);

    // closing an invalid fd return -1
    TEST(close(fd) == -1 && errno == EBADF);

    // can close stdio
    TEST(close(0) == 0);

    return t_status;
}
