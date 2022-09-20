set(DLMALLOC_DIR ${PROJECT_SOURCE_DIR}/dlmalloc)

add_library(dlmalloc OBJECT
  ${DLMALLOC_DIR}/src/dlmalloc.c)

target_include_directories(dlmalloc
  PRIVATE
  ${DLMALLOC_DIR}/include)

add_dependencies(dlmalloc sysroot-headers)