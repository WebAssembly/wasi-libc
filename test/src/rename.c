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
	char tmp[] = "test";
        char tmp1[] = "test1";
        char nonexistent[] = "bogus";
        struct stat st;
        int fd;
        int flags = 0;

        // Create a directory
        TEST(mkdir(tmp, 0755)==0);
        // Rename
        TEST(rename(tmp, tmp1)==0);
        TEST(stat(tmp, &st)!=0);
        TEST(stat(tmp1, &st)==0);
        TEST(rmdir(tmp1)==0);

        // Create a file
        TEST((fd = open(tmp, flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
        TEST(write(fd, 0, 1)==1);
        close(fd);
        // Rename
        TEST(rename(tmp, tmp1)==0);
        TEST(stat(tmp, &st)!=0);
        TEST(stat(tmp1, &st)==0);
        TEST(unlink(tmp1)==0);

        // Nonexistent source should fail
        TEST(rename(nonexistent, tmp1)==-1);

	return t_status;
}
