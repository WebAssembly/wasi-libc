// Modified from libc-test to not use tmpfile or /dev/null

#include "test.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c " failed: " __VA_ARGS__), 0))

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
  struct stat st;
  FILE *f;
  time_t t;

  if (TEST(stat(".", &st) == 0, "errno = %s\n", strerror(errno))) {
    TEST(S_ISDIR(st.st_mode), "\n");
    TEST(st.st_nlink > 0, "%ju\n", (uintmax_t)st.st_nlink);
    t = time(0);
    TEST(st.st_ctime <= t, "%jd > %jd\n", (intmax_t)st.st_ctime, (intmax_t)t);
    TEST(st.st_mtime <= t, "%jd > %jd\n", (intmax_t)st.st_mtime, (intmax_t)t);
    TEST(st.st_atime <= t, "%jd > %jd\n", (intmax_t)st.st_atime, (intmax_t)t);
  }

  if ((f = make_temp_file())) {
    size_t fputs_result = fputs("hello", f);
    TEST(fputs_result >= 0, "fputs_result=%jd\n", fputs_result);
    int flush_result = fflush(f);
    TEST(flush_result == 0, "%jd vs 0, errno=%d errnp = %s\n", flush_result,
         errno, strerror(errno));
    if (TEST(fstat(fileno(f), &st) == 0, "errnp = %s\n", strerror(errno))) {
      TEST(st.st_size == 5, "%jd vs 5\n", (intmax_t)st.st_size);
    }
    fclose(f);
  }

  return t_status;
}
