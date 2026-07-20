include(ExternalProject)
include(CTest)
enable_testing()

option(PYTHON_TESTS "Build Python with this wasi-libc and run its tests" OFF)

# ========= Download libc-test ==================================

set(LIBC_TEST_COMMIT 18e28496adee3d84fefdda6efcb9c5b8996a2398)
ExternalProject_Add(
  libc-test
  EXCLUDE_FROM_ALL ON
  URL "https://github.com/bytecodealliance/libc-test/archive/${LIBC_TEST_COMMIT}.tar.gz"
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(libc-test SOURCE_DIR)
set(LIBC_TEST "${SOURCE_DIR}")
message(STATUS "libc-test source directory: ${LIBC_TEST}")

# ======== Clone Open POSIX Test Suite ==========================

set(OPEN_POSIX_TEST_SUITE_COMMIT dc7c7d54fc4f6219bb4c7af348be2290335bf17e)
ExternalProject_Add(
  posix-testsuite
  EXCLUDE_FROM_ALL ON
  URL "https://github.com/bytecodealliance/open-posix-test-suite/archive/${OPEN_POSIX_TEST_SUITE_COMMIT}.tar.gz"
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(posix-testsuite SOURCE_DIR)
set(POSIX_TESTSUITE "${SOURCE_DIR}")
message(STATUS "Open POSIX Test Suite source directory: ${POSIX_TESTSUITE}")


# ========= Download a test runner =============================
include(ba-download)

option(TEST_WITH_V8 "Run tests in headless Chromium (V8) via Playwright instead of Wasmtime" OFF)

if(TEST_WITH_V8)
  find_program(NPM_EXECUTABLE npm REQUIRED)
  find_program(NPX_EXECUTABLE npx REQUIRED)

  set(browser_test_src "${CMAKE_CURRENT_SOURCE_DIR}/scripts/browser-test")
  set(browser_test_dir "${CMAKE_CURRENT_BINARY_DIR}/browser-test")
  set(browser_test_stamp "${browser_test_dir}/deps.stamp")

  set(browser_test_inputs
    "${browser_test_src}/package.json"
    "${browser_test_src}/package-lock.json"
  )

  add_custom_command(
    OUTPUT ${browser_test_stamp}
    COMMAND ${NPM_EXECUTABLE} -C ${browser_test_src} install
    COMMAND ${NPX_EXECUTABLE} -C ${browser_test_src} playwright install chromium-headless-shell
    COMMAND ${CMAKE_COMMAND} -E touch ${browser_test_stamp}
    DEPENDS ${browser_test_inputs}
  )

  set(ENGINE "${browser_test_src}/harness.mjs")
  add_custom_target(engine ALL DEPENDS ${browser_test_stamp})
  message(STATUS "Test engine: V8 (headless browser) via ${ENGINE}")

elseif(NOT ENGINE OR ENGINE STREQUAL "")
  # ====== Download Wasmtime as a test runner ==================
  set(wasmtime_version dev)
  if (TARGET_TRIPLE MATCHES "-threads")
    set(wasmtime_version "v44.0.0")
  endif()
  ba_download(
    wasmtime
    "https://github.com/bytecodealliance/wasmtime"
    ${wasmtime_version}
  )
  ExternalProject_Get_Property(wasmtime SOURCE_DIR)
  set(ENGINE "${SOURCE_DIR}/wasmtime")
  message(STATUS "Wasmtime executable: ${ENGINE}")
  add_custom_target(engine ALL DEPENDS wasmtime)
else()
  add_custom_target(engine)
endif()

# Helper to completely disable tests which currently fail in v8.
function(expect_fail_under_v8 test_name)
  if(TEST_WITH_V8)
    set_tests_properties(${test_name} PROPERTIES WILL_FAIL ON)
  endif()
endfunction()

# All tests are compiled against the wasi-libc sysroot
add_compile_options(--sysroot ${SYSROOT})
add_link_options(--sysroot ${SYSROOT})

# ========= Helper functions for tests ==========================
#
# These are a set of helper functions and helper variables and such which
# are used to build test executables appropriately with various flags and
# to then run the test with appropriate arguments.

set(libc_test_support_files
  "${LIBC_TEST}/src/common/path.c"
  "${LIBC_TEST}/src/common/print.c"
  "${LIBC_TEST}/src/common/rand.c"
  "${LIBC_TEST}/src/common/utf8.c"
)
set_source_files_properties(${libc_test_support_files} PROPERTIES GENERATED TRUE)
set_source_files_properties("${LIBC_TEST}/src/common/rand.c"
  PROPERTIES COMPILE_OPTIONS -Wno-unused-function)
set_source_files_properties("${LIBC_TEST}/src/common/print.c"
  PROPERTIES COMPILE_OPTIONS -Wno-sign-compare)

# Adds a new test executable to build
#
# * `executable_name` must be a unique name and valid cmake target name.
# * `src` is the path to the test file to compile.
#
# Optional arguments:
#
# * `CFLAGS -a -b -c` - additional flags to pass to the compiler
# * `LDFLAGS -a -b -c` - additional flags to pass to the linker
# * `SHARED` - link to the shared version of libc_test_support
# * `SHARED_LIBS` - extra libs to pass to `wasm-tools component link`
function(add_test_executable executable_name src)
  set(options SHARED)
  set(oneValueArgs)
  set(multiValueArgs LDFLAGS CFLAGS SHARED_LIBS INCLUDE DEPENDENCIES)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  # Build the test exeutable itself and apply all custom options as applicable.
  add_executable(${executable_name} ${src} ${libc_test_support_files})
  target_include_directories(${executable_name} PRIVATE "${LIBC_TEST}/src/common")
  add_dependencies(${executable_name} sysroot builtins libc-test)

  if (TARGET_TRIPLE MATCHES "-threads")
    target_link_options(${executable_name} PRIVATE -pthread
      -Wl,--import-memory,--export-memory,--shared-memory,--max-memory=1073741824)
  endif()

  if (ENABLE_COOP_THREADS)
    # FIXME(#808): --export-table here shouldn't be necessary
    target_link_options(${executable_name} PRIVATE -pthread -Wl,--export-table)
  endif()

  if(arg_SHARED)
    set_pic(${executable_name})
    # Skip wit-component when linking to manually run `wasm-tools component
    # link` below. Additionally use `-shared` to wasm-ld, but notably not clang,
    # to get clang to work with this as an executable but get `wasm-ld` to
    # emit shared library imports.
    #
    # Note that `-fvisibility=default` is used to make the generated
    # `__main_void` symbol from clang visible to wasi-libc itself.
    target_link_options(${executable_name} PRIVATE -Wl,--skip-wit-component,-shared)
    target_compile_options(${executable_name} PRIVATE -fvisibility=default)
    add_custom_command(
      TARGET ${executable_name}
      POST_BUILD
      COMMAND
        ${wasm_tools} component link
          $<TARGET_FILE:${executable_name}>
          "${SYSROOT_LIB}/libc.so"
          ${arg_SHARED_LIBS}
          -o ${executable_name}
    )
    add_dependencies(${executable_name} wasm-tools)
    target_link_libraries(${executable_name} PRIVATE c)
  else()
    clang_format_target(${executable_name})
    target_link_libraries(${executable_name} PRIVATE c-static)
  endif()
  foreach(flag IN LISTS arg_CFLAGS)
    target_compile_options(${executable_name} PRIVATE ${flag})
  endforeach()
  foreach(dir IN LISTS arg_INCLUDE)
    target_include_directories(${executable_name} PRIVATE ${dir})
  endforeach()
  foreach(flag IN LISTS arg_LDFLAGS)
    target_link_options(${executable_name} PRIVATE ${flag})
  endforeach()
  foreach(dep IN LISTS arg_DEPENDENCIES)
    add_dependencies(${executable_name} ${dep})
  endforeach()
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
# * `SETJMP` - this test requires setjmp/longjmp
function(register_test test_name executable_name)
  set(options FS NETWORK SETJMP)
  set(oneValueArgs CLIENT PASS_REGULAR_EXPRESSION)
  set(multiValueArgs ARGV ENV ENGINE_ARG)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  set(wasmtime_args)

  if (arg_FS)
    set(fsdir "${CMAKE_CURRENT_BINARY_DIR}/tmp/${test_name}/fs")
    list(APPEND wasmtime_args --dir ${fsdir}::/)
  endif()
  if (arg_NETWORK)
    list(APPEND wasmtime_args -Sinherit-network,allow-ip-name-lookup)
  endif()
  foreach(env IN LISTS arg_ENV)
    list(APPEND wasmtime_args --env ${env})
  endforeach()
  foreach(arg IN LISTS arg_ENGINE_ARG)
    list(APPEND wasmtime_args ${arg})
  endforeach()
  if (TARGET_TRIPLE MATCHES "-threads")
    list(APPEND wasmtime_args --wasi threads --wasm shared-memory)
  endif()
  if (ENABLE_COOP_THREADS)
    list(APPEND wasmtime_args --wasm component-model-threading)
  endif()
  if (arg_SETJMP)
    list(APPEND wasmtime_args --wasm exceptions)
    target_compile_options(${executable_name} PRIVATE
      "SHELL:-mllvm -wasm-enable-sjlj"
      "SHELL:-mllvm -wasm-use-legacy-eh=false"
    )
    target_link_options(${executable_name} PRIVATE
      -lsetjmp
      $<$<BOOL:LTO>:-Wl,-mllvm=-wasm-enable-sjlj>
      $<$<BOOL:LTO>:-Wl,-mllvm=-wasm-use-legacy-eh=false>
    )
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

  if (arg_PASS_REGULAR_EXPRESSION)
    set_tests_properties(${test_name} PROPERTIES PASS_REGULAR_EXPRESSION "${arg_PASS_REGULAR_EXPRESSION}")
  endif()
  set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)


  add_dependencies(${test_name} engine)
endfunction()

# Helper function to add two versions of a test: a statically linked version
# and a dynamically linked version.
#
# This will automatically disable the dynamically linked version on
# wasip1-based targets or if dynamic libraries aren't built.
#
# This function recognizes a `NOSHARED` option to also disable dynamic linking
# on a per-test basis. All other arguments are forwarded to `add_test_executable`
# and `register_test`.
function(add_test_pair test_name test_file)
  set(options NOSHARED)
  set(oneValueArgs)
  set(multiValueArgs)
  cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "${multiValueArgs}")

  add_test_executable(${test_name} ${test_file} ${ARGN})
  register_test(${test_name} ${test_name} ${ARGN})

  # If shared libraries are enabled, and this test supports shared libraries,
  # then build a version of the test that's linked against `libc.so` and run the
  # test that way. This is skipped on WASIp1 since the format used here requires
  # components, and WASIp3 coop threads testing is the subject of #813
  if(NOT arg_NOSHARED AND BUILD_SHARED AND NOT (WASI MATCHES "p1")
      AND NOT (WASI MATCHES "p3" AND ENABLE_COOP_THREADS))
    add_test_executable("shared_${test_name}" ${test_file} SHARED ${ARGN})
    register_test("shared_${test_name}" "shared_${test_name}" ${ARGN})
  endif()
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
  set_source_files_properties(${test_file} PROPERTIES GENERATED TRUE)

  add_test_pair(${test_name} ${test_file} ${ARGN})
