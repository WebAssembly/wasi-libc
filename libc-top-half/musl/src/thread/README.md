# pthreads

This directory contains two implementations of the `pthreads` library:

- `wasi-threads` - based on the original musl implementation, used for the `wasm32-wasip1-threads` target; see [`wasi-threads`](https://github.com/WebAssembly/wasi-threads) for details
- `stubs` - used for single-threaded WASIP1 and WASIP2 targets; is never able to spawn new threads, but otherwise follows the letter of the specification
