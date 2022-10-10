# WebAssembly floating-point match doesn't trap.
# TODO: Add -fno-signaling-nans when the compiler supports it.
add_compile_options(-fno-trapping-math)

# Add all warnings, but disable a few which occur in third-party code.
add_compile_options(
  -Wall -Wextra -Werror
  -Wno-null-pointer-arithmetic
  -Wno-unused-parameter
  -Wno-sign-compare
  -Wno-unused-variable
  -Wno-unused-function
  -Wno-ignored-attributes
  -Wno-missing-braces
  -Wno-ignored-pragmas
  -Wno-unused-but-set-variable
  -Wno-unknown-warning-option
)

# Configure support for threads
if("${THREAD_MODEL}" STREQUAL "single")
  add_compile_options(-mthread-model single)
elseif("${THREAD_MODEL}" STREQUAL "posix")
  add_compile_options(-mthread-model posix -pthread -ftls-model=local-exec)
endif()

# Expose the public headers to the implementation. We use `-isystem` for
# purpose for two reasons:
#
# 1. It only does `<...>` not `"...."` lookup. We are making a libc,
#    which is a system library, so all public headers should be
#    accessible via `<...>` and never also need `"..."`. `-isystem` main
#    purpose is to only effect `<...>` lookup.
#
# 2. The `-I` for private headers added for specific C files below
#    should come earlier in the search path, so they can "override"
#    and/or `#include_next` the public headers. `-isystem` (like
#    `-idirafter`) comes later in the search path than `-I`.
include_directories(SYSTEM ${CMAKE_BINARY_DIR}/sysroot/include)
