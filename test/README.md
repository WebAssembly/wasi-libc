# Test

This directory runs C tests compiled to WebAssembly against `wasi-libc` to check
its functionality. It enables a subset of [libc-test] as well as adding custom C
tests; all enabled tests are contained in the [`src`] directory.

### Pre-requisites

- Clang
- [`libc-test`]
- `libclang_rt.builtins-wasm32.a`
- a WebAssembly engine
- other WebAssembly tools, especially for `wasm32-wasip2` support (see the
  [`Makefile] for a complete list)

All but Clang are downloaded automatically by the `make download` target.

### Build and run

To build and run all tests:

```sh
$ make TARGET_TRIPLE=...
Tests passed
```

Note that `wasm-ld` must be available, so an additional
`CC=<wasi-sdk>/bin/clang` may be necessary. Each test runs in a directory that
looks like (see [`run-test.sh`]):

```sh
$ ls run/$TARGET_TRIPLE/misc/some-test
cmd.sh      # the exact command used to run the test
fs          # a directory containing any test-created files
output.log  # the captured printed output--only for errors
```

### Running tests in the browser

To run a test in the browser, use the `scripts/browser-test/harness.mjs` as `ENGINE`. The harness uses [`bjorn3/browser_wasi_shim`](https://github.com/bjorn3/browser_wasi_shim/) as a WASI implementation on browser.

```sh
$ npm -C scripts/browser-test install
$ npx -C scripts/browser-test playwright install chromium-headless-shell
$ make ENGINE="$PWD/scripts/browser-test/harness.mjs" TARGET_TRIPLE=...
```

### Adding tests

To add a test, create a new C file in [`src/misc`]:

```c
//! filter.py(TARGET_TRIPLE): !wasm32-wasip2
//! add-flags.py(CFLAGS): ...
//! add-flags.py(LDFLAGS): ...
//! add-flags.py(RUN): ...
void main() { ... }
```

- to pass, the `main` function must exit successfully and avoid printing output
- the `filter.py` directive controls when the test builds and runs (e.g., not
  for `wasip2`)
- the `add-flags.py` directive adds extra information for building or running
  the test (see the [`Makefile`] for precise use).

### Enabling more [libc-test] tests

[libc-test] has more tests available that are not yet enabled (e.g., to count
the enabled subset, `find src -name *.c | wc -l`). Each enabled test contains a
stub file in [`src/libc-test`] that `#include`s its downloaded version and adds
various `filter.py` and `add-flags.py` directives.

To quickly create stub files for not-yet-enabled tests:

```sh
$ make generate-stubs
$ git status
...
src/libc-test/functional/tls_align.c
src/libc-test/functional/tls_align_dlopen.c
src/libc-test/functional/tls_align_dso.c
src/libc-test/functional/tls_init.c
```

Then modify the directives for these new stub files to get the new tests to
compile and successfully run.



[libc-test]: https://wiki.musl-libc.org/libc-test.html
[`Makefile`]: Makefile
[`run-test.sh`]: scripts/run-test.sh
[`src`]: src
[`src/libc-test`]: src/libc-test
[`src/misc`]: src/misc
