# Test

This directory runs C tests compiled to WebAssembly against `wasi-libc` to check
its functionality. It enables a subset of [libc-test] as well as adding custom C
tests; all enabled tests are contained in the [`src`] directory.

### Pre-requisites

- Clang
- CMake

### Build and run

Before building/running tests `wasi-libc` must itself be built. For example
`make` must be run in the root of the repository.

Next tests must be configured with CMake. This can be done like so from the root
of the `wasi-libc` repository:

```sh
$ cmake -S test -B testbuild
```

This will by default build test for `wasm32-wasi` and must match the target
found in the `sysroot` folder of the repository too.

There are a few configuration options you might want to specify to the `cmake`
command line as well:

* `-G Ninja` - or some other build system, but this configures the default build
  system that CMake will generate.
* `-DCMAKE_C_COMPILER=clang` - specifies which compiler to use when building tests.
* `-DTARGET_TRIPLE=wasm32-wasip1` - specify an alternative target to be tested.
* `-DENGINE=engine` - override the default usage of Wasmtime to run test with
  the `engine` specified.

This `cmake` step will create a folder called `testbuild` which contains the
build system and will have the intermediate artifacts of the tests as well. If
`-G Ninja` was passed then the tests can then be built as:

```sh
$ ninja -C testbuild
```

This build step is required as the testing/`ctest` step below will not build
tests. By building tests ahead of time you're ensuring that the latest copy of
tests are available.

Finally tests can be run with:

```sh
$ ninja -C testbuild test
```

This will run all tests with CMake's `ctest` executable. You can also directly
run `ctest` for more control over the output. For example:

```sh
$ ctest -j4 --output-on-failure --test-dir testbuild
```

CMake's [documentation for
CTest](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html)
documents many other options/configurations as well.

### Running tests in the browser

To run a test in the browser, use the `scripts/browser-test/harness.mjs` as `ENGINE`. The harness uses [`bjorn3/browser_wasi_shim`](https://github.com/bjorn3/browser_wasi_shim/) as a WASI implementation on browser.

```sh
$ npm -C scripts/browser-test install
$ npx -C scripts/browser-test playwright install chromium-headless-shell
$ cmake -S test -B testbuild -DENGINE="$PWD/scripts/browser-test/harness.mjs" -DTARGET_TRIPLE=...
```

### Adding tests

To add a test, create a new C file in [`src`]:

```c
int main() { ... }
```

To pass, the `main` function must exit successfully.

Next edit [`CMakeLists.txt`] and use the `add_wasilibc_test` helper function toa
dd your test. The documentation of `register_test` shows some options which can
be passed to configuring how a test is built or what it can access at runtime.

### Enabling more [libc-test] tests

[libc-test] has more tests available that are not yet enabled. Each enabled test
is listed in `CMakeLists.txt` and is managed with the `add_libc_test` helper
function. To enable more tests edit the section and add more tests to be
listed.

The `cmake` configuration step will print out where `libc-test` is located:

```
$ cmake -S test -B testbuild
...
-- libc-test source directory: $PWD/testbuild/_deps/libc-test-src
...
```

You can explore this direcotry to see what tests are available and have not yet
been enabled.

[libc-test]: https://wiki.musl-libc.org/libc-test.html
[`src`]: src
