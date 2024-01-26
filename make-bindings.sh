#!/bin/bash

# This script will regenerate the WASI Preview 2 bindings.  To use it, first
# clone the wit directory from
# https://github.com/WebAssembly/wasi-cli/tree/main/wit, install
# https://github.com/bytecodealliance/wit-bindgen/releases/tag/wit-bindgen-cli-0.16.0
# (or newer), and then run the script.
#
# TODO: Automate the above instructions.

set -e

wit-bindgen c --world wasi:cli/imports@0.2.0 \
    --rename wasi:clocks/monotonic-clock@0.2.0=monotonic_clock \
    --rename wasi:clocks/wall-clock@0.2.0=wall_clock \
    --rename wasi:filesystem/preopens@0.2.0=filesystem_preopens \
    --rename wasi:filesystem/types@0.2.0=filesystem \
    --rename wasi:io/error@0.2.0=io_error \
    --rename wasi:io/poll@0.2.0=poll \
    --rename wasi:io/streams@0.2.0=streams \
    --rename wasi:random/insecure-seed@0.2.0=random_insecure_seed \
    --rename wasi:random/insecure@0.2.0=random_insecure \
    --rename wasi:random/random@0.2.0=random \
    --rename wasi:sockets/instance-network@0.2.0=instance_network \
    --rename wasi:sockets/ip-name-lookup@0.2.0=ip_name_lookup \
    --rename wasi:sockets/network@0.2.0=network \
    --rename wasi:sockets/tcp-create-socket@0.2.0=tcp_create_socket \
    --rename wasi:sockets/tcp@0.2.0=tcp \
    --rename wasi:sockets/udp-create-socket@0.2.0=udp_create_socket \
    --rename wasi:sockets/udp@0.2.0=udp \
    --rename wasi:cli/environment@0.2.0=environment \
    --rename wasi:cli/exit@0.2.0=exit \
    --rename wasi:cli/stdin@0.2.0=stdin \
    --rename wasi:cli/stdout@0.2.0=stdout \
    --rename wasi:cli/stderr@0.2.0=stderr \
    --rename wasi:cli/terminal-input@0.2.0=terminal_input \
    --rename wasi:cli/terminal-output@0.2.0=terminal_output \
    --rename wasi:cli/terminal-stdin@0.2.0=terminal_stdin \
    --rename wasi:cli/terminal-stdout@0.2.0=terminal_stdout \
    --rename wasi:cli/terminal-stderr@0.2.0=terminal_stderr \
    ./wit
mv imports.h libc-bottom-half/headers/private/preview2.h
sed 's/#include "imports.h"/#include "preview2.h"/' < imports.c > libc-bottom-half/cloudlibc/src/libc/sys/wasi_preview2/preview2.c
rm imports.c
mv imports_component_type.o libc-bottom-half/cloudlibc/src/libc/sys/wasi_preview2/preview2_component_type.o