endfunction()

# ========= libc-test defined tests =============================

add_libc_test(functional/argv.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/basename.c)
add_libc_test(functional/clocale_mbfuncs.c CFLAGS -Wno-unused-variable -Wno-sign-compare)
add_libc_test(functional/clock_gettime.c)
add_libc_test(functional/crypt.c)
add_libc_test(functional/dirname.c)
add_libc_test(functional/env.c)
add_libc_test(functional/fnmatch.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/iconv_open.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/mbc.c)
add_libc_test(functional/memstream.c)
add_libc_test(functional/qsort.c)
add_libc_test(functional/random.c)
add_libc_test(functional/search_hsearch.c)
if (MALLOC STREQUAL "emmalloc")
  set_tests_properties(libc_test_functional_search_hsearch.wasm PROPERTIES WILL_FAIL TRUE)
endif()
add_libc_test(functional/search_insque.c)
if (LTO STREQUAL "full")
  set_tests_properties(libc_test_functional_search_insque.wasm PROPERTIES WILL_FAIL TRUE)
endif()
add_libc_test(functional/search_lsearch.c)
add_libc_test(functional/search_tsearch.c CFLAGS -Wno-unused-parameter)
add_libc_test(functional/snprintf.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/sscanf.c CFLAGS -Wno-literal-range)
add_libc_test(functional/strftime.c)
add_libc_test(functional/string.c)
add_libc_test(functional/string_memcpy.c)
add_libc_test(functional/string_memmem.c)
add_libc_test(functional/string_memset.c CFLAGS -Wno-unused-variable)
add_libc_test(functional/string_strchr.c)
add_libc_test(functional/string_strcspn.c)
add_libc_test(functional/string_strstr.c)
add_libc_test(functional/strtod.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/strtod_long.c)
add_libc_test(functional/strtod_simple.c)
add_libc_test(functional/strtof.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/strtol.c)
# TODO: the `c-printscan-long-double` file is not in its current form
# compatible with shared libraries. That should be fixed at some point.
add_libc_test(functional/strtold.c NOSHARED CFLAGS -Wno-sign-compare LDFLAGS -lc-printscan-long-double)
add_libc_test(functional/swprintf.c CFLAGS -Wno-sign-compare)
add_libc_test(functional/tgmath.c)
add_libc_test(functional/udiv.c CFLAGS -Wno-missing-braces -Wno-sign-compare)
add_libc_test(functional/wcsstr.c)
add_libc_test(functional/wcstol.c)

