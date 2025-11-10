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

    char *filename1, *filename2, *filename3, *filename4, *filename5, *filename6;
    TEST(asprintf(&filename1, "%s/input.txt", argv[1]) != -1);

    int fd = open(filename1, O_WRONLY | O_CREAT);
    TEST(fd > 2);
    close(fd);

    TEST(asprintf(&filename2, "%s/dir1", argv[1]) != -1);
    TEST(mkdir(filename2, 0755) == 0);
    TEST(asprintf(&filename3, "%s/dir1/dir2", argv[1]) != -1);
    TEST(mkdir(filename3, 0755) == 0);
    TEST(asprintf(&filename4, "%s/dir1/dir2/dir3", argv[1]) != -1);
    TEST(mkdir(filename4, 0755) == 0);

    TEST(asprintf(&filename5, "%s/input.txt", filename4) != -1);

    fd = open(filename5, O_WRONLY | O_CREAT);
    TEST(fd > 2);
    close(fd);

    // invalid utf-8 shouldn't exist
    TEST(asprintf(&filename6, "%s/\xff\xff", filename4) != -1);
    fd = open(filename6, O_WRONLY | O_CREAT);
    TEST(fd == -1);

    TEST(unlink(filename5)==0);
    TEST(rmdir(filename4)==0);
    TEST(rmdir(filename3)==0);
    TEST(rmdir(filename2)==0);
    TEST(unlink(filename1)==0);

    return t_status;
}
