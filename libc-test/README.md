This is an adaption of the [libc-test] testsuite for use in testing
WASI libc.

This code is based on libc-test revision
281b80f0e70fe28356c3f0ded8b86f28a5c74d0a from
git://repo.or.cz/libc-test.

Changes to upstream code are wrapped in preprocessor directives controlled
by the macro `__wasilibc_unmodified_upstream`.

There is a simple test.sh file for running the tests and comparing
the results with expected outcomes.

[libc-test]: https://wiki.musl-libc.org/libc-test.html
