//! add-flags.py(RUN): --dir fs::/

// Modified from libc-test to not use tmpfile or /dev/null

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>
#include "test.h"

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

int main(void)
{
        char tmp[] = "testsuite-XXXXXX";
        int fd;
        int flags = 0;

        // Not a very useful test, but it's hard to test poll() without threads

        TEST((fd = open(tmp, flags | O_RDWR | O_CREAT | O_EXCL, 0600)) > 2);
        TEST(write(fd, "hello", 6)==6);

        struct pollfd poll_fd = { .fd =  fd, .events = POLLRDNORM, .revents = 0 };
        TEST(poll(&poll_fd, 1, 1)==1);

        poll_fd.events = POLLWRNORM;
        TEST(poll(&poll_fd, 1, 1)==1);

        close(fd);

        if (fd > 2)
            TEST(unlink(tmp) != -1);

	return t_status;
}