if (TARGET_TRIPLE MATCHES "-threads")
  add_libc_test(functional/pthread_mutex.c CFLAGS -Wno-unused-variable)
  add_libc_test(functional/pthread_tsd.c)
  add_libc_test(functional/pthread_cond.c)
endif()

# ========= wasi-libc-test defined tests ========================

function(add_wasilibc_test test_file)
  cmake_path(REPLACE_EXTENSION test_file wasm OUTPUT_VARIABLE test_name)
  set(test_file "${CMAKE_CURRENT_SOURCE_DIR}/src/${test_file}")

  add_test_pair(${test_name} ${test_file} ${ARGN})
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
# I'm not really entirely sure why this test is failing, it seemingly exits with
# no output and fails on the very first `open`. Needs more investigation with a
# native windows machine.
if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  set_tests_properties(fadvise.wasm PROPERTIES WILL_FAIL TRUE)
  set_tests_properties(shared_fadvise.wasm PROPERTIES WILL_FAIL TRUE)
endif()
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
# Note that `-Wtimeout` here is passed to force Wasmtime to avoid blocking the
# main thread when doing file I/O. This is used to exercise this test's
# behavior where file streams use fallback code for nonblocking reads/writes.
add_wasilibc_test(fs-nonblocking.c FS ENGINE_ARG -Wtimeout=10s)
expect_fail_under_v8(fs-nonblocking.wasm)
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
add_wasilibc_test(pselect.c FS)
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
  LDFLAGS -lwasi-emulated-process-clocks
  SHARED_LIBS "${SYSROOT_LIB}/libwasi-emulated-process-clocks.so")
