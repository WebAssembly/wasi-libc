add_internal_object_library(fts musl-fts/fts.c)
foreach(obj fts-shared fts-static)
  target_include_directories(${obj} PRIVATE . musl-fts)
endforeach()

add_sysroot_header(musl-fts/fts.h fts.h)
