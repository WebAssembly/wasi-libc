#!/bin/bash -e
export TARGET_ARCH=wasm32
export TARGET_OS=wasi
cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
make -j 14
