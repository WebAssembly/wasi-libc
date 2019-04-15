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
libpreopen - https://github.com/musec/libpreopen 8265fc50b9db3730c250597bdd084f1e728f3e48

Whole files which are unused are omitted. Changes to upstream code are wrapped
in preprocessor directives controlled by the macro `__wasilibc_unmodified_upstream`,
except that CloudABI names have also been renamed to WASI names without annotations.

WASI libc currently depends on the basics and dlmalloc components of reference-sysroot.
