#include "test.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
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

int main(int argc, char **argv) {
  TEST(argc == 2);

  DIR *dir = opendir(argv[1]);
  TEST(dir != NULL);

  int count = 0;
  bool saw_dot = false;
  bool saw_dot_dot = false;
  for (;; count += 1) {
    struct dirent *ent = readdir(dir);
    if (ent == NULL) {
      if (errno == 0)
        break;
      TEST(ent != NULL);
    }

    if (strcmp(ent->d_name, ".") == 0) {
      TEST(ent->d_type == DT_DIR);
      saw_dot = true;
    } else if (strcmp(ent->d_name, "..") == 0) {
      TEST(ent->d_type == DT_DIR);
      saw_dot_dot = true;
    }
  }
  TEST(count == 2);
  TEST(saw_dot);
  TEST(saw_dot_dot);

  TEST(closedir(dir) == 0);

  return t_status;
}
