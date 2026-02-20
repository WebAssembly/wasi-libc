include(FetchContent)
include(ExternalProject)
include(CTest)
enable_testing()

option(PYTHON_TESTS "Build Python with this wasi-libc and run its tests" OFF)

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

# ======== Clone Open POSIX Test Suite ==========================

FetchContent_Declare(
  posix-testsuite
  GIT_REPOSITORY https://github.com/tartanllama/open-posix-test-suite
  GIT_TAG 78b3bc8386ef3d933c7f41c95c89cd80e60e60a9
  GIT_SHALLOW true
)
FetchContent_MakeAvailable(posix-testsuite)
set(POSIX_TESTSUITE "${posix-testsuite_SOURCE_DIR}")
message(STATUS "Open POSIX Test Suite source directory: ${POSIX_TESTSUITE}")


# ========= Download wasmtime as a test runner ==================
include(ba-download)

if(NOT ENGINE OR ENGINE STREQUAL "")
  ba_download(
    wasmtime
    "https://github.com/bytecodealliance/wasmtime"
    "v40.0.0"
  )
  ExternalProject_Get_Property(wasmtime SOURCE_DIR)
  set(ENGINE "${SOURCE_DIR}/wasmtime")
  message(STATUS "Wasmtime executable: ${ENGINE}")
  add_custom_target(engine ALL DEPENDS wasmtime)
else()
  add_custom_target(engine)
endif()

# ========= libc-test defined tests =============================

function(add_wasilibc_flags target)
  add_dependencies(${target} sysroot builtins)
  # Using an `INTERFACE` looks to get what we want here which is to rebuild this
  # if the library changes but not actually add any arguments to the link-line.
  target_link_libraries(${target} INTERFACE c-static)

  if (TARGET_TRIPLE MATCHES "-threads")
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
  clang_format_target(${executable_name})
  add_wasilibc_flags(${executable_name})
  target_link_libraries(${executable_name} PRIVATE libc_test_support)
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
# * `FAILP3` - this test fails on wasip3 targets
function(register_test test_name executable_name)
  set(options FS NETWORK FAILP3)
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
    list(APPEND wasmtime_args --wasi threads --wasm shared-memory)
  endif()
  if (WASI STREQUAL "p3")
    list(APPEND wasmtime_args --wasm component-model-async)
    list(APPEND wasmtime_args --wasi p3)
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

  if (arg_FAILP3 AND (WASI STREQUAL "p3"))
    set_tests_properties(${test_name} PROPERTIES WILL_FAIL TRUE)
  endif()

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
if (MALLOC STREQUAL "emmalloc")
  set_tests_properties(libc_test_functional_search_hsearch.wasm PROPERTIES WILL_FAIL TRUE)
endif()
add_libc_test(functional/search_insque.c)
add_libc_test(functional/search_lsearch.c)
add_libc_test(functional/search_tsearch.c)
add_libc_test(functional/snprintf.c)
add_libc_test(functional/sscanf.c CFLAGS -Wno-literal-range)
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
  add_wasilibc_test(access.c FS FAILP3)
