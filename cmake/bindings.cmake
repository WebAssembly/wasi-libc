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

  if (NOT (WIT_BINDGEN_VERSION MATCHES "0\\.51\\.0"))
    message(WARNING "wit-bindgen version 0.51.0 is required, found: ${WIT_BINDGEN_VERSION}")
    set(WIT_BINDGEN_EXECUTABLE "")
  endif()
endif()

if (NOT WIT_BINDGEN_EXECUTABLE)
  include(ba-download)
  ba_download(
    wit-bindgen
    "https://github.com/bytecodealliance/wit-bindgen"
    "0.51.0"
  )
  ExternalProject_Get_Property(wit-bindgen SOURCE_DIR)
  set(wit_bindgen "${SOURCE_DIR}/wit-bindgen")
else()
  add_custom_target(wit-bindgen)
  set(wit_bindgen ${WIT_BINDGEN_EXECUTABLE})
endif()

include(ExternalProject)
set(p2 0.2.0)
ExternalProject_Add(
  wasip2-wits
  URL https://github.com/WebAssembly/wasi-cli/archive/refs/tags/v${p2}.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  EXCLUDE_FROM_ALL TRUE
)
ExternalProject_Get_Property(wasip2-wits SOURCE_DIR)
set(wasip2_wit_dir ${SOURCE_DIR}/wit)
set(bottom_half "${CMAKE_SOURCE_DIR}/libc-bottom-half")

add_custom_target(
  bindings-p2
  COMMAND
    ${wit_bindgen} c
      --autodrop-borrows yes
      --rename-world wasip2
      --type-section-suffix __wasi_libc
      --world wasi:cli/imports@${p2}
      --rename wasi:clocks/monotonic-clock@${p2}=monotonic_clock
      --rename wasi:clocks/wall-clock@${p2}=wall_clock
      --rename wasi:filesystem/preopens@${p2}=filesystem_preopens
      --rename wasi:filesystem/types@${p2}=filesystem
      --rename wasi:io/error@${p2}=io_error
      --rename wasi:io/poll@${p2}=poll
      --rename wasi:io/streams@${p2}=streams
      --rename wasi:random/insecure-seed@${p2}=random_insecure_seed
      --rename wasi:random/insecure@${p2}=random_insecure
      --rename wasi:random/random@${p2}=random
      --rename wasi:sockets/instance-network@${p2}=instance_network
      --rename wasi:sockets/ip-name-lookup@${p2}=ip_name_lookup
      --rename wasi:sockets/network@${p2}=network
      --rename wasi:sockets/tcp-create-socket@${p2}=tcp_create_socket
      --rename wasi:sockets/tcp@${p2}=tcp
      --rename wasi:sockets/udp-create-socket@${p2}=udp_create_socket
      --rename wasi:sockets/udp@${p2}=udp
      --rename wasi:cli/environment@${p2}=environment
      --rename wasi:cli/exit@${p2}=exit
      --rename wasi:cli/stdin@${p2}=stdin
      --rename wasi:cli/stdout@${p2}=stdout
      --rename wasi:cli/stderr@${p2}=stderr
      --rename wasi:cli/terminal-input@${p2}=terminal_input
      --rename wasi:cli/terminal-output@${p2}=terminal_output
      --rename wasi:cli/terminal-stdin@${p2}=terminal_stdin
      --rename wasi:cli/terminal-stdout@${p2}=terminal_stdout
      --rename wasi:cli/terminal-stderr@${p2}=terminal_stderr
      ${wasip2_wit_dir}
  COMMAND cmake -E copy wasip2.h ${bottom_half}/headers/public/wasi/__generated_wasip2.h
  COMMAND cmake -E copy wasip2_component_type.o ${bottom_half}/sources
  COMMAND cmake -E copy wasip2.c ${bottom_half}/sources
  DEPENDS wit-bindgen wasip2-wits
)

set(p3 0.3.0-rc-2025-09-16)
ExternalProject_Add(
  wasip3-wits
  URL https://github.com/WebAssembly/wasi-cli/archive/refs/tags/v${p3}.tar.gz
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  EXCLUDE_FROM_ALL TRUE
)
ExternalProject_Get_Property(wasip3-wits SOURCE_DIR)
set(wasip3_wit_dir ${SOURCE_DIR}/wit-0.3.0-draft)

add_custom_target(
  bindings-p3
  COMMAND
    ${wit_bindgen} c
      --generate-threading-helpers
      --autodrop-borrows yes
      --rename-world wasip3
      --type-section-suffix __wasi_libc
      --world wasi:cli/imports@${p3}
      --rename wasi:clocks/monotonic-clock@${p3}=monotonic_clock
      --rename wasi:clocks/wall-clock@${p3}=wall_clock
      --rename wasi:filesystem/preopens@${p3}=filesystem_preopens
      --rename wasi:filesystem/types@${p3}=filesystem
      --rename wasi:random/insecure-seed@${p3}=random_insecure_seed
      --rename wasi:random/insecure@${p3}=random_insecure
      --rename wasi:random/random@${p3}=random
      --rename wasi:sockets/types@${p3}=sockets
      --rename wasi:sockets/ip-name-lookup@${p3}=ip_name_lookup
      --rename wasi:cli/environment@${p3}=environment
      --rename wasi:cli/exit@${p3}=exit
      --rename wasi:cli/stdin@${p3}=stdin
      --rename wasi:cli/stdout@${p3}=stdout
      --rename wasi:cli/stderr@${p3}=stderr
      --rename wasi:cli/terminal-input@${p3}=terminal_input
      --rename wasi:cli/terminal-output@${p3}=terminal_output
      --rename wasi:cli/terminal-stdin@${p3}=terminal_stdin
      --rename wasi:cli/terminal-stdout@${p3}=terminal_stdout
      --rename wasi:cli/terminal-stderr@${p3}=terminal_stderr
      ${wasip3_wit_dir}
  COMMAND cmake -E copy wasip3.h ${bottom_half}/headers/public/wasi/__generated_wasip3.h
  COMMAND cmake -E copy wasip3_component_type.o ${bottom_half}/sources
  COMMAND cmake -E copy wasip3.c ${bottom_half}/sources
  DEPENDS wit-bindgen wasip3-wits
)

add_custom_target(bindings DEPENDS bindings-p2 bindings-p3)

function(wit_bindgen_edit p)
  add_custom_target(
    bindings-${p}-edit
    COMMAND sed -i '' "'s_#include .wasi${p}\.h._#include \"wasi/wasi${p}.h\"_'" ${bottom_half}/sources/wasi${p}.c
    COMMAND sed -i '' "s/extern void exit_exit/_Noreturn extern void exit_exit/" ${bottom_half}/headers/public/wasi/__generated_wasi${p}.h
    COMMAND sed -i '' "s/extern void __wasm_import_exit_exit/_Noreturn extern void __wasm_import_exit_exit/" ${bottom_half}/sources/wasi${p}.c
    DEPENDS bindings-${p}
  )
  add_dependencies(bindings bindings-${p}-edit)
endfunction()

wit_bindgen_edit(p2)
wit_bindgen_edit(p3)
