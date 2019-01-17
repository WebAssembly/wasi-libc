# WebAssembly Reference Sysroot

Caution: This is currently quite experimental and not generally usable yet!

## What is this?

It's several things.

First, it's a usable libc. It builds a "sysroot" which can be pointed to by
compilers, such as Clang 8.0, using the wasm32-unknown-wasi-musl target triple.
It's experimental, but it is already sufficient to run basic programs.

Second, it's a "reference" implementation, which means the interfaces defined
here can be used by other tools and libraries, even if they don't use the
actual implementations here. For example, we don't expect everyone will want
to use the exact `malloc` implementation provided here, but tools and
libraries using an ABI-compatible `malloc` interface will be able to
interoperate regardless of which actual implementation is used.

Third, it's an example showing the use of the WASI API. The libc functionality
is implemented using calls to WASI functions.

## Usage

Obtain a WebAssembly-supporting C compiler (currently this is only clang,
though we'd like to support other compilers as well), and then run:

```
make WASM_CC=/path/to/wasm/supporting/c/compiler
```

This makes a directory called "sysroot", by default. See the top of the Makefile
for customization options.

To use the sysroot, use the `--sysroot=` option:

```
/path/to/wasm/supporting/c/compiler --sysroot=/path/to/the/newly/built/sysroot ...
```

to run the compiler using the newly built sysroot.
