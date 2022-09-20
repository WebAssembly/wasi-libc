file(GLOB_RECURSE
    CLOCKS_SOURCES
    ${LIBC_BOTTOM_HALF_DIR}/clocks/*.c)

add_library(wasi-emulated-process-clocks STATIC
  ${CLOCKS_SOURCES})

target_include_directories(wasi-emulated-process-clocks
  PRIVATE
    ${LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_DIR})

add_dependencies(wasi-emulated-process-clocks sysroot-headers)

add_custom_command(TARGET wasi-emulated-process-clocks POST_BUILD
  COMMAND ${CMAKE_C_COMPILER_AR} crs $<TARGET_FILE:c>
)
