//! add-flags.py(RUN): --dir fs::foo
//! add-flags.py(ARGS): foo

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

int main(int argc, char **argv)
{
    TEST(argc == 2);

    char *filename;
    TEST(asprintf(&filename, "%s/input.txt", argv[1]) != -1);

    int fd = open(filename, O_WRONLY | O_CREAT);
    TEST(fd > 2);
    close(fd);

    TEST(asprintf(&filename, "%s/dir1", argv[1]) != -1);
    TEST(mkdir(filename, 0755) != 0);
    TEST(asprintf(&filename, "%s/dir1/dir2", argv[1]) != -1);
    TEST(mkdir(filename, 0755) != 0);
    TEST(asprintf(&filename, "%s/dir1/dir2/dir3", argv[1]) != -1);
    TEST(mkdir(filename, 0755) != 0);

    TEST(asprintf(&filename, "%s/input.txt", filename) != -1);

    fd = open(filename, O_WRONLY | O_CREAT);
    TEST(fd > 2);
    close(fd);

    return t_status;
}
