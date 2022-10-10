file(GLOB_RECURSE
    MMAN_SOURCES
    ${LIBC_BOTTOM_HALF_DIR}/mman/*.c)

add_library(wasi-emulated-mman STATIC
  ${MMAN_SOURCES})

add_dependencies(wasi-emulated-mman sysroot-headers)

add_custom_command(TARGET wasi-emulated-mman POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)
