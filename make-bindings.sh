#!/bin/bash

# As of this writing, the `wit` directory was copied from
# https://github.com/bytecodealliance/wasmtime/tree/759aa585496adfbe47aefb4fdf9608aabf8d64ac/crates/wasi/wit

set -e

wit-bindgen c --world wasi:cli/imports@0.2.0-rc-2023-12-05 \
    --rename wasi:clocks/monotonic-clock@0.2.0-rc-2023-11-10=monotonic_clock \
    --rename wasi:clocks/wall-clock@0.2.0-rc-2023-11-10=wall_clock \
    --rename wasi:filesystem/preopens@0.2.0-rc-2023-11-10=filesystem_preopens \
    --rename wasi:filesystem/types@0.2.0-rc-2023-11-10=filesystem \
    --rename wasi:io/error@0.2.0-rc-2023-11-10=io_error \
    --rename wasi:io/poll@0.2.0-rc-2023-11-10=poll \
    --rename wasi:io/streams@0.2.0-rc-2023-11-10=streams \
    --rename wasi:random/insecure-seed@0.2.0-rc-2023-11-10=random_insecure_seed \
    --rename wasi:random/insecure@0.2.0-rc-2023-11-10=random_insecure \
    --rename wasi:random/random@0.2.0-rc-2023-11-10=random \
    --rename wasi:sockets/instance-network@0.2.0-rc-2023-11-10=instance_network \
    --rename wasi:sockets/ip-name-lookup@0.2.0-rc-2023-11-10=ip_name_lookup \
    --rename wasi:sockets/network@0.2.0-rc-2023-11-10=network \
    --rename wasi:sockets/tcp-create-socket@0.2.0-rc-2023-11-10=tcp_create_socket \
    --rename wasi:sockets/tcp@0.2.0-rc-2023-11-10=tcp \
    --rename wasi:sockets/udp-create-socket@0.2.0-rc-2023-11-10=udp_create_socket \
    --rename wasi:sockets/udp@0.2.0-rc-2023-11-10=udp \
    --rename wasi:cli/environment@0.2.0-rc-2023-12-05=environment \
    --rename wasi:cli/exit@0.2.0-rc-2023-12-05=exit \
    --rename wasi:cli/stdin@0.2.0-rc-2023-12-05=stdin \
    --rename wasi:cli/stdout@0.2.0-rc-2023-12-05=stdout \
    --rename wasi:cli/stderr@0.2.0-rc-2023-12-05=stderr \
    --rename wasi:cli/terminal-input@0.2.0-rc-2023-12-05=terminal_input \
    --rename wasi:cli/terminal-output@0.2.0-rc-2023-12-05=terminal_output \
    --rename wasi:cli/terminal-stdin@0.2.0-rc-2023-12-05=terminal_stdin \
    --rename wasi:cli/terminal-stdout@0.2.0-rc-2023-12-05=terminal_stdout \
    --rename wasi:cli/terminal-stderr@0.2.0-rc-2023-12-05=terminal_stderr \
    ./wit
mv imports.h libc-bottom-half/headers/private/preview2.h
sed 's/#include "imports.h"/#include "preview2.h"/' < imports.c > libc-bottom-half/cloudlibc/src/libc/sys/wasi_preview2/preview2.c
rm imports.c
mv imports_component_type.o libc-bottom-half/cloudlibc/src/libc/sys/wasi_preview2/preview2_component_type.o
