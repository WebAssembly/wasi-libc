set(LIBC_BOTTOM_HALF_HEADERS_PUBLIC_DIR ${PROJECT_SOURCE_DIR}/libc-bottom-half/headers/public)
set(LIBC_TOP_HALF_MUSL_DIR ${PROJECT_SOURCE_DIR}/libc-top-half/musl)
set(LIBC_TOP_HALF_MUSL_SRC_DIR ${LIBC_TOP_HALF_MUSL_DIR}/src)
set(LIBC_BOTTOM_HALF_DIR ${PROJECT_SOURCE_DIR}/libc-bottom-half)
set(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_DIR ${LIBC_BOTTOM_HALF_DIR}/cloudlibc/src)

set(INSTALL_LIB_DIR lib/${MULTIARCH_TRIPLE})
set(INSTALL_SHARE_DIR share/${MULTIARCH_TRIPLE})

include(global-compiler-options.cmake)
include(local-sysroot.cmake)
include(predefined-macros.cmake)
include(libc-imports.cmake)

add_subdirectory(libc-bottom-half)
add_subdirectory(libc-top-half)

if(${MALLOC_IMPL} STREQUAL "dlmalloc")
  add_subdirectory(dlmalloc)
endif()

add_library(wasi-emulated-signal)
target_link_libraries(wasi-emulated-signal
  wasi-emulated-signal-musl-objs
  wasi-emulated-signal-objs
)
add_custom_command(TARGET wasi-emulated-signal POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)

add_library(c)

set (LIBC_LINK_LIBRARIES
  libc-bottom-half
)

if(${BUILD_LIBC_TOP_HALF} STREQUAL "ON")
    list(APPEND LIBC_LINK_LIBRARIES
      libc-top-half
    )
endif()

if(${MALLOC_IMPL} STREQUAL "dlmalloc")
  list(APPEND LIBC_LINK_LIBRARIES
    dlmalloc)
elseif(${MALLOC_IMPL} STREQUAL "none")
  # nothing to do here
else()
  message(FATAL_ERROR "unknown malloc implementation ${MALLOC_IMPL}")
endif()

target_link_libraries(c
  PRIVATE
    ${LIBC_LINK_LIBRARIES}
)

add_custom_command(TARGET c POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)

add_dependencies(c sysroot-headers)

# Create empty placeholder libraries.
foreach(stub m rt pthread crypt util xnet resolve dl)
  set(lib_path ${SYSROOT_LIB_DIR}/lib${stub}.a)
  add_custom_command(OUTPUT ${lib_path}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SYSROOT_LIB_DIR}
    COMMAND ${CMAKE_C_COMPILER_AR} crs ${lib_path})
  add_custom_target(${stub} ALL
    DEPENDS ${lib_path})
endforeach()

install(
  TARGETS
    c
    c-printscan-long-double
    c-printscan-no-floating-point
    wasi-emulated-mman
    wasi-emulated-process-clocks
    wasi-emulated-getpid
    wasi-emulated-signal
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
)

install(
  DIRECTORY ${SYSROOT_INC_DIR}
  DESTINATION .
)

install(
  DIRECTORY ${SYSROOT_SHARE_DIR}
  DESTINATION share
)

install(
  DIRECTORY ${SYSROOT_LIB_DIR}
  DESTINATION lib
)

add_test(NAME check-metadata
  COMMAND diff -wur
    ${PROJECT_SOURCE_DIR}/expected/${MULTIARCH_TRIPLE}/${THREAD_MODEL}/predefined-macros.txt
    ${CMAKE_BINARY_DIR}/sysroot/share/${MULTIARCH_TRIPLE}/predefined-macros.txt)

add_test(NAME check-headers
  COMMAND ${CMAKE_C_COMPILER}
    -target ${CMAKE_C_COMPILER_TARGET}
    --sysroot=${CMAKE_BINARY_DIR}/sysroot
    -fsyntax-only
    "-Wno#warnings"
    ${CMAKE_BINARY_DIR}/sysroot/share/${MULTIARCH_TRIPLE}/include-all.c)
