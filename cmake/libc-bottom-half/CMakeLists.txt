set(LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC_DIR ${LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_DIR}/include)
set(LIBC_BOTTOM_HALF_SRC_DIR ${LIBC_BOTTOM_HALF_DIR}/sources)
set(LIBC_BOTTOM_HALF_HEADERS_PRIVATE_DIR ${LIBC_BOTTOM_HALF_DIR}/headers/private)

file(GLOB_RECURSE
  LIBC_BOTTOM_HALF_ALL_SOURCES
  CONFIGURE_DEPENDS
  ${LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_DIR}/*.c
  ${LIBC_BOTTOM_HALF_SRC_DIR}/*.c)

set(LIBC_BOTTOM_HALF_INC_DIRS
  ${LIBC_BOTTOM_HALF_HEADERS_PRIVATE_DIR}
  ${LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_INC_DIR}
  ${LIBC_BOTTOM_HALF_CLOUDLIBC_SRC_DIR}
  ${LIBC_TOP_HALF_MUSL_SRC_DIR}/include
  ${LIBC_TOP_HALF_MUSL_SRC_DIR}/internal
)

# FIXME(https://reviews.llvm.org/D85567) - due to a bug in LLD the weak
# references to a function defined in `chdir.c` only work if `chdir.c` is at the
# end of the archive, but once that LLD review lands and propagates into LLVM
# then we don't have to do this.
set(CHDIR_SRC "${LIBC_BOTTOM_HALF_SRC_DIR}/chdir.c")
list(REMOVE_ITEM LIBC_BOTTOM_HALF_ALL_SOURCES ${CHDIR_SRC})
list(APPEND LIBC_BOTTOM_HALF_ALL_SOURCES ${CHDIR_SRC})

add_library(libc-bottom-half OBJECT
  ${LIBC_BOTTOM_HALF_ALL_SOURCES})
add_dependencies(libc-bottom-half sysroot-headers)
target_include_directories(libc-bottom-half
  PRIVATE
    ${LIBC_BOTTOM_HALF_INC_DIRS}
  SYSTEM
    ${SYSROOT_INC_DIR}
)

add_subdirectory(clocks)
add_subdirectory(getpid)
add_subdirectory(mman)
add_subdirectory(signal)
add_subdirectory(crt1)