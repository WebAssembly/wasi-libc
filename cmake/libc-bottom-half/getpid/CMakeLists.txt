file(GLOB_RECURSE
    GETPID_SOURCES
    ${LIBC_BOTTOM_HALF_DIR}/getpid/*.c)

add_library(wasi-emulated-getpid STATIC
  ${GETPID_SOURCES})

set_target_properties(wasi-emulated-getpid
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/sysroot/lib
)

add_dependencies(wasi-emulated-getpid sysroot-headers)

add_custom_command(TARGET wasi-emulated-getpid POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)
