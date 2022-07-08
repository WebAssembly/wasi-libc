#include <__macro_PAGESIZE.h>

/*
 * POSIX deprecated `getpagesize` [in the 1990's] apparently because the return
 * type is unfortunately an `int`, so it typically can't represent page sizes
 * of 2 GiB or more. That was before most OS's supported what we now know as
 * "huge"/"large"/"super" pages. Today, it doesn't seem like a practical
 * problem anymore. 2 GiB+ pages are a thing, but we now know they're not the
 * base page size of a process, so `getpagesize` is ok with its `int` return
 * type.
 *
 * [in the 1990's]: https://pubs.opengroup.org/onlinepubs/007908799/xsh/getpagesize.html
 */
int getpagesize(void)
{
    return PAGESIZE;
}
