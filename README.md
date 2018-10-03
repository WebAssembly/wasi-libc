# WebAssembly Reference Sysroot

Caution: This is currently quite experimental and not generally usable yet!

## What is this?

This is a "reference sysroot", which is meant to be part of a common C ABI
that can be shared across C libraries and compilers. While it's intended to
(eventually) be usable in its own right, we fully expect other
implementations to be used in practice by many different systems, though
we do hope that in those cases, this library defines a useful ABI that can
be followed.

## Usage

Obtain a WebAssembly-supporting C compiler, and then run:

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
