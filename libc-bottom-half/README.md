"WASI" the WebAssembly System Interface.

WASI libc is conceptually the lower half of a traditional libc implementation.
It provides C interfaces to the low-level WASI syscalls.

This is largely based on [CloudABI], [cloudlibc], and [libpreopen], however we
use just the low-level syscall wrappers rather than all of cloudlibc and
libpreopen, and we have several customizations for use in a WebAssembly sysroot.

[CloudABI]: https://github.com/NuxiNL/cloudabi
[cloudlibc]: https://github.com/NuxiNL/cloudlibc
[libpreopen]: https://github.com/musec/libpreopen

The upstream repositories and versions used here are:

cloudlibc - https://github.com/NuxiNL/cloudlibc 8835639f27fc42d32096d59d294a0bbb857dc368
libpreopen - https://github.com/musec/libpreopen b29e9287cc75a7db7291ce3eb468a3d2bad8ceb1

Whole files which are unused are omitted. Changes to upstream code are wrapped
in preprocessor directives controlled by the macro `__wasilibc_unmodified_upstream`,
except that CloudABI names have also been renamed to WASI names without annotations.

WASI libc currently depends on the basics and dlmalloc components of reference-sysroot.
