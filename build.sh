#!/bin/bash -e
cargo run --manifest-path tools/wasi-headers/Cargo.toml generate-libc
