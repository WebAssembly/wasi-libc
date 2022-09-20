cmake_minimum_required(VERSION 3.16.3)

project(wasi-libc LANGUAGES C)

enable_testing()

if("${CMAKE_C_COMPILER}" MATCHES "(.*)clang(-[0-9]+)?$")
  set(CMAKE_NM "${CMAKE_MATCH_1}llvm-nm${CMAKE_MATCH_2}")
endif()

find_package(Python COMPONENTS Interpreter REQUIRED)

set(TARGET_TRIPLE wasm32-wasi)
set(MULTIARCH_TRIPLE wasm32-wasi)

set(CMAKE_C_COMPILER_TARGET ${TARGET_TRIPLE} CACHE STRING "The target to compile for")

# note that pthread support is still a work-in-progress.
set(THREAD_MODEL "single" CACHE STRING "single or posix")

set(MALLOC_IMPL "dlmalloc" CACHE STRING "dlmalloc or none")

option(BUILD_LIBC_TOP_HALF "Build libc top half" ON)

# When the length is no larger than this threshold, we consider the
# overhead of bulk memory opcodes to outweigh the performance benefit,
# and fall back to the original musl implementation. See
# https://github.com/WebAssembly/wasi-libc/pull/263 for relevant
# discussion
set(BULK_MEMORY_THRESHOLD "32" CACHE STRING "bulk memory threshold")

add_subdirectory(cmake)