add_wasilibc_test(time.c)
add_wasilibc_test(utime.c FS)
add_wasilibc_test(rewinddir.c FS)
add_wasilibc_test(seekdir.c FS)
add_wasilibc_test(usleep.c)
add_wasilibc_test(nanosleep.c)
add_wasilibc_test(sleep.c)
add_wasilibc_test(write.c FS)
add_wasilibc_test(wasi-defines.c)

# This tests the behavior of the WASIp2+ descriptor table and won't succeed on
# p1:
if (NOT (WASI STREQUAL "p1"))
  add_wasilibc_test(clear_fds.c FS)
endif()

if (
  # Setjmp currently requires llvm 20+ for the support necessary in wasi-libc.
  CMAKE_C_COMPILER_VERSION VERSION_GREATER 20.0
  # LTO + LLVM 22 is buggy
  AND (
    (NOT LTO) OR
    (CMAKE_C_COMPILER_VERSION VERSION_LESS 22.0) OR
    (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 23.0)
  )
  # Currently not setup to run this test
  AND NOT TEST_WITH_V8
)
  # LLVM 22-and-prior can't build a shared library with setjmp just yet because
  # `wasm-ld` didn't work well with tags exported from shared libraries.
  # Supported with LLVM 23-and-later
  set(noshared NOSHARED)
  if (CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 23.0)
    set(noshared)
  endif()
  add_wasilibc_test(setjmp.c SETJMP ${noshared})
endif()

# This test works for single-threaded targets too
add_wasilibc_test(pthread_recursive_mutex.c)

