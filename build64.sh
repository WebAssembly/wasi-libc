#!/bin/bash -e
export TARGET_ARCH=wasm64
export TARGET_OS=wasix
cargo run --manifest-path tools/wasix-headers/Cargo.toml generate-libc
make -j 14
