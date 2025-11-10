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

        int positions[5];
        int count = 0;

        DIR* dir = opendir("test");
        TEST(dir != NULL);
        struct dirent *entry;

        int is_end;
        while(1) {
          positions[count] = telldir(dir);
          if (readdir(dir) == NULL)
            break;
          count++;
        }
        TEST(count == 5);

        for (int i = 0; i < count; i++) {
          seekdir(dir, positions[i]);
          TEST(telldir(dir) == positions[i]);
          int c = countdir(dir);
          TEST(c == (5 - i));
        }

	return t_status;
}


