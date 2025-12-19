// Modified from libc to not use mkstemp and to use a relative path

#include "test.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

int filter(const struct dirent *dirent) {
  const char *name = dirent->d_name;
  int is_b = strcmp(name, "b") == 0;
  int is_dot = name[0] == '.';
  return !(is_b || is_dot);
}

int compare(const struct dirent **dirent1, const struct dirent **dirent2) {
  return strcmp((*dirent1)->d_name, (*dirent2)->d_name);
}

int main(void) {
  char tmp[] = "testsuite-XXXXXX";
  int fd;
  int flags = 0;

  // Create the directory
  TEST(mkdir("test", 0755) == 0);
  TEST((fd = open("test/a", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, 0, 1) == 1);
  close(fd);
  TEST((fd = open("test/b", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, 0, 1) == 1);
  close(fd);
  TEST((fd = open("test/c", flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
  TEST(write(fd, 0, 1) == 1);
  close(fd);

  struct dirent **namelist;
  TEST(scandir("test", &namelist, &filter, &compare) == 2);

  TEST(unlink("test/a") != -1);
  TEST(unlink("test/b") != -1);
  TEST(unlink("test/c") != -1);
  TEST(rmdir("test") == 0);

  return t_status;
}
