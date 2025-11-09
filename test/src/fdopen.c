// Modified from libc to not use mkstemp and to use a relative path

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
	char tmp[] = "testsuite-XXXXXX";
	char foo[6];
	int fd;
        int flags = 0;
	FILE *f;

	TEST((fd = open(tmp, flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
	TEST(write(fd, "hello", 6)==6);
	TEST(f = fdopen(fd, "rb"));
	if (f) {
                TEST(ftello(f) == 6);
                TEST(fseeko(f, 0, SEEK_SET)==0);
		TEST(fgets(foo, sizeof foo, f));
		if (strcmp(foo,"hello") != 0)
			t_error("fgets read back: \"%s\"; wanted: \"hello\"\n", foo);
		fclose(f);
	}
	if (fd > 2)
		TEST(unlink(tmp) != -1);
	return t_status;
}
