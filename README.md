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

## Why doesn't this contain a full libc implementation?

In the short term, one of the main goals is just to provide a reference
point for people already maintaining their own libc codebases, to help
reduce interface incompatibilities between the several different
environments out there.

In the long term, there may some day be some form of standardized
syscall/import layer for wasm which would could support a full
"reference libc", at which point this repository might make sense as
a place to host such a thing.

In between, if there are specific pieces of libc functionality which
people would find useful to have here, and which don't depend on any
syscalls, we could add them, using code from existing third-party
codebases as appropriate.
