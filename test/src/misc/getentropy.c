//! add-flags.py(CFLAGS): -I.
//! add-flags.py(ARGS): foo bar
#include <limits.h>
#include <stdio.h>
#include "test.h"

#define TEST(c, ...) \
	( (c) || (t_error(#c " failed: " __VA_ARGS__),0) )

int main()
{
        size_t len = 256;
        uint8_t buffer[len];

        TEST(getentropy(&buffer, len) == 0, "getentropy() should return 0\n");
        int all_zeroes = 1;

        for (size_t i = 0; i < len; i++) {
            if (buffer[i] != 0) {
                all_zeroes = 0;
                break;
            }
        }

        TEST(all_zeroes == 0, "getentropy() returned 256 zeroes\n");

        // More than 256 bytes is an error
        TEST(getentropy(&buffer, 257)==-1, "requesting > 256 bytes should be an error\n");

        return t_status;
}
