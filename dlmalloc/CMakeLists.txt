# Disable some warnings in this third-party code.
add_compile_options(-Wno-null-pointer-arithmetic -Wno-unused-but-set-variable)

add_object_library(dlmalloc src/dlmalloc.c)
foreach(obj dlmalloc-shared dlmalloc-static)
  target_include_directories(${obj} PRIVATE include)
endforeach()
