# Single-threaded pthreads stubs

The goal of these files is to provide stub implementations of pthreads
functions for `THREAD_MODEL=single`. This implementation should _always_
follow the strict letter of the POSIX specifications. This means that
"doing nothing", "always succeeding", etc. are not proper implementations
-- these stubs aim for higher conformance than that.

The code that is "more" aligned with the spirit of the POSIX specifications
 ends up compiled into libc itself. This primarily consists of synchronization
  primitives (which are implemented to actually track state).

The code that is "less" aligned with the spirit of the specifications
(e.g. by "rules-lawyering" and always failing) are built into a library
`wasi-emulated-pthread.a`. The distinction is ultimately made by vibes and a
judgement call, not formal criteria.