endif()
add_wasilibc_test(append.c FS FAILP3)
add_wasilibc_test(argv_two_args.c ARGV foo bar)
add_wasilibc_test(clock_nanosleep.c FAILP3)
add_wasilibc_test(chdir.c FS FAILP3)
add_wasilibc_test(close.c FS FAILP3)
add_wasilibc_test(external_env.c ENV VAR1=foo VAR2=bar)
add_wasilibc_test(fadvise.c FS FAILP3)
add_wasilibc_test(fallocate.c FS FAILP3)
add_wasilibc_test(fcntl.c FS FAILP3)
add_wasilibc_test(fdatasync.c FS FAILP3)
add_wasilibc_test(fdopen.c FS FAILP3)
add_wasilibc_test(feof.c FS FAILP3)
add_wasilibc_test(file_permissions.c FS FAILP3)
add_wasilibc_test(file_nonblocking.c FS FAILP3)
add_wasilibc_test(fseek.c FS FAILP3)
add_wasilibc_test(fstat.c FS FAILP3)
add_wasilibc_test(fsync.c FS FAILP3)
add_wasilibc_test(ftruncate.c FS FAILP3)
add_wasilibc_test(fts.c FS FAILP3)
add_wasilibc_test(fwscanf.c FS FAILP3)
add_wasilibc_test(getentropy.c)
add_wasilibc_test(hello.c PASS_REGULAR_EXPRESSION "Hello, World!" FAILP3)
add_wasilibc_test(ioctl.c FS FAILP3)
add_wasilibc_test(isatty.c FS FAILP3)
add_wasilibc_test(link.c FS FAILP3)
add_wasilibc_test(lseek.c FS FAILP3)
add_wasilibc_test(memchr.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(memcmp.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(opendir.c FS FAILP3 ARGV /)
add_wasilibc_test(open_relative_path.c FS FAILP3 ARGV /)
add_wasilibc_test(poll.c FS FAILP3)
add_wasilibc_test(preadvwritev.c FS FAILP3)
add_wasilibc_test(preadwrite.c FS FAILP3)
add_wasilibc_test(readlink.c FS FAILP3)
add_wasilibc_test(readv.c FS FAILP3)
add_wasilibc_test(rename.c FS FAILP3)
add_wasilibc_test(rmdir.c FS FAILP3)
add_wasilibc_test(scandir.c FS FAILP3)
add_wasilibc_test(stat.c FS FAILP3)
add_wasilibc_test(stdio.c FS FAILP3)
add_wasilibc_test(strchrnul.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(strlen.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(strptime.c)
add_wasilibc_test(strrchr.c LDFLAGS -Wl,--stack-first -Wl,--initial-memory=327680)
add_wasilibc_test(time_and_times.c
  CFLAGS -D_WASI_EMULATED_PROCESS_CLOCKS
  LDFLAGS -lwasi-emulated-process-clocks)
add_wasilibc_test(time.c)
add_wasilibc_test(utime.c FS FAILP3)
add_wasilibc_test(rewinddir.c FS FAILP3)
add_wasilibc_test(seekdir.c FS FAILP3)
add_wasilibc_test(usleep.c FAILP3)
add_wasilibc_test(nanosleep.c FAILP3)
add_wasilibc_test(write.c FS FAILP3)

if (TARGET_TRIPLE MATCHES "-threads")
  add_wasilibc_test(busywait.c)
  add_wasilibc_test(pthread_cond_busywait.c)
  add_wasilibc_test(pthread_tsd_busywait.c)
  add_wasilibc_test(pthread_mutex_busywait.c)
endif()

# ========= sockets-related tests ===============================

if (NOT (WASI STREQUAL "p1"))
  add_wasilibc_test(poll-nonblocking-socket.c NETWORK FAILP3)
  add_wasilibc_test(setsockopt.c NETWORK FAILP3)
  add_wasilibc_test(sockets-nonblocking-udp.c NETWORK FAILP3)
  add_wasilibc_test(sockets-nonblocking-multiple.c NETWORK FAILP3)
  add_wasilibc_test(sockets-nonblocking-udp-multiple.c NETWORK FAILP3)

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
    set(options FAILP3)
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

    if (arg_FAILP3 AND (WASI STREQUAL "p3"))
      set_tests_properties(${test_name} PROPERTIES WILL_FAIL TRUE)
    endif()
  endfunction()

  sockets_test(sockets sockets-client.wasm sockets-server.wasm FAILP3)
  sockets_test(sockets-udp-blocking sockets-client-udp-blocking.wasm
    sockets-server-udp-blocking.wasm FAILP3)
  sockets_test(sockets-multiple sockets-multiple-client.wasm sockets-multiple-server.wasm
    NCLIENTS 10 FAILP3)

  # Various forms of client hangups
  sockets_test(sockets-client-hangup-after-connect
    sockets-client-hangup-after-connect.wasm sockets-server-handle-hangups.wasm
    FAILP3)
  sockets_test(sockets-client-hangup-while-sending
    sockets-client-hangup-while-sending.wasm sockets-server-handle-hangups.wasm
    FAILP3)
  sockets_test(sockets-client-hangup-after-sending
    sockets-client-hangup-after-sending.wasm sockets-server-handle-hangups.wasm
    FAILP3)
  sockets_test(sockets-client-hangup-while-receiving
    sockets-client-hangup-while-receiving.wasm sockets-server-handle-hangups.wasm
    FAILP3)

  # Various forms of server hangups, including when there's no server at all
  sockets_test(sockets-server-hangup-before-send
    sockets-client-handle-hangups.wasm sockets-server-hangup-before-send.wasm
    FAILP3)
  sockets_test(sockets-server-hangup-during-send
    sockets-client-handle-hangups.wasm sockets-server-hangup-during-send.wasm
    FAILP3)
  sockets_test(sockets-server-hangup-before-recv
    sockets-client-handle-hangups.wasm sockets-server-hangup-before-recv.wasm
    FAILP3)
  sockets_test(sockets-server-hangup-during-recv
    sockets-client-handle-hangups.wasm sockets-server-hangup-during-recv.wasm
    FAILP3)
  sockets_test(sockets-client-handle-hangups
    sockets-client-handle-hangups.wasm hello.wasm
    FAILP3)
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


# ========= Open POSIX Test Suite tests ========================

function(add_posix_test test_file)
  cmake_path(REPLACE_EXTENSION test_file wasm OUTPUT_VARIABLE test_name)
  string(REPLACE "/" "_" test_name ${test_name})
  set(test_name "open_posix_test_${test_name}")
  set(test_file "${POSIX_TESTSUITE}/${test_file}")

  add_test_executable(${test_name} "${test_file}" ${ARGN} LDFLAGS -Wl,--export-table)
  register_test(${test_name} ${test_name} ${ARGN})
  target_include_directories(${test_name} PRIVATE ${POSIX_TESTSUITE}/include)
endfunction()

if (WASI STREQUAL "p3")
  # unsupported(fork): conformance/interfaces/pthread_atfork/*
  
  add_posix_test(conformance/interfaces/pthread_attr_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_destroy/2-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_destroy/3-1.c)
  
  add_posix_test(conformance/interfaces/pthread_attr_getdetachstate/1-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_getdetachstate/1-2.c)
  
  # unsupported(scheduling): conformance/interfaces/pthread_attr_getinheritsched/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_getschedparam/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_getschedpolicy/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_getscope/*
  
  add_posix_test(conformance/interfaces/pthread_attr_getstack/1-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_getstacksize/1-1.c)
  
  add_posix_test(conformance/interfaces/pthread_attr_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_init/2-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_init/3-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_init/4-1.c)
  
  add_posix_test(conformance/interfaces/pthread_attr_setdetachstate/1-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_setdetachstate/1-2.c)
  add_posix_test(conformance/interfaces/pthread_attr_setdetachstate/2-1.c)
  add_posix_test(conformance/interfaces/pthread_attr_setdetachstate/4-1.c)
  
  # unsupported(scheduling): conformance/interfaces/pthread_attr_setinheritsched/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_setschedparam/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_setschedpolicy/*
  # unsupported(scheduling): conformance/interfaces/pthread_attr_setscope/*
  
  add_posix_test(conformance/interfaces/pthread_attr_setstack/1-1.c)
  # bad test(untouched stack): conformance/interfaces/pthread_attr_setstack/2-1.c
  add_posix_test(conformance/interfaces/pthread_attr_setstack/4-1.c)
  # bad test(untouched stack): conformance/interfaces/pthread_attr_setstack/6-1.c
  add_posix_test(conformance/interfaces/pthread_attr_setstack/7-1.c)
  
  add_posix_test(conformance/interfaces/pthread_attr_setstacksize/1-1.c)
  # bad test(untouched stack): conformance/interfaces/pthread_attr_setstacksize/2-1.c
  # bad test(stack/page size): conformance/interfaces/pthread_attr_setstacksize/4-1.c
  
  add_posix_test(conformance/interfaces/pthread_barrier_destroy/1-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_barrier_destroy/2-1.c
  add_posix_test(conformance/interfaces/pthread_barrier_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_barrier_init/3-1.c)
  # bad test(undefined behaviour): conformance/interfaces/pthread_barrier_init/4-1.c
  add_posix_test(conformance/interfaces/pthread_barrier_wait/1-1.c)
  add_posix_test(conformance/interfaces/pthread_barrier_wait/2-1.c)
  # unsupported(signals): conformance/interfaces/pthread_barrier_wait/3-1.c
  # unsupported(signals): conformance/interfaces/pthread_barrier_wait/3-2.c
  # bad test(undefined behaviour): conformance/interfaces/pthread_barrier_wait/6-1.c

  # unsupported(process-shared): conformance/interfaces/pthread_barrierattr_destroy/*
  # unsupported(process-shared): conformance/interfaces/pthread_barrierattr_getpshared/*
  # unsupported(process-shared): conformance/interfaces/pthread_barrierattr_init/*
  # unsupported(process-shared): conformance/interfaces/pthread_barrierattr_setpshared/*
  
  # unsupported(cancellation): conformance/interfaces/pthread_cancel/*
  
  # temporarily unsupported(clocks,pthread_exit): conformance/interfaces/pthread_cleanup_pop/1-1.c
  # temporarily unsupported(clocks,pthread_exit): conformance/interfaces/pthread_cleanup_pop/1-2.c
  # temporarily unsupported(pthread_exit): conformance/interfaces/pthread_cleanup_pop/1-3.c
  
  # temporarily unsupported(pthread_exit): conformance/interfaces/pthread_cleanup_push/1-1.c
  # unsupported(cancellation): conformance/interfaces/pthread_cleanup_push/1-2.c
  # unsupported(cancellation): conformance/interfaces/pthread_cleanup_push/1-3.c
  
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/1-1.c)
  # temporarily unsupported(clocks): conformance/interfaces/pthread_cond_broadcast/1-2.c
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/2-1.c)
  # temporarily unsupported(timed waits): conformance/interfaces/pthread_cond_broadcast/2-2.c
  # unsupported(mmap): conformance/interfaces/pthread_cond_broadcast/2-3.c
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/4-1.c)
  # unsupported(signals): conformance/interfaces/pthread_cond_broadcast/4-2.c
  
  add_posix_test(conformance/interfaces/pthread_cond_destroy/1-1.c)
  # temporarily unsupported(clocks): conformance/interfaces/pthread_cond_destroy/2-1.c
  add_posix_test(conformance/interfaces/pthread_cond_destroy/3-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_destroy/speculative/4-1.c)
  
  add_posix_test(conformance/interfaces/pthread_cond_init/1-1.c)
  # unsupported(set time): conformance/interfaces/pthread_cond_init/1-2.c
  # unsupported(processes): conformance/interfaces/pthread_cond_init/1-3.c
  add_posix_test(conformance/interfaces/pthread_cond_init/2-1.c)
  # temporarily unsupported(clocks): conformance/interfaces/pthread_cond_init/2-2.c
  add_posix_test(conformance/interfaces/pthread_cond_init/3-1.c)
  # unsupported(fork): conformance/interfaces/pthread_cond_init/4-1.c
  # unsupported(signals): conformance/interfaces/pthread_cond_init/4-2.c

  add_posix_test(conformance/interfaces/pthread_cond_signal/1-1.c)
  # temporarily unsupported(clocks): conformance/interfaces/pthread_cond_signal/1-2.c
  add_posix_test(conformance/interfaces/pthread_cond_signal/2-1.c)
  # temporarily unsupported(timed waits): conformance/interfaces/pthread_cond_signal/2-2.c
  add_posix_test(conformance/interfaces/pthread_cond_signal/4-1.c)
  # unsupported(signals): conformance/interfaces/pthread_cond_signal/4-2.c
  
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/1-1.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-1.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-2.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-3.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-4.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-5.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-6.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/2-7.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/3-1.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/4-1.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/4-2.c
  # temporarily unsupported(timed wait): conformance/interfaces/pthread_cond_timedwait/4-3.c
  
  add_posix_test(conformance/interfaces/pthread_cond_wait/1-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_wait/2-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_wait/2-2.c)
  # unsupported(cancellation): conformance/interfaces/pthread_cond_wait/2-3.c
  # test needs modified(alarm waking): conformance/interfaces/pthread_cond_wait/3-1.c
  # test needs modified(alarm waking): conformance/interfaces/pthread_cond_wait/4-1.c
  
  add_posix_test(conformance/interfaces/pthread_condattr_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_destroy/2-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_destroy/3-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_destroy/4-1.c)
  
  add_posix_test(conformance/interfaces/pthread_condattr_getclock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_getclock/1-2.c)
  
  # unsupported(process-shared): conformance/interfaces/pthread_condattr_getpshared/*

  add_posix_test(conformance/interfaces/pthread_condattr_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_init/3-1.c)
  
  # temporarily unsupported(clocks): conformance/interfaces/pthread_condattr_setclock/1-1.c
  # temporarily unsupported(clocks): conformance/interfaces/pthread_condattr_setclock/1-2.c
  # unsupported(getcpuclockid): conformance/interfaces/pthread_condattr_setclock/1-3.c
  # temporarily unsupported(clocks): conformance/interfaces/pthread_condattr_setclock/2-1.c
  
  # unsupported(process-shared): conformance/interfaces/pthread_condattr_setpshared/*
  
  add_posix_test(conformance/interfaces/pthread_create/1-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_create/1-2.c
  # unsupported(signals): conformance/interfaces/pthread_create/1-3.c
  # unsupported(processes): conformance/interfaces/pthread_create/1-4.c
  # unsupported(fork): add_posix_test(conformance/interfaces/pthread_create/1-5.c
  # unsupported(scheduling): conformance/interfaces/pthread_create/1-6.c
  # unsupported(signals): add_posix_test(conformance/interfaces/pthread_create/10-1.c)
  # temporarily unsupported(clocks): conformance/interfaces/pthread_create/11-1.c
  add_posix_test(conformance/interfaces/pthread_create/12-1.c)
  # unsupported(signals): conformance/interfaces/pthread_create/14-1.c
  add_posix_test(conformance/interfaces/pthread_create/15-1.c)
  add_posix_test(conformance/interfaces/pthread_create/2-1.c)
  add_posix_test(conformance/interfaces/pthread_create/3-1.c)
  # unsupported(fork): conformance/interfaces/pthread_create/3-2.c
  add_posix_test(conformance/interfaces/pthread_create/4-1.c)
  add_posix_test(conformance/interfaces/pthread_create/5-1.c)
  add_posix_test(conformance/interfaces/pthread_create/5-2.c)
  # unsupported(signals): conformance/interfaces/pthread_create/8-1.c
  # unsupported(signals): conformance/interfaces/pthread_create/8-2.c
  
  add_posix_test(conformance/interfaces/pthread_detach/1-1.c)
  add_posix_test(conformance/interfaces/pthread_detach/1-2.c)
  add_posix_test(conformance/interfaces/pthread_detach/2-1.c)
  add_posix_test(conformance/interfaces/pthread_detach/2-2.c)
  add_posix_test(conformance/interfaces/pthread_detach/3-1.c)
  add_posix_test(conformance/interfaces/pthread_detach/4-1.c)
  # bad test(invalid assumption): conformance/interfaces/pthread_detach/4-2.c
  # unsupported(signals): conformance/interfaces/pthread_detach/4-3.c
  
  add_posix_test(conformance/interfaces/pthread_equal/1-1.c)
  add_posix_test(conformance/interfaces/pthread_equal/1-2.c)
  # unsupported(signals): conformance/interfaces/pthread_equal/2-1.c
  
  # temporarily unsupported(pthread_exit) conformance/interfaces/pthread_exit/*
  # unsupported(fork): conformance/interfaces/pthread_exit/6-1.c
  
  # unsupported(getcpuclockid): conformance/interfaces/pthread_getcpuclockid/*
  # unsupported(scheduling): conformance/interfaces/pthread_getschedparam/*
  
  add_posix_test(conformance/interfaces/pthread_getspecific/1-1.c)
  add_posix_test(conformance/interfaces/pthread_getspecific/3-1.c)
  
  add_posix_test(conformance/interfaces/pthread_join/1-1.c)
  add_posix_test(conformance/interfaces/pthread_join/1-2.c)
  add_posix_test(conformance/interfaces/pthread_join/2-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_join/3-1.c
  # unsupported(cancellation): conformance/interfaces/pthread_join/4-1.c
  add_posix_test(conformance/interfaces/pthread_join/5-1.c)
  # bad test(undefined behaviour): conformance/interfaces/pthread_join/6-2.c
  # unsupported(signals): conformance/interfaces/pthread_join/6-3.c
  add_posix_test(conformance/interfaces/pthread_join/speculative/6-1.c)
  
  add_posix_test(conformance/interfaces/pthread_key_create/1-1.c)
  add_posix_test(conformance/interfaces/pthread_key_create/1-2.c)
  add_posix_test(conformance/interfaces/pthread_key_create/2-1.c)
  add_posix_test(conformance/interfaces/pthread_key_create/3-1.c)
  add_posix_test(conformance/interfaces/pthread_key_create/speculative/5-1.c)
  
  add_posix_test(conformance/interfaces/pthread_key_delete/1-1.c)
  add_posix_test(conformance/interfaces/pthread_key_delete/1-2.c)
  add_posix_test(conformance/interfaces/pthread_key_delete/2-1.c)
  
  # unsupported(signals): conformance/interfaces/pthread_kill/*
  
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/2-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/2-2.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/5-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/5-2.c)
  add_posix_test(conformance/interfaces/pthread_mutex_destroy/speculative/4-2.c)
  
  # unsupported(scheduling): conformance/interfaces/pthread_mutex_getprioceiling/*
  
  add_posix_test(conformance/interfaces/pthread_mutex_init/1-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_mutex_init/1-2.c
  add_posix_test(conformance/interfaces/pthread_mutex_init/2-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_init/3-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_mutex_init/3-2.c
  add_posix_test(conformance/interfaces/pthread_mutex_init/4-1.c)
  # unsupported(fork): add_posix_test(conformance/interfaces/pthread_mutex_init/5-1.c
  # unsupported(signals): conformance/interfaces/pthread_mutex_init/5-3.c
  # unsupported(privileges): conformance/interfaces/pthread_mutex_init/speculative/5-2.c
  
  add_posix_test(conformance/interfaces/pthread_mutex_lock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_lock/2-1.c)
  # unsupported(signals): add_posix_test(conformance/interfaces/pthread_mutex_lock/3-1.c
  add_posix_test(conformance/interfaces/pthread_mutex_lock/4-1.c)
  # unsupported(signals): add_posix_test(conformance/interfaces/pthread_mutex_lock/5-1.c
  
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/1-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/2-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/4-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/5-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/5-2.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_mutex_timedlock/5-3.c
  
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/1-2.c)
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/2-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/4-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_trylock/4-2.c)
  # unsupported(signals): conformance/interfaces/pthread_mutex_trylock/4-3.c
  
  add_posix_test(conformance/interfaces/pthread_mutex_unlock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_unlock/2-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_unlock/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_unlock/5-1.c)
  add_posix_test(conformance/interfaces/pthread_mutex_unlock/5-2.c)
  
  add_posix_test(conformance/interfaces/pthread_mutexattr_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_destroy/2-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_destroy/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_destroy/4-1.c)
  
  # unsupported(scheduling): conformance/interfaces/pthread_mutexattr_getprioceiling/*
  
  add_posix_test(conformance/interfaces/pthread_mutexattr_getprotocol/1-1.c)
  # unsupported(scheduling): conformance/interfaces/pthread_mutexattr_getprotocol/1-2.c
  
  # unsupported(process-shared): conformance/interfaces/pthread_mutexattr_getpshared/*
  
  add_posix_test(conformance/interfaces/pthread_mutexattr_gettype/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_gettype/1-2.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_gettype/1-3.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_gettype/1-4.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_gettype/1-5.c)
  # bad test(assumes layout): conformance/interfaces/pthread_mutexattr_gettype/speculative/3-1.c
  
  add_posix_test(conformance/interfaces/pthread_mutexattr_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_init/3-1.c)
  
  # unsupported(scheduling): conformance/interfaces/pthread_mutexattr_setprioceiling/*
  
  # unsupported(scheduling): conformance/interfaces/pthread_mutexattr_setprotocol/1-1.c
  add_posix_test(conformance/interfaces/pthread_mutexattr_setprotocol/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_setprotocol/3-2.c)
  
  # unsupported(process-shared): conformance/interfaces/pthread_mutexattr_setpshared/*
  
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/1-1.c)
  # bad test(undefined behaviour): conformance/interfaces/pthread_mutexattr_settype/2-1.c
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/3-1.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/3-2.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/3-3.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/3-4.c)
  add_posix_test(conformance/interfaces/pthread_mutexattr_settype/7-1.c)
  
  add_posix_test(conformance/interfaces/pthread_once/1-1.c)
  add_posix_test(conformance/interfaces/pthread_once/1-2.c)
  add_posix_test(conformance/interfaces/pthread_once/1-3.c)
  add_posix_test(conformance/interfaces/pthread_once/2-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_once/3-1.c
  add_posix_test(conformance/interfaces/pthread_once/4-1.c)
  # unsupported(signals): conformance/interfaces/pthread_once/6-1.c
  
  add_posix_test(conformance/interfaces/pthread_rwlock_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_destroy/3-1.c)
  
  add_posix_test(conformance/interfaces/pthread_rwlock_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_init/2-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_init/3-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_init/6-1.c)
  
  add_posix_test(conformance/interfaces/pthread_rwlock_rdlock/1-1.c)
  # unsupported(scheduling): conformance/interfaces/pthread_rwlock_rdlock/2-1.c
  # unsupported(scheduling): conformance/interfaces/pthread_rwlock_rdlock/2-2.c
  # unsupported(scheduling): conformance/interfaces/pthread_rwlock_rdlock/2-3.c
  # unsupported(signals): conformance/interfaces/pthread_rwlock_rdlock/4-1.c
  add_posix_test(conformance/interfaces/pthread_rwlock_rdlock/5-1.c)
  
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/1-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/2-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/3-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/5-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/6-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedrdlock/6-2.c
  
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/1-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/2-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/3-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/5-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/6-1.c
  # temporarily unsupported(timed lock): conformance/interfaces/pthread_rwlock_timedwrlock/6-2.c
  
  add_posix_test(conformance/interfaces/pthread_rwlock_tryrdlock/1-1.c)
  
  add_posix_test(conformance/interfaces/pthread_rwlock_trywrlock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_trywrlock/speculative/3-1.c)
  
  add_posix_test(conformance/interfaces/pthread_rwlock_unlock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_unlock/2-1.c)
  # unsupported(scheduling): conformance/interfaces/pthread_rwlock_unlock/3-1.c
  # bad test(undefined behaviour): conformance/interfaces/pthread_rwlock_unlock/4-1.c
  # bad test(undefined behaviour): conformance/interfaces/pthread_rwlock_unlock/4-2.c
  
  add_posix_test(conformance/interfaces/pthread_rwlock_wrlock/1-1.c)
  # unsupported(signals): conformance/interfaces/pthread_rwlock_wrlock/2-1.c
  add_posix_test(conformance/interfaces/pthread_rwlock_wrlock/3-1.c)
  
  add_posix_test(conformance/interfaces/pthread_rwlockattr_destroy/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlockattr_destroy/2-1.c)
  
  # unsupported(process-shared): conformance/interfaces/pthread_rwlockattr_getpshared/*
  
  add_posix_test(conformance/interfaces/pthread_rwlockattr_init/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlockattr_init/2-1.c)
  
  # unsupported(process-shared): conformance/interfaces/pthread_rwlockattr_setpshared/*
  
  add_posix_test(conformance/interfaces/pthread_self/1-1.c)
  
  # unsupported(cancellation): conformance/interfaces/pthread_setcancelstate/*
  
  # unsupported(cancellation): conformance/interfaces/pthread_setcanceltype/*
  
  # unsupported(scheduling): conformance/interfaces/pthread_setschedparam/*
  # unsupported(scheduling): conformance/interfaces/pthread_setschedprio/*
  
  add_posix_test(conformance/interfaces/pthread_setspecific/1-1.c)
  add_posix_test(conformance/interfaces/pthread_setspecific/1-2.c)
  
  # unsupported(signals): conformance/interfaces/pthread_sigmask/*
  
  # unsupported(spinlocks): conformance/interfaces/pthread_spin_destroy/*
  # unsupported(spinlocks): conformance/interfaces/pthread_spin_init/*
  # unsupported(spinlocks): conformance/interfaces/pthread_spin_lock/*
  # unsupported(spinlocks): conformance/interfaces/pthread_spin_trylock/*
  # unsupported(spinlocks): conformance/interfaces/pthread_spin_unlock/*
  
  # unsupported(cancellation): conformance/interfaces/pthread_testcancel/*
endif()

# If enabled add a copy of Python which is built against `wasi-libc` and run
# its tests.
if (PYTHON_TESTS)
  find_program(PYTHON python3 python REQUIRED)
  find_program(MAKE make REQUIRED)

  set(flags "--target=${TARGET_TRIPLE} --sysroot=${SYSROOT}")

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
  ExternalProject_Add_StepDependencies(python configure sysroot)
endif()
