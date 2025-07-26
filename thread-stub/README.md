# Stub pthreads implementation

This library provides the foundation for a POSIX conformant implementation of
pthreads API for `THREAD_MODEL=single`. When combined with the appropriate
parts of musl's libc-top-half, it forms a complete pthreads implementation
that is never able to spawn new threads, but otherwise follows the letter of
the specification in every way.