if (TARGET_TRIPLE MATCHES "-threads")
  add_wasilibc_test(busywait.c)
  add_wasilibc_test(pthread_cond_busywait.c)
  add_wasilibc_test(pthread_tsd_busywait.c)

  # This test #include's a test whose source is not controlled here, so disable
  # some extra warnings.
  add_wasilibc_test(pthread_mutex_busywait.c)
  target_compile_options(pthread_mutex_busywait.wasm PRIVATE -Wno-unused-variable)

  # These tests #include source files from the `libc-test/src/functional`
  # directory so they need an extra `-I` path.
  target_include_directories(busywait.wasm PRIVATE ${LIBC_TEST})
  target_include_directories(pthread_cond_busywait.wasm PRIVATE ${LIBC_TEST})
  target_include_directories(pthread_tsd_busywait.wasm PRIVATE ${LIBC_TEST})
  target_include_directories(pthread_mutex_busywait.wasm PRIVATE ${LIBC_TEST})
endif()

# ========= cooperative/not threads related tests ================

if (ENABLE_COOP_THREADS)
  add_wasilibc_test(coop_smoke.c)
  add_wasilibc_test(coop_sem_multiwaiter.c)
  add_wasilibc_test(coop_cond_signal_wakeup.c)
  add_wasilibc_test(coop_stdio_orphan_lock.c)
  add_wasilibc_test(coop_barrier_reuse.c)
  add_wasilibc_test(coop_join_uaf.c)
  add_wasilibc_test(coop_timedwaits.c)
  add_wasilibc_test(coop_cond_timedwait_return.c)
  add_wasilibc_test(coop_timedwait_abs_deadline.c)
  add_wasilibc_test(coop_timedwait_past_deadline.c)
  add_wasilibc_test(coop_timedwait_wake_race.c)
  add_wasilibc_test(coop_timedwait_rwlock.c)
  add_wasilibc_test(coop_timedwait_sem_steal.c)
  add_wasilibc_test(coop_timedwait_join.c)
  add_wasilibc_test(coop_timedwait_mixed_waiters.c)
  add_wasilibc_test(coop_timedwait_einval.c)
  add_wasilibc_test(coop_cond_clock_monotonic.c)
endif()

add_wasilibc_test(tls_smoke.c)
if (TARGET_TRIPLE MATCHES "-threads")
  expect_fail_under_v8(tls_smoke.wasm)
endif()
add_wasilibc_test(cwd_relative_threads.c FS)

add_wasilibc_test(futex_wait_in_past.c)
add_wasilibc_test(futex_wait_mismatch.c)
if (WASILIBC_MULTITHREADING)
  add_wasilibc_test(futex_wake_one.c)
  add_wasilibc_test(futex_wake_all.c)
endif()

if (TARGET_TRIPLE MATCHES "-threads")
  # atomic.wait32 can't be executed in this context
  expect_fail_under_v8(futex_wake_one.wasm)
  expect_fail_under_v8(futex_wake_all.wasm)
endif()

# These futex tests should trap on single-threaded targets.
if (NOT WASILIBC_MULTITHREADING)
  set_tests_properties(futex_wait_in_past.wasm PROPERTIES WILL_FAIL TRUE)
  if (TEST shared_futex_wait_in_past.wasm)
    set_tests_properties(shared_futex_wait_in_past.wasm PROPERTIES WILL_FAIL TRUE)
  endif()
endif()

# ========= sockets-related tests ===============================

if (NOT (WASI STREQUAL "p1"))
  add_wasilibc_test(poll-connect.c NETWORK)
  add_wasilibc_test(poll-nonblocking-socket.c NETWORK)
  add_wasilibc_test(setsockopt.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-udp.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-multiple.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-udp-multiple.c NETWORK)
  add_wasilibc_test(sockets-fcntl.c NETWORK)
  add_wasilibc_test(sockets-udp-connected.c NETWORK)
  add_wasilibc_test(getaddrinfo.c NETWORK)
  add_wasilibc_test(sockets-nonblocking.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-udp-no-connection.c NETWORK)
  add_wasilibc_test(sockets-eof-delayed.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-accept-multiple.c NETWORK)
  add_wasilibc_test(sockets-nonblocking-shutdown.c NETWORK)

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
  endfunction()

  sockets_test(sockets sockets-client.wasm sockets-server.wasm)
  sockets_test(sockets-udp-blocking sockets-client-udp-blocking.wasm
    sockets-server-udp-blocking.wasm)
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

