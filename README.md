# WASI(X) Libc

This fork of wasi-libc extends libc with the missing functionality needed to
build complete and useful applications, in particular it incorporates these
additional extensions:

- full support for efficient multithreading including joins, signals
  and `getpid`
- `pthreads` support (now extended from the WASI threads spec)
- full support for sockets (`socket`, `bind`, `connect`, `resolve`)
    - IPv4, IPv6
    - UDP, TCP
    - Multicast, Anycast
    - RAW sockets
- current directory support (`chdir`) integrated with the runtime
- `setjmp` / `longjmp` support (used extensively in `libc` ) via `asyncify`
- process forking (`fork` and `vfork` )
- subprocess spawning and waiting (`exec` , `wait` )
- TTY support
- asynchronous polling of sockets and files
- pipe and event support (`pipe`, `event` )
- DNS resolution support (`resolve` )

# WASI(X) Extensions Spec

[WASIX](https://wasix.org) is maintained by wasix.org [here](https://github.com/wasix-org/wasix-witx)

WASI(X) intent is to extend the WASI proposal and complete the ABI sufficiently
now to build useful and productive applications today - it is not intended as a
fork but rather to be a superset on top of WASI. Therefore it maintains full
forwards and backwards compatibility with this major version of WASI and stabilizes
it for the long term.

# WASI(X) Contributions

All contributions are welcome on extending WASI(X) with other extension(s). Just
submit your pull request here and we will review via normal GitHub processes.

# Long-term Support

WASIX will receive long term support by this community with a guarantee of
backwards compatibility on the ABI. Runtime(s) that support this ABI are assured
of its stability just as standard libraries and libraries can also count on that
same stability to join the dots and make the connections.

Major bug fixes and/or zero day vulnerabilities will be addressed promptly here
with careful consideration for resolving issues without compromising the
long-term support goal.

# WASI Libc

WASI Libc is a libc for WebAssembly programs built on top of WASI system calls.
It provides a wide array of POSIX-compatible C APIs, including support for
standard I/O, file I/O, filesystem manipulation, memory management, time, string,
environment variables, program startup, and many other APIs.

WASI Libc is sufficiently stable and usable for many purposes, as most of the
POSIX-compatible APIs are stable, though it is continuing to evolve to better
align with wasm and WASI. For example, pthread support is still a work in
progress.

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
