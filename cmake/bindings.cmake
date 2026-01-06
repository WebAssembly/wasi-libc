# Enable turning this rule off entirely if so desired.
option(BINDINGS_TARGET "Generate bindings target" ON)
if (NOT BINDINGS_TARGET)
  return()
endif()

# If `wit-bindgen` is on the system and has the right version, favor that,
# otherwise download a known good version.
find_program(WIT_BINDGEN_EXECUTABLE NAMES wit-bindgen)
if(WIT_BINDGEN_EXECUTABLE)
  message(STATUS "Found wit-bindgen: ${WIT_BINDGEN_EXECUTABLE}")

  execute_process(
    COMMAND ${WIT_BINDGEN_EXECUTABLE} --version
    OUTPUT_VARIABLE WIT_BINDGEN_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  if (NOT (WIT_BINDGEN_VERSION MATCHES "0\\.48\\.0"))
    message(WARNING "wit-bindgen version 0.48.0 is required, found: ${WIT_BINDGEN_VERSION}")
    set(WIT_BINDGEN_EXECUTABLE "")
  endif()
endif()

if (NOT WIT_BINDGEN_EXECUTABLE)
  include(ba-download)
  ba_download(
    wit-bindgen
    "https://github.com/bytecodealliance/wit-bindgen"
    "0.48.0"
  )
  ExternalProject_Get_Property(wit-bindgen SOURCE_DIR)
  set(wit_bindgen "${SOURCE_DIR}/wit-bindgen")
else()
  set(wit_bindgen ${WIT_BINDGEN_EXECUTABLE})
endif()

include(ExternalProject)
ExternalProject_Add(
  wasi-wits
  URL https://github.com/WebAssembly/wasi-cli/archive/refs/tags/v0.2.0.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  EXCLUDE_FROM_ALL TRUE
)
ExternalProject_Get_Property(wasi-wits SOURCE_DIR)
set(wit_dir ${SOURCE_DIR}/wit)
set(bottom_half "${CMAKE_SOURCE_DIR}/libc-bottom-half")

add_custom_target(
  bindings
  COMMAND
    ${wit_bindgen} c
      --autodrop-borrows yes
      --rename-world wasip2
      --type-section-suffix __wasi_libc
      --world wasi:cli/imports@0.2.0
      --rename wasi:clocks/monotonic-clock@0.2.0=monotonic_clock
      --rename wasi:clocks/wall-clock@0.2.0=wall_clock
      --rename wasi:filesystem/preopens@0.2.0=filesystem_preopens
      --rename wasi:filesystem/types@0.2.0=filesystem
      --rename wasi:io/error@0.2.0=io_error
      --rename wasi:io/poll@0.2.0=poll
      --rename wasi:io/streams@0.2.0=streams
      --rename wasi:random/insecure-seed@0.2.0=random_insecure_seed
      --rename wasi:random/insecure@0.2.0=random_insecure
      --rename wasi:random/random@0.2.0=random
      --rename wasi:sockets/instance-network@0.2.0=instance_network
      --rename wasi:sockets/ip-name-lookup@0.2.0=ip_name_lookup
      --rename wasi:sockets/network@0.2.0=network
      --rename wasi:sockets/tcp-create-socket@0.2.0=tcp_create_socket
      --rename wasi:sockets/tcp@0.2.0=tcp
      --rename wasi:sockets/udp-create-socket@0.2.0=udp_create_socket
      --rename wasi:sockets/udp@0.2.0=udp
      --rename wasi:cli/environment@0.2.0=environment
      --rename wasi:cli/exit@0.2.0=exit
      --rename wasi:cli/stdin@0.2.0=stdin
      --rename wasi:cli/stdout@0.2.0=stdout
      --rename wasi:cli/stderr@0.2.0=stderr
      --rename wasi:cli/terminal-input@0.2.0=terminal_input
      --rename wasi:cli/terminal-output@0.2.0=terminal_output
      --rename wasi:cli/terminal-stdin@0.2.0=terminal_stdin
      --rename wasi:cli/terminal-stdout@0.2.0=terminal_stdout
      --rename wasi:cli/terminal-stderr@0.2.0=terminal_stderr
      ${wit_dir}
  COMMAND cmake -E copy wasip2.h ${bottom_half}/headers/public/wasi/
  COMMAND cmake -E copy wasip2_component_type.o ${bottom_half}/sources
  COMMAND cmake -E copy wasip2.c ${bottom_half}/sources
  COMMAND sed -i "'s_#include .wasip2\.h._#include \"wasi/wasip2.h\"_'" ${bottom_half}/sources/wasip2.c
  COMMAND sed -i "s/extern void exit_exit/_Noreturn extern void exit_exit/" ${bottom_half}/headers/public/wasi/wasip2.h
  COMMAND sed -i "s/extern void __wasm_import_exit_exit/_Noreturn extern void __wasm_import_exit_exit/" ${bottom_half}/sources/wasip2.c
  DEPENDS wit-bindgen wasi-wits
)
