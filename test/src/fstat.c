#include "test.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define TEST(c)                                                                \
  do {                                                                         \
    errno = 0;                                                                 \
    if (!(c))                                                                  \
      t_error("%s failed (errno = %d)\n", #c, errno);                          \
  } while (0)

static FILE *make_temp_file() {
  const char *path = "temp_file";
  FILE *f = fopen(path, "w");
  if (f == NULL) {
    printf("Error: fopen(%s) failed: %s\n", path, strerror(errno));
    exit(1);
  }
  return f;
}

int main(void) {
  struct stat stat;

  for (int i = 0; i < 3; i++) {
    printf("testing fd %d\n", i);
    TEST(fstat(i, &stat) == 0);
    printf("st_mode is 0o%o\n", stat.st_mode);
  }

  FILE *f = make_temp_file();
  TEST(f);
  int fd = fileno(f);

  TEST(fstat(fd, &stat) == 0);
  TEST((stat.st_mode & S_IFMT) == S_IFREG);
  fclose(f);

  return t_status;
}
