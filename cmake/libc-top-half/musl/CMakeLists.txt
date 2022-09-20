set(LIBC_TOP_HALF_MUSL_PRINTSCAN_SOURCES
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/internal/floatscan.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/stdio/vfprintf.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/stdio/vfwprintf.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/stdio/vfscanf.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/stdlib/strtod.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/stdlib/wcstod.c
)

add_library(c-printscan-no-floating-point
  ${LIBC_TOP_HALF_MUSL_PRINTSCAN_SOURCES}
)

target_compile_definitions(c-printscan-no-floating-point PRIVATE
  __wasilibc_printscan_no_floating_point
  __wasilibc_printscan_floating_point_support_option="remove -lc-printscan-no-floating-point from the link command")
target_compile_options(c-printscan-no-floating-point
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_COMPILE_OPTIONS}
)

target_include_directories(c-printscan-no-floating-point PRIVATE
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_INC_DIRS}
)

add_dependencies(c-printscan-no-floating-point sysroot-headers)

add_custom_command(TARGET c-printscan-no-floating-point POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)

add_library(c-printscan-long-double
  ${LIBC_TOP_HALF_MUSL_PRINTSCAN_SOURCES}
)
target_compile_options(c-printscan-long-double
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_COMPILE_OPTIONS})

target_include_directories(c-printscan-long-double
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_INC_DIRS}
)
add_dependencies(c-printscan-long-double sysroot-headers)

add_custom_command(TARGET c-printscan-long-double POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)


add_library(wasi-emulated-signal-musl-objs
  OBJECT
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/signal/psignal.c
    ${LIBC_TOP_HALF_MUSL_SRC_DIR}/string/strsignal.c
)
target_compile_definitions(wasi-emulated-signal-musl-objs
  PRIVATE
    _WASI_EMULATED_SIGNAL
)
target_compile_options(wasi-emulated-signal-musl-objs
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_COMPILE_OPTIONS})

target_include_directories(wasi-emulated-signal-musl-objs
  PRIVATE
    ${LIBC_TOP_HALF_COMMON_INC_DIRS}
)

add_dependencies(wasi-emulated-signal-musl-objs sysroot-headers)