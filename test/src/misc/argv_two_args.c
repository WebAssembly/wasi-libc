//! add-flags.py(CFLAGS): -I.
//! add-flags.py(ARGS): foo bar
#include <limits.h>
#include <stdio.h>
#include "test.h"

#define TEST(c, ...) \
	( (c) || (t_error(#c " failed: " __VA_ARGS__),0) )

int main(int argc, char **argv)
{
        char buf[PATH_MAX];
        TEST(argc == 3, "argc should be 3\n");
        TEST(argv[0] != 0, "argv[0] should not be NULL\n");
        TEST(argv[1] != 0, "argv[1] should not be NULL\n");
        TEST(argv[2] != 0, "argv[2] should not be NULL\n");
        TEST(argv[3] == NULL, "argv[3] should be NULL\n");

        for (int i = 0; i < argc; i++) {
            TEST(argv[i][0] != 0, "argument should not be empty\n");
            TEST(snprintf(buf, sizeof buf, "%s", argv[i]) < sizeof buf, "argument is not a valid path\n");
        }
        return t_status;
}
