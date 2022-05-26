# WASI Libc

WASI Libc is a libc for WebAssembly programs built on top of WASI system calls.
It provides a wide array of POSIX-compatible C APIs, including support for
standard I/O, file I/O, filesystem manipulation, memory management, time, string,
environment variables, program startup, multithreading, networkign, and many other
APIs.

## Usage

The easiest way to get started with this is to use [wasi-sdk], which includes a
build of WASI Libc in its sysroot.

## Building from source

To build a WASI sysroot from source, obtain a WebAssembly-supporting C compiler
(currently this is only clang 8+, though we'd like to support other compilers as well),
and then run:

```sh
make CC=/path/to/clang/with/wasm/support \
     AR=/path/to/llvm-ar \
     NM=/path/to/llvm-nm
```

This makes a directory called "sysroot", by default. See the top of the Makefile
for customization options.

To use the sysroot, use the `--sysroot=` option:

```sh
/path/to/wasm/supporting/c/compiler --sysroot=/path/to/the/newly/built/sysroot ...
```

to run the compiler using the newly built sysroot.

Note that Clang packages typically don't include cross-compiled builds of
compiler-rt, libcxx, or libcxxabi, for `libclang_rt.builtins-wasm32.a`, libc++.a,
or libc++abi.a, respectively, so they may not be usable without
extra setup. This is one of the things [wasi-sdk] simplifies, as it includes
cross-compiled builds of compiler-rt, libc++.a, and libc++abi.a.
