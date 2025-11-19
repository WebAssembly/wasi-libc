add_object_library(dlmalloc src/dlmalloc.c)
foreach(obj dlmalloc-shared dlmalloc-static)
  target_include_directories(${obj} PRIVATE include)
endforeach()
