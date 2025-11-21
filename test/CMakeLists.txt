cmake_minimum_required(VERSION 3.26)
project(wasi-libc-tests)

if(NOT CMAKE_C_COMPILER_ID MATCHES Clang)
  message(FATAL_ERROR "C compiler ${CMAKE_C_COMPILER} is not `Clang`, it is ${CMAKE_C_COMPILER_ID}")
endif()

message(STATUS "Found executable for `nm`: ${CMAKE_NM}")
message(STATUS "Found executable for `ar`: ${CMAKE_AR}")
message(STATUS "Found executable for `ranlib`: ${CMAKE_RANLIB}")

include(FetchContent)
include(ExternalProject)
include(CTest)
enable_testing()

set(TARGET_TRIPLE "wasm32-wasi" CACHE STRING "WASI target to test")
set(MALLOC_IMPL "" CACHE STRING "Malloc implementation in use")
option(PYTHON_TESTS "Build Python with this wasi-libc and run its tests" OFF)

# ========= Sysroot sanity check ================================

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH LIBC_SRC_DIR)
set(SYSROOT_DIR "${LIBC_SRC_DIR}/sysroot")
set(SYSROOT "${SYSROOT_DIR}/lib/${TARGET_TRIPLE}")

# This build configuration does not currently manage the sysroot itself (but
# ideally it will one day). Double-check that the sysroot exists before actually
# trying to build any tests.
if (NOT EXISTS "${SYSROOT_DIR}")
  message(FATAL_ERROR "
  No sysroot for ${TARGET_TRIPLE} available at ${SYSROOT_DIR}; to build it, e.g.:
    cd ${LIBC_SRC_DIR}
    make TARGET_TRIPLE=${TARGET_TRIPLE}
  ")
endif()

# ========= Clone libc-test =====================================

FetchContent_Declare(
  libc-test
  GIT_REPOSITORY https://github.com/bytecodealliance/libc-test
  GIT_TAG 18e28496adee3d84fefdda6efcb9c5b8996a2398
  GIT_SHALLOW true
)
FetchContent_MakeAvailable(libc-test)
set(LIBC_TEST "${libc-test_SOURCE_DIR}")
message(STATUS "libc-test source directory: ${LIBC_TEST}")

# ========= Download wasmtime as a test runner ==================

if(NOT ENGINE OR ENGINE STREQUAL "")
  set(WASMTIME_VERSION "v38.0.2")
  set(WASMTIME_REPO "https://github.com/bytecodealliance/wasmtime")

  if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(WASMTIME_ARCH "x86_64")
  elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
    set(WASMTIME_ARCH "aarch64")
  elseif (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(WASMTIME_ARCH "aarch64")
  else()
    message(FATAL_ERROR "Unsupported architecture ${CMAKE_HOST_SYSTEM_PROCESSOR} for Wasmtime")
  endif()

  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(WASMTIME_OS macos)
  elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(WASMTIME_OS linux)
  else()
    message(FATAL_ERROR "Unsupported system ${CMAKE_SYSTEM_NAME} for Wasmtime")
  endif()

  ExternalProject_Add(
    engine
    URL "${WASMTIME_REPO}/releases/download/${WASMTIME_VERSION}/wasmtime-${WASMTIME_VERSION}-${WASMTIME_ARCH}-${WASMTIME_OS}.tar.xz"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  ExternalProject_Get_Property(engine SOURCE_DIR)
  set(ENGINE "${SOURCE_DIR}/wasmtime")
  message(STATUS "Wasmtime executable: ${ENGINE}")
else()
  add_custom_target(engine)
endif()

# ========= libc-test defined tests =============================

function(add_wasilibc_flags target)
  target_compile_options(${target} PRIVATE
    "--target=${TARGET_TRIPLE}"
    "--sysroot=${SYSROOT_DIR}"
  )
  target_link_options(${target} PRIVATE
    "--target=${TARGET_TRIPLE}"
    "--sysroot=${SYSROOT_DIR}"
    "-resource-dir=${LIBC_SRC_DIR}/build/${TARGET_TRIPLE}/resource-dir"
  )
  if (TARGET_TRIPLE MATCHES "-threads")
    target_compile_options(${target} PRIVATE -pthread)
    target_link_options(${target} PRIVATE -pthread
      -Wl,--import-memory,--export-memory,--shared-memory,--max-memory=1073741824)
  endif()
endfunction()

add_library(libc_test_support STATIC
  "${LIBC_TEST}/src/common/path.c"
  "${LIBC_TEST}/src/common/print.c"
  "${LIBC_TEST}/src/common/rand.c"
  "${LIBC_TEST}/src/common/utf8.c"
)
target_include_directories(libc_test_support PUBLIC "${LIBC_TEST}/src/common")
add_wasilibc_flags(libc_test_support)

# Adds a new test executable to build
#
# * `executable_name` must be a unique name and valid cmake target name.
# * `src` is the path to the test file to compile.
#
# Optional arguments:
#
# * `CFLAGS -a -b -c` - additional flags to pass to the compiler
# * `LDFLAGS -a -b -c` - additional flags to pass to the linker
function(add_test_executable executable_name src)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs LDFLAGS CFLAGS)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  # Build the test exeutable itself and apply all custom options as applicable.
  add_executable(${executable_name} ${src})
  add_wasilibc_flags(${executable_name})
  target_link_libraries(${executable_name} libc_test_support)
  foreach(flag IN LISTS arg_CFLAGS)
    target_compile_options(${executable_name} PRIVATE ${flag})
  endforeach()
  foreach(flag IN LISTS arg_LDFLAGS)
    target_link_options(${executable_name} PRIVATE ${flag})
  endforeach()
  target_include_directories(${executable_name} PRIVATE "${LIBC_TEST}")
endfunction()

# Adds a new test to run.
#
# * `test_name` must be a unique name and valid cmake target name.
# * `test_file` is the path to the test file to compile.
#
# Optional arguments:
#
# * `FS` - this test requires a temporary directory mounted as `/`
# * `ARGV arg1 arg2` - additional arguments to pass to the test at runtime
# * `ENV a=b b=c` - set env vars for when executing this test
# * `NETWORK` - this test uses the network and sockets.
# * `PASS_REGULAR_EXPRESSION` - a regex that must match the test output to pass
function(register_test test_name executable_name)
  set(options FS NETWORK)
  set(oneValueArgs CLIENT PASS_REGULAR_EXPRESSION)
  set(multiValueArgs ARGV ENV LDFLAGS CFLAGS)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  set(wasmtime_args)

  if (arg_FS)
    set(fsdir "${CMAKE_CURRENT_BINARY_DIR}/tmp/${test_name}/fs")
    list(APPEND wasmtime_args --dir ${fsdir}::/)
  endif()
  if (arg_NETWORK)
    list(APPEND wasmtime_args -Sinherit-network)
  endif()
  foreach(env IN LISTS arg_ENV)
    list(APPEND wasmtime_args --env ${env})
  endforeach()
  if (TARGET_TRIPLE MATCHES "-threads")
    list(APPEND wasmtime_args --wasi threads)
  endif()

  add_test(
    NAME "${test_name}"
    COMMAND
      ${ENGINE}
        ${wasmtime_args}
        $<TARGET_FILE:${executable_name}> ${arg_ARGV}
  )

  # Use CTest fixtures to create a the temporary directory before the test
  # starts running and clean it up afterwards.
  if (arg_FS)
    add_test(NAME "setup_${test_name}" COMMAND mkdir -p ${fsdir})
    add_test(NAME "cleanup_${test_name}" COMMAND rm -rf ${fsdir})
    set_tests_properties("setup_${test_name}" PROPERTIES FIXTURES_SETUP "fs_${test_name}")
    set_tests_properties("cleanup_${test_name}" PROPERTIES FIXTURES_CLEANUP "fs_${test_name}")
    set_tests_properties("${test_name}" PROPERTIES FIXTURES_REQUIRED "fs_${test_name}")
  endif()

  # All sockets tests use the same port right now, so only one can run at a
  # time.
  if (arg_NETWORK)
    set_tests_properties(${test_name} PROPERTIES RESOURCE_LOCK socket-test)
  endif()

  if (arg_PASS_REGULAR_EXPRESSION)
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${arg_PASS_REGULAR_EXPRESSION}")
  endif()
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)

  add_dependencies(${test_name} engine)
endfunction()

# Adds a new test from the `libc-test` repository where `test_file` is a
# relative path from the `src` directory.
#
# Also supports options `register_test` does.
function(add_libc_test test_file)
  cmake_path(REPLACE_EXTENSION test_file wasm OUTPUT_VARIABLE test_name)
  string(REPLACE "/" "_" test_name ${test_name})
  set(test_name "libc_test_${test_name}")
  set(test_file "${LIBC_TEST}/src/${test_file}")

  add_test_executable(${test_name} "${test_file}" ${ARGN})
  register_test(${test_name} ${test_name} ${ARGN})
endfunction()

add_libc_test(functional/argv.c)
add_libc_test(functional/basename.c)
add_libc_test(functional/clocale_mbfuncs.c)
add_libc_test(functional/clock_gettime.c)
add_libc_test(functional/crypt.c)
add_libc_test(functional/dirname.c)
add_libc_test(functional/env.c)
add_libc_test(functional/fnmatch.c)
add_libc_test(functional/iconv_open.c)
add_libc_test(functional/mbc.c)
add_libc_test(functional/memstream.c)
add_libc_test(functional/qsort.c)
add_libc_test(functional/random.c)
add_libc_test(functional/search_hsearch.c)
if (MALLOC_IMPL STREQUAL "emmalloc")
  set_tests_properties(libc_test_functional_search_hsearch.wasm PROPERTIES WILL_FAIL TRUE)
endif()
add_libc_test(functional/search_insque.c)
add_libc_test(functional/search_lsearch.c)
add_libc_test(functional/search_tsearch.c)
add_libc_test(functional/snprintf.c)
add_libc_test(functional/sscanf.c)
add_libc_test(functional/strftime.c)
add_libc_test(functional/string.c)
add_libc_test(functional/string_memcpy.c)
add_libc_test(functional/string_memmem.c)
add_libc_test(functional/string_memset.c)
add_libc_test(functional/string_strchr.c)
add_libc_test(functional/string_strcspn.c)
add_libc_test(functional/string_strstr.c)
add_libc_test(functional/strtod.c)
add_libc_test(functional/strtod_long.c)
add_libc_test(functional/strtod_simple.c)
add_libc_test(functional/strtof.c)
add_libc_test(functional/strtol.c)
add_libc_test(functional/strtold.c LDFLAGS -lc-printscan-long-double)
add_libc_test(functional/swprintf.c)
add_libc_test(functional/tgmath.c)
add_libc_test(functional/udiv.c)
add_libc_test(functional/wcsstr.c)
add_libc_test(functional/wcstol.c)

if (TARGET_TRIPLE MATCHES "-threads")
  add_libc_test(functional/pthread_mutex.c)
  add_libc_test(functional/pthread_tsd.c)
  add_libc_test(functional/pthread_cond.c)
endif()

# ========= wasi-libc-test defined tests ========================

function(add_wasilibc_test test_file)
  cmake_path(REPLACE_EXTENSION test_file wasm OUTPUT_VARIABLE test_name)
  set(test_file "${CMAKE_CURRENT_SOURCE_DIR}/src/${test_file}")

  add_test_executable(${test_name} "${test_file}" ${ARGN})
  register_test(${test_name} ${test_name} ${ARGN})
endfunction()

# TODO: this test fails with `-Sthreads` in Wasmtime since that uses a different
# implementation of WASI which causes this test to fail.
if (NOT TARGET_TRIPLE MATCHES "-threads")
  add_wasilibc_test(access.c FS)
endif()
add_wasilibc_test(append.c FS)
add_wasilibc_test(argv_two_args.c ARGV foo bar)
add_wasilibc_test(clock_nanosleep.c)
add_wasilibc_test(chdir.c FS)
add_wasilibc_test(close.c FS)
add_wasilibc_test(external_env.c ENV VAR1=foo VAR2=bar)
add_wasilibc_test(fadvise.c FS)
add_wasilibc_test(fallocate.c FS)
add_wasilibc_test(fcntl.c FS)
add_wasilibc_test(fdatasync.c FS)
add_wasilibc_test(fdopen.c FS)
add_wasilibc_test(feof.c FS)
add_wasilibc_test(file_permissions.c FS)
add_wasilibc_test(file_nonblocking.c FS)
add_wasilibc_test(fseek.c FS)
add_wasilibc_test(fstat.c FS)
add_wasilibc_test(fsync.c FS)
add_wasilibc_test(ftruncate.c FS)
add_wasilibc_test(fts.c FS)
add_wasilibc_test(fwscanf.c FS)
add_wasilibc_test(getentropy.c)
add_wasilibc_test(hello.c PASS_REGULAR_EXPRESSION "Hello, World!")
add_wasilibc_test(ioctl.c FS)
add_wasilibc_test(isatty.c FS)
add_wasilibc_test(link.c FS)
add_wasilibc_test(lseek.c FS)
add_wasilibc_test(memchr.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(memcmp.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(opendir.c FS ARGV /)
add_wasilibc_test(open_relative_path.c FS ARGV /)
add_wasilibc_test(poll.c FS)
add_wasilibc_test(preadvwritev.c FS)
add_wasilibc_test(preadwrite.c FS)
add_wasilibc_test(readlink.c FS)
add_wasilibc_test(readv.c FS)
add_wasilibc_test(rename.c FS)
add_wasilibc_test(rmdir.c FS)
add_wasilibc_test(scandir.c FS)
add_wasilibc_test(stat.c FS)
add_wasilibc_test(stdio.c FS)
add_wasilibc_test(strchrnul.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(strlen.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(strptime.c)
add_wasilibc_test(strrchr.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(time_and_times.c
  CFLAGS -D_WASI_EMULATED_PROCESS_CLOCKS
  LDFLAGS -lwasi-emulated-process-clocks)
add_wasilibc_test(time.c)
add_wasilibc_test(utime.c FS)
add_wasilibc_test(rewinddir.c FS)
add_wasilibc_test(seekdir.c FS)
add_wasilibc_test(usleep.c)
add_wasilibc_test(write.c FS)

if (TARGET_TRIPLE MATCHES "-threads")
  add_wasilibc_test(busywait.c)
  add_wasilibc_test(pthread_cond_busywait.c)
  add_wasilibc_test(pthread_tsd_busywait.c)
  add_wasilibc_test(pthread_mutex_busywait.c)
endif()

# ========= sockets-related tests ===============================

if (TARGET_TRIPLE MATCHES "wasip2")
  add_wasilibc_test(poll-nonblocking-socket.c NETWORK)
  add_wasilibc_test(setsockopt.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-udp.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-multiple.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-udp-multiple.c NETWORK)

  # TODO: flaky tests
  # add_wasilibc_test(sockets-nonblocking.c NETWORK)
  # add_wasilibc_test(sockets-nonblocking-udp-no-connection.c NETWORK)

  # Define executables for server/client tests, and they're paired together in
  # various combinations below for various tests.
  function(add_sockets_test_executable path)
    cmake_path(REPLACE_EXTENSION path wasm OUTPUT_VARIABLE exe_name)
    set(path "src/${path}")
    add_test_executable(${exe_name} ${path})
  endfunction()

  add_sockets_test_executable(sockets-client.c)
  add_sockets_test_executable(sockets-client-handle-hangups.c)
  add_sockets_test_executable(sockets-client-hangup-after-connect.c)
  add_sockets_test_executable(sockets-client-hangup-after-sending.c)
  add_sockets_test_executable(sockets-client-hangup-while-receiving.c)
  add_sockets_test_executable(sockets-client-hangup-while-sending.c)
  add_sockets_test_executable(sockets-client-udp-blocking.c)
  add_sockets_test_executable(sockets-multiple-client.c)
  add_sockets_test_executable(sockets-server.c)
  add_sockets_test_executable(sockets-server-handle-hangups.c)
  add_sockets_test_executable(sockets-server-hangup-before-recv.c)
  add_sockets_test_executable(sockets-server-hangup-before-send.c)
  add_sockets_test_executable(sockets-server-hangup-during-recv.c)
  add_sockets_test_executable(sockets-server-hangup-during-send.c)
  add_sockets_test_executable(sockets-server-udp-blocking.c)
  add_sockets_test_executable(sockets-multiple-server.c)

  function(sockets_test test_name client server)
    set(options)
    set(oneValueArgs NCLIENTS)
    set(multiValueArgs)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

    add_test(
      NAME "${test_name}"
      COMMAND
        ${CMAKE_COMMAND}
          -DENGINE=${ENGINE}
          -DSERVER=$<TARGET_FILE:${server}>
          -DCLIENT=$<TARGET_FILE:${client}>
          -DNCLIENTS=${arg_NCLIENTS}
          -P ${CMAKE_CURRENT_SOURCE_DIR}/socket-test.cmake
    )
    set_tests_properties(${test_name} PROPERTIES RESOURCE_LOCK socket-test)
  endfunction()

  sockets_test(sockets sockets-client.wasm sockets-server.wasm)
  sockets_test(sockets-udp-blocking sockets-client-udp-blocking.wasm sockets-server-udp-blocking.wasm)
  sockets_test(sockets-multiple sockets-multiple-client.wasm sockets-multiple-server.wasm
    NCLIENTS 10)

  # Various forms of client hangups
  sockets_test(sockets-client-hangup-after-connect
    sockets-client-hangup-after-connect.wasm sockets-server-handle-hangups.wasm)
  sockets_test(sockets-client-hangup-while-sending
    sockets-client-hangup-while-sending.wasm sockets-server-handle-hangups.wasm)
  sockets_test(sockets-client-hangup-after-sending
    sockets-client-hangup-after-sending.wasm sockets-server-handle-hangups.wasm)
  sockets_test(sockets-client-hangup-while-receiving
    sockets-client-hangup-while-receiving.wasm sockets-server-handle-hangups.wasm)

  # Various forms of server hangups, including when there's no server at all
  sockets_test(sockets-server-hangup-before-send
    sockets-client-handle-hangups.wasm sockets-server-hangup-before-send.wasm)
  sockets_test(sockets-server-hangup-during-send
    sockets-client-handle-hangups.wasm sockets-server-hangup-during-send.wasm)
  sockets_test(sockets-server-hangup-before-recv
    sockets-client-handle-hangups.wasm sockets-server-hangup-before-recv.wasm)
  sockets_test(sockets-server-hangup-during-recv
    sockets-client-handle-hangups.wasm sockets-server-hangup-during-recv.wasm)
  sockets_test(sockets-client-handle-hangups
    sockets-client-handle-hangups.wasm hello.wasm)
endif()

# Flag some tests as failing in V8
set_tests_properties(hello.wasm PROPERTIES LABELS v8fail)
set_tests_properties(clock_nanosleep.wasm PROPERTIES LABELS v8fail)
# Skip test that uses environment variables
set_tests_properties(external_env.wasm PROPERTIES LABELS v8fail)
# Skip test that uses command-line arguments
set_tests_properties(argv_two_args.wasm PROPERTIES LABELS v8fail)
if (TARGET_TRIPLE MATCHES "-threads")
  # atomic.wait32 can't be executed on the main thread
  set_tests_properties(libc_test_functional_pthread_mutex.wasm PROPERTIES LABELS v8fail)
  set_tests_properties(libc_test_functional_pthread_tsd.wasm PROPERTIES LABELS v8fail)
  # "poll_oneoff" can't be implemented in the browser
  set_tests_properties(libc_test_functional_pthread_cond.wasm PROPERTIES LABELS v8fail)
endif()

# If enabled add a copy of Python which is built against `wasi-libc` and run
# its tests.
if (PYTHON_TESTS)
  find_program(PYTHON python3 python REQUIRED)
  find_program(MAKE make REQUIRED)

  set(flags "--target=${TARGET_TRIPLE} --sysroot=${SYSROOT_DIR}")

  ExternalProject_Add(
    python

    # Pin the source to 3.14 for now, but this is fine to change later if
    # tests still pass.
    URL https://github.com/python/cpython/archive/refs/tags/v3.14.0.tar.gz

    # Python as-is doesn't pass with the current wasi-libc. For example
    # wasi-libc now provides dummy pthread symbols which tricks Python into
    # thinking it can spawn threads, so a patch is needed for a WASI-specific
    # clause to disable that.
    #
    # More generally though this is an escape hatch to apply any other
    # changes as necessary without trying to upstream the patches to Python
    # itself. The patch is most "easily" generated by checking out cpython
    # at the `v3.14.0` tag, applying the existing patch, editing source,
    # and then regenerating the patch.
    PATCH_COMMAND
      patch -Np1 < ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cpython3.14.patch

    # The WASI build of Python looks to need an in-source build, or otherwise I
    # couldn't figure out an out-of-tree build.
    BUILD_IN_SOURCE ON

    # These steps take a long time, so stream the output to the terminal instead
    # of capturing it by default.
    USES_TERMINAL_CONFIGURE ON
    USES_TERMINAL_BUILD ON
    USES_TERMINAL_TEST ON

    # The following steps are copied from Python's own CI for managing WASI.
    # In general I don't know what they do. If Python's CI changes these
    # should change as well.
    #
    # More information about building Python for WASI can be found at
    # https://devguide.python.org/getting-started/setup-building/#wasi
    CONFIGURE_COMMAND
      ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi configure-build-python -- --config-cache --with-pydebug
    COMMAND
      ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi make-build-python

    BUILD_COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${flags} LDFLAGS=${flags} --
        ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi configure-host -- --config-cache
    COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${flags} LDFLAGS=${flags} --
        ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi make-host
    COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${flags} LDFLAGS=${flags} --
        ${MAKE} --directory cross-build/wasm32-wasip1 pythoninfo

    INSTALL_COMMAND ""

    TEST_COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${flags} LDFLAGS=${flags} --
        ${MAKE} --directory cross-build/wasm32-wasip1 test
  )
endif()
