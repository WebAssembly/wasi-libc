add_object_library(emmalloc emmalloc.c)

# emmalloc uses a lot of pointer type-punning, which is UB under strict
# aliasing, and this was found to have real miscompilations in wasi-libc#421.
foreach(obj emmalloc-shared emmalloc-static)
  target_include_directories(${obj} PRIVATE emmalloc)
  target_compile_options(${obj} PRIVATE -fno-strict-aliasing)
endforeach()