# Disable some tests that can't run in V8
expect_fail_under_v8(hello.wasm)
# Skip test that uses environment variables
expect_fail_under_v8(external_env.wasm)
# Skip test that uses command-line arguments
expect_fail_under_v8(argv_two_args.wasm)
if (TARGET_TRIPLE MATCHES "-threads")
  # atomic.wait32 can't be executed on the main thread
  expect_fail_under_v8(libc_test_functional_pthread_mutex.wasm)
  expect_fail_under_v8(libc_test_functional_pthread_tsd.wasm)
  # "poll_oneoff" can't be implemented in the browser
  expect_fail_under_v8(libc_test_functional_pthread_cond.wasm)
endif()


# ========= Open POSIX Test Suite tests ========================

function(add_posix_test test_file)
  cmake_path(REPLACE_EXTENSION test_file wasm OUTPUT_VARIABLE test_name)
  string(REPLACE "/" "_" test_name ${test_name})
  set(test_name "open_posix_test_${test_name}")
  set(test_file "${POSIX_TESTSUITE}/${test_file}")
  set_source_files_properties(${test_file} PROPERTIES GENERATED TRUE)

  add_test_pair(${test_name} ${test_file}
    CFLAGS -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-global
    INCLUDE ${POSIX_TESTSUITE}/include
    DEPENDENCIES posix-testsuite)
endfunction()

