// Modified from libc-test in order to not use tmpfile

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))
#define TESTVAL(v,op,x) TEST(v op x, "%jd\n", (intmax_t)(v))

static FILE *make_temp_file() {
    const char* path = "temp_file";
    FILE *f = fopen(path, "w");
    if (f == NULL) {
      printf("Error: fopen(%s) failed: %s\n", path, strerror(errno));
      exit(1);
    }
    return f;
}

static int doit_futimens(int fd, const struct timespec times[2]) {
    return futimens(fd, times);
}

static int doit_utimensat(int fd, const struct timespec times[2]) {
    return utimensat(AT_FDCWD, "temp_file", times, 0);
}

void run(int (*set_times)(int, const struct timespec[2]))
{
	struct stat st;
	FILE *f;
	int fd;
	time_t t;

	if (!TEST(f = make_temp_file())) return;
	fd = fileno(f);

	TEST(set_times(fd, (struct timespec[2]){0}) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_atim.tv_nsec,==,0);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);

	TEST(set_times(fd, ((struct timespec[2]){{.tv_sec=1,.tv_nsec=UTIME_OMIT},{.tv_sec=1,.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_atim.tv_nsec,==,0);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);

	t = time(0);

	TEST(set_times(fd, ((struct timespec[2]){{.tv_nsec=UTIME_NOW},{.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,>=,t);
	TESTVAL(st.st_mtim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_nsec,==,0);

	TEST(set_times(fd, (struct timespec[2]){0}) == 0, "\n");
	TEST(set_times(fd, ((struct timespec[2]){{.tv_nsec=UTIME_OMIT},{.tv_nsec=UTIME_NOW}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,==,0);
	TESTVAL(st.st_mtim.tv_sec,>=,t);

	TEST(set_times(fd, ((struct timespec[2]){{.tv_nsec=UTIME_NOW},{.tv_nsec=UTIME_OMIT}})) == 0, "\n");
	TEST(fstat(fd, &st) == 0, "\n");
	TESTVAL(st.st_atim.tv_sec,>=,t);
	TESTVAL(st.st_mtim.tv_sec,>=,t);

	if (TEST((time_t)(1LL<<32) == (1LL<<32), "implementation has Y2038 EOL\n")) {
		if (TEST(set_times(fd, ((struct timespec[2]){{.tv_sec=1LL<<32},{.tv_sec=1LL<<32}})) == 0, "%s\n", strerror(errno))) {
			TEST(fstat(fd, &st) == 0, "\n");
			TESTVAL(st.st_atim.tv_sec, ==, 1LL<<32);
			TESTVAL(st.st_mtim.tv_sec, ==, 1LL<<32);
		}
	}

	fclose(f);
}

int main(void)
{
	TEST(futimens(-1, ((struct timespec[2]){{.tv_nsec=UTIME_OMIT},{.tv_nsec=UTIME_OMIT}}))==0 || errno==EBADF,
		"%s\n", strerror(errno));
        fprintf(stderr, "testing futimens...\n");
        run(doit_futimens);
        fprintf(stderr, "testing utimensat...\n");
        run(doit_utimensat);

	return t_status;
}
