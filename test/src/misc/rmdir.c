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
	char tmp[] = "test";
        char nonexistent[] = "bogus";

        // Create a directory
        TEST(mkdir(tmp, 0755)==0);
        // Remove with rmdir()
        TEST(rmdir(tmp) == 0);

        // Re-create
        TEST(mkdir(tmp, 0755)==0);
        // Remove with unlink() -- this fails for a directory
        TEST(unlink(tmp)==-1);

        // Remove with remove()
        TEST(remove(tmp) == 0);

        // Test that it fails with a non-existent directory
        TEST(rmdir(nonexistent)==-1);
        TEST(unlink(nonexistent)==-1);
        TEST(remove(nonexistent)==-1);

	return t_status;
}
