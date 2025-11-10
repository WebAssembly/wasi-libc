#include <dirent.h>
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

static int countdir(DIR *dir)
{
  int count = 0;
  while (readdir(dir) != NULL) {
    int is_error = errno != 0;
    TEST(!is_error);
    count++;
  }
  return count;
}

int main(void)
{
	int fd;
        int flags = 0;

        // Create the directory
        TEST(mkdir("test", 0755)==0);
        TEST((fd = open("test/a", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
        TEST(write(fd, 0, 1)==1);
        close(fd);
        TEST((fd = open("test/b", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
        TEST(write(fd, 0, 1)==1);
        close(fd);
        TEST((fd = open("test/c", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
        TEST(write(fd, 0, 1)==1);
        close(fd);

        DIR* dir = opendir("test");
        TEST(dir != NULL);
        TEST(countdir(dir) == 5);
        TEST(countdir(dir) == 0);
        rewinddir(dir);
        TEST(countdir(dir) == 5);
        TEST(countdir(dir) == 0);

	return t_status;
}

