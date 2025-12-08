#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <wchar.h>
#include "test.h"

#define TEST(r, f, x, m) ( \
	errno=0, ((r) = (f)) == (x) || \
	(t_error("%s failed (" m ")\n", #f, r, x, strerror(errno)), 0) )

#define TEST_S(s, x, m) ( \
	!strcmp((s),(x)) || \
	(t_error("[%s] != [%s] (%s)\n", s, x, m), 0) )

static FILE *make_temp_file() {
    const char* path = "temp_file";
    FILE *f = fopen(path, "w+");
    if (f == NULL) {
      printf("Error: fopen(%s) failed: %s\n", path, strerror(errno));
      exit(1);
    }
    return f;
}

static FILE *writetemp(const char *data)
{
	FILE *f = make_temp_file();
	size_t n = strlen(data);
	if (!f) return 0;
	if (write(fileno(f), data, n) != n) {
		t_error("write: %s\n", strerror(errno));
		fclose(f);
		return 0;
	}
        if (lseek(fileno(f), 0, (SEEK_SET)) != 0) {
            t_error("lseek: %s\n", strerror(errno));
            fclose(f);
            return 0;
        }
	return f;
}

int main(void)
{
	FILE *f;
        int i;

	TEST(i, !!(f=writetemp("ab")), 1, "failed to make temp file");
	if (f) {
            char c;
            c = getc(f);
            TEST(i, c, 'a', "%c != %c");
            c = getc(f);
            TEST(i, c, 'b', "%c != %c");
            c = getc(f);
            TEST(i, c, EOF, "%c != %c");
            TEST(i, !!feof(f), 1, "%d != %d");
            fclose(f);
	}

	return t_status;
}
