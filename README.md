# `wasi-libc`

`wasi-libc` is a libc for WebAssembly programs built on top of WASI system
calls. It provides a wide array of POSIX-compatible C APIs, including support
for standard I/O, file I/O, filesystem manipulation, memory management, time,
string, environment variables, program startup, and many other APIs.

`wasi-libc` is sufficiently stable and usable for many purposes, as most of the
POSIX-compatible APIs are stable, though it is continuing to evolve to better
align with wasm and WASI. For example, pthread support is experimentally
provided via the [wasi-threads] proposal.

[wasi-threads]: https://github.com/WebAssembly/wasi-threads

## Usage

The easiest way to get started with this is to use [wasi-sdk], which includes a
build of `wasi-libc` in its sysroot.

## Building from source

To build a WASI sysroot from source, obtain a WebAssembly-supporting C compiler
(currently this is only clang 10+, though we'd like to support other compilers
as well), and then run:

```sh
make CC=/path/to/clang/with/wasm/support \
     AR=/path/to/llvm-ar \
     NM=/path/to/llvm-nm
```

This makes a directory called "sysroot" by default. See the top of the Makefile
for customization options.

To use the sysroot, use the `--sysroot=` option:

```sh
/path/to/wasm/supporting/c/compiler --sysroot=/path/to/the/newly/built/sysroot ...
```

to run the compiler using the newly built sysroot.

## Other default libraries

Note that Clang packages typically don't include cross-compiled builds of
compiler-rt, libcxx, or libcxxabi, for `libclang_rt.builtins-wasm32.a`,
`libc++.a`, or `libc++abi.a`, respectively. This means that by default just
passing `--sysroot` may not be enough as Clang will also look for some of these
libraries, notably `libclang_rt.builtins-wasm32.a`, by default. This is one of
the things [wasi-sdk] simplifies, as it includes cross-compiled builds of
compiler-rt, `libc++.a`, and `libc++abi.a`.

The build of wasi-libc itself may depend on compiler-rt, or
`libclang_rt.builtins-wasm32.a`. This manifests in error messages such as
`undefined symbol: __muloti4` for example. Specifically the PIC build of
wasi-libc will require that Clang can locate `libclang_rt.builtins-wasm32.a`,
meaning that `libc.so` requires this library to be present. To handle this the
default behavior of the build system is to download the latest version of
compiler-rt from [wasi-sdk] and use that for the build of `libc.so`. You can
also set `BUILTINS_LIB` as a path to `libclang_rt.builtins-wasm32.a` as well to
use that instead of downloading a version.

## Building in pthread support

To enable pthreads support via the [wasi-threads] proposal, follow the above
build directions with one addition: `make ... THREAD_MODEL=posix`. This creates
additional artifacts in `sysroot/lib/wasm32-wasi-threads` to support `--target
wasm32-wasi-threads`.

## 64 bits support

To enable 64 bits support, follow the above
build directions with one addition: `make ... WASM64=yes`. This creates
artifacts in `sysroot/lib/wasm64-wasi` to support `--target
wasm64-wasi`.

You can build 64 bits with pthread support together with
'make ... WASM64=yes THREAD_MODEL=posix'. This creates
artifacts in `sysroot/lib/wasm64-wasi-threads` to support `--target
wasm64-wasi-threads`.

## Arch Linux AUR package

For Arch Linux users, there's an official [wasi-libc] package tracking this Git
repository. You might want to install other [WASI related packages] as well.

[wasi-sdk]: https://github.com/WebAssembly/wasi-sdk
[wasi-libc]: https://archlinux.org/packages/extra/any/wasi-libc/
[WASI related packages]: https://archlinux.org/packages/?q=wasi-