if (ENABLE_COOP_THREADS)
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
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/1-2.c)
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/2-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/2-2.c)
  # unsupported(mmap): conformance/interfaces/pthread_cond_broadcast/2-3.c
  add_posix_test(conformance/interfaces/pthread_cond_broadcast/4-1.c)
  # unsupported(signals): conformance/interfaces/pthread_cond_broadcast/4-2.c

  add_posix_test(conformance/interfaces/pthread_cond_destroy/1-1.c)
  # temporarily unsupported(timed waits): conformance/interfaces/pthread_cond_destroy/2-1.c
  add_posix_test(conformance/interfaces/pthread_cond_destroy/3-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_destroy/speculative/4-1.c)

  add_posix_test(conformance/interfaces/pthread_cond_init/1-1.c)
  # unsupported(set time): conformance/interfaces/pthread_cond_init/1-2.c
  # unsupported(processes): conformance/interfaces/pthread_cond_init/1-3.c
  add_posix_test(conformance/interfaces/pthread_cond_init/2-1.c)
  # unsupported(set time): conformance/interfaces/pthread_cond_init/2-2.c
  add_posix_test(conformance/interfaces/pthread_cond_init/3-1.c)
  # unsupported(fork): conformance/interfaces/pthread_cond_init/4-1.c
  # unsupported(signals): conformance/interfaces/pthread_cond_init/4-2.c

  add_posix_test(conformance/interfaces/pthread_cond_signal/1-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_signal/1-2.c)
  add_posix_test(conformance/interfaces/pthread_cond_signal/2-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_signal/2-2.c)
  add_posix_test(conformance/interfaces/pthread_cond_signal/4-1.c)
  # unsupported(signals): conformance/interfaces/pthread_cond_signal/4-2.c

  add_posix_test(conformance/interfaces/pthread_cond_timedwait/1-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-1.c)
  # unsupported(signals/exit) add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-2.c)
  # unsupported(signals/exit) add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-3.c)
  # unsupported(processes) add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-4.c)
  add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-5.c)
  # unsupported(fails??) add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-6.c)
  # unsupported(processes) add_posix_test(conformance/interfaces/pthread_cond_timedwait/2-7.c)
  add_posix_test(conformance/interfaces/pthread_cond_timedwait/3-1.c)
  add_posix_test(conformance/interfaces/pthread_cond_timedwait/4-1.c)
  # unsupported(processes) add_posix_test(conformance/interfaces/pthread_cond_timedwait/4-2.c)
  # unsupported(signals) add_posix_test(conformance/interfaces/pthread_cond_timedwait/4-3.c)

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

  add_posix_test(conformance/interfaces/pthread_condattr_setclock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_condattr_setclock/1-2.c)
  # unsupported(getcpuclockid): conformance/interfaces/pthread_condattr_setclock/1-3.c
  add_posix_test(conformance/interfaces/pthread_condattr_setclock/2-1.c)

  # unsupported(process-shared): conformance/interfaces/pthread_condattr_setpshared/*

  add_posix_test(conformance/interfaces/pthread_create/1-1.c)
  # unsupported(cancellation): conformance/interfaces/pthread_create/1-2.c
  # unsupported(signals): conformance/interfaces/pthread_create/1-3.c
  # unsupported(processes): conformance/interfaces/pthread_create/1-4.c
  # unsupported(fork): add_posix_test(conformance/interfaces/pthread_create/1-5.c
  # unsupported(scheduling): conformance/interfaces/pthread_create/1-6.c
  # unsupported(signals): add_posix_test(conformance/interfaces/pthread_create/10-1.c)
  # bad test(incorrect assumption):conformance/interfaces/pthread_create/11-1.c
  add_posix_test(conformance/interfaces/pthread_create/12-1.c)
  # unsupported(signals): conformance/interfaces/pthread_create/14-1.c
  add_posix_test(conformance/interfaces/pthread_create/15-1.c)
  # unsupported(ub): add_posix_test(conformance/interfaces/pthread_create/2-1.c)
  add_posix_test(conformance/interfaces/pthread_create/3-1.c)
  # unsupported(fork): conformance/interfaces/pthread_create/3-2.c
  add_posix_test(conformance/interfaces/pthread_create/4-1.c)
  add_posix_test(conformance/interfaces/pthread_create/5-1.c)
  add_posix_test(conformance/interfaces/pthread_create/5-2.c)
  # unsupported(signals): conformance/interfaces/pthread_create/8-1.c
  # unsupported(signals): conformance/interfaces/pthread_create/8-2.c

  add_posix_test(conformance/interfaces/pthread_detach/1-1.c)
  # unsupported(ub): add_posix_test(conformance/interfaces/pthread_detach/1-2.c)
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

  # unsupported(exit) add_posix_test(conformance/interfaces/pthread_mutex_timedlock/1-1.c)
  # unsupported(exit) add_posix_test(conformance/interfaces/pthread_mutex_timedlock/2-1.c)
  # unsupported(exit) add_posix_test(conformance/interfaces/pthread_mutex_timedlock/4-1.c)
  # unsupported(exit) add_posix_test(conformance/interfaces/pthread_mutex_timedlock/5-1.c)
  # unsupported(exit) add_posix_test(conformance/interfaces/pthread_mutex_timedlock/5-2.c)
  add_posix_test(conformance/interfaces/pthread_mutex_timedlock/5-3.c)

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

  add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/2-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/3-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/5-1.c)
  # unsupported(signals) add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/6-1.c)
  # unsupported(signals) add_posix_test(conformance/interfaces/pthread_rwlock_timedrdlock/6-2.c)

  add_posix_test(conformance/interfaces/pthread_rwlock_timedwrlock/1-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_timedwrlock/2-1.c)
  add_posix_test(conformance/interfaces/pthread_rwlock_timedwrlock/5-1.c)
  # unsupported(signals) add_posix_test(conformance/interfaces/pthread_rwlock_timedwrlock/6-1.c)
  # unsupported(signals) add_posix_test(conformance/interfaces/pthread_rwlock_timedwrlock/6-2.c)

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

  set(cflags "--target=${TARGET_TRIPLE} --sysroot=${SYSROOT}")
  set(ldflags "${cflags} -resource-dir ${tmp_resource_dir}")

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
        -E env CFLAGS=${cflags} LDFLAGS=${ldflags} --
        ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi configure-host
        # Note that this gives double the stack that Python does by default to
        # allow for minor changes in what seems to be a combination of either
        # wasi-libc or wasmtime or something like that. Additionally, while
        # wasip3 is being developed, this passes extra flags to enable running
        # wasip3 binaries.
        #
        # Finally, the `--env` part here is copied from Python's own configuration,
        # and if Python is updated we'll have to update this too.
        --host-runner "\
          ${ENGINE} run \
            --wasm max-wasm-stack=33554432 \
            --dir <SOURCE_DIR>::/ \
            --env PYTHONPATH=/cross-build/wasm32-wasip1/build/lib.wasi-wasm32-3.14 \
          "
          -- --config-cache
    COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${cflags} LDFLAGS=${ldflags} --
        ${PYTHON} <SOURCE_DIR>/Tools/wasm/wasi make-host
    COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${cflags} LDFLAGS=${ldflags} --
        ${MAKE} --directory cross-build/wasm32-wasip1 pythoninfo

    INSTALL_COMMAND ""

    TEST_COMMAND
      ${CMAKE_COMMAND}
        -E env CFLAGS=${cflags} LDFLAGS=${ldflags} --
        ${MAKE} --directory cross-build/wasm32-wasip1 test
  )
  ExternalProject_Add_StepDependencies(python configure sysroot engine)
endif()
