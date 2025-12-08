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
	char foo[6];
	int fd;
        int flags = 0;
        flags |= O_RDWR | O_CREAT | O_EXCL;

	TEST((fd = open(tmp, flags, 0200)) > 2);
        TEST(write(fd, "hello", 6)==6);
        TEST(pwrite(fd, "42", 2, 2)==2);
        TEST(pread(fd, &foo, 3, 2)==3);
        TEST(strcmp(foo, "42o")==0);

        // Negative offset is an error
        TEST(pwrite(fd, "q", 1, -1)==-1);
        TEST(pread(fd, &foo, 2, -1)==-1);

        close(fd);

        TEST(unlink(tmp) != -1);

	return t_status;
}
