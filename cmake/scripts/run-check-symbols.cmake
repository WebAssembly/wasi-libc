# A CMake-based script to populate files in `share/*` related to
# defined/undefined symbols/macros.
#
# You might rightfully recoil a bit to see such a meaty script written in CMake.
# Given the choice of using shell, which isn't portable, or something like
# Python, which would only be used for this, I opted to go with CMake. Unsure
# if that was a good idea but it's at least all here and hopefully working well
# enough. If you need to edit this it's probably gonna be at the bottom related
# to filtering out macros and such.

file(GLOB objects ${SYSROOT_LIB}/*.[ao])

# ===========================================
# Generate defined-symbols.txt and undefined-symbols.txt

set(defined_symbols)
set(undefined_symbols)

foreach(file ${objects})
  if (file MATCHES "lib(setjmp|dl).a$")
    continue()
  endif()
  # TODO: Use llvm-nm --extern-only instead of grep. This is blocked on
  # LLVM PR40497, which is fixed in 9.0, but not in 8.0.
  execute_process(
    COMMAND ${CMAKE_NM} --defined-only ${file}
    OUTPUT_VARIABLE nm_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  string(REPLACE "\n" ";" nm_lines "${nm_output}")
  list(APPEND defined_symbols ${nm_lines})

  execute_process(
    COMMAND ${CMAKE_NM} --undefined-only ${file}
    OUTPUT_VARIABLE nm_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
  )
  string(REPLACE "\n" ";" nm_lines "${nm_output}")
  list(APPEND undefined_symbols ${nm_lines})
endforeach()

# Filter/transform/sort `defined_symbols` into `final_defined_symbols`.
list(FILTER defined_symbols INCLUDE REGEX " [A-Z] ")
foreach(line IN LISTS defined_symbols)
  string(REGEX REPLACE ".* [A-Z] " "" symbol "${line}")
  list(APPEND final_defined_symbols ${symbol})
endforeach()
list(SORT final_defined_symbols)
list(REMOVE_DUPLICATES final_defined_symbols)


# Filter/transform/sort `undefined_symbols` into `final_undefined_symbols`.
list(FILTER undefined_symbols INCLUDE REGEX " U ")
foreach(line IN LISTS undefined_symbols)
  string(REGEX REPLACE ".* U " "" symbol "${line}")
  list(APPEND final_undefined_symbols ${symbol})
endforeach()
list(SORT final_undefined_symbols)
list(REMOVE_DUPLICATES final_undefined_symbols)

# Write out `defined-symbos.txt` with all our symbols.
set(out_defined_symbols ${OUT_DIR}/defined-symbols.txt)
string(JOIN "\n" defined_symbols_content ${final_defined_symbols})
file(WRITE ${out_defined_symbols} "${defined_symbols_content}")

# Write out `undefined-symbosl.txt`, but skip some symbols.
set(out_undefined_symbols ${OUT_DIR}/undefined-symbols.txt)
file(WRITE ${out_undefined_symbols} "")
foreach(symbol IN LISTS final_undefined_symbols)
  list(FIND final_defined_symbols "${symbol}" found_index)
  if(NOT(found_index EQUAL -1))
    continue()
  endif()
  # Ignore certain llvm builtin symbols such as those starting with __mul
  # since these dependencies can vary between llvm versions.
  if(NOT symbol MATCHES "^__mul" AND
     NOT symbol STREQUAL "__memory_base" AND
     NOT symbol STREQUAL "__indirect_function_table" AND
     NOT symbol STREQUAL "__tls_base")
    file(APPEND ${out_undefined_symbols} "${symbol}\n")
  endif()
endforeach()

# ===========================================
# Generate include-all.c

file(GLOB_RECURSE headers RELATIVE ${SYSROOT_INC} ${SYSROOT_INC}/*.h)
set(out_include_all ${OUT_DIR}/include-all.c)
file(WRITE ${out_include_all} "")
foreach(header IN LISTS headers)
  if(header MATCHES bits/ OR
      header MATCHES /c\\+\\+/ OR
      # setjmp.h is excluded because it requires a different compiler option
      header MATCHES "^(setjmp|signal).h$" OR
      # Ignore some headers that require extra options for example.
      header MATCHES "/(mman|times|signal|resource|descriptor_table).h$")
    continue()
  endif()
  file(APPEND ${out_include_all} "#include <${header}>\n")
endforeach()

# ===========================================
# Generate predefined-macros.txt

set(out_predefined_macros ${OUT_DIR}/predefined-macros.txt)
execute_process(
  COMMAND
    ${CMAKE_C_COMPILER}
    ${CMAKE_C_FLAGS}
    ${out_include_all}
    -isystem ${SYSROOT_INC}
    -std=gnu17
    -E -dM
    -D_ALL_SOURCE
    # Skip compiler version macros
    -U__llvm__
    -U__clang__
    -U__clang_major__
    -U__clang_minor__
    -U__clang_patchlevel__
    -U__clang_version__
    -U__clang_literal_encoding__
    -U__clang_wide_literal_encoding__
    # Skip macros related to wasm features being on or off
    -U__wasm_extended_const__
    -U__wasm_mutable_globals__
    -U__wasm_sign_ext__
    -U__wasm_multivalue__
    -U__wasm_reference_types__
    -U__wasm_nontrapping_fptoint__
    -U__wasm_bulk_memory__
    -U__wasm_bulk_memory_opt__
    # Skip some other macros that vary between compiler versions
    -U__GNUC__
    -U__GNUC_MINOR__
    -U__GNUC_PATCHLEVEL__
    -U__VERSION__
    -U__NO_MATH_ERRNO__
    -U__BITINT_MAXWIDTH__
    -U__FLT_EVAL_METHOD__
    "-Wno-#warnings"
    -Wno-builtin-macro-redefined
  OUTPUT_FILE ${out_predefined_macros}
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)
file(STRINGS ${out_predefined_macros} predefined_macro_lines)
file(WRITE ${out_predefined_macros} "")
foreach(line IN LISTS predefined_macro_lines)
  # Try to normalize some macros
  if(line MATCHES "__[A-Z0-9]*_ATOMIC_[A-Z0-9_]*_LOCK_FREE")
    string(REGEX REPLACE "__[A-Z0-9]*_ATOMIC_([A-Z0-9_]*)_LOCK_FREE" "__compiler_ATOMIC_\\1_LOCK_FREE" line "${line}")
  endif()
  if(line MATCHES "^#define __GNUC_VA_LIST $")
    string(REPLACE "__GNUC_VA_LIST " "__GNUC_VA_LIST 1" line "${line}")
  endif()

  # Skip a whole bunch of other macros that vary between compilers
  if(line MATCHES "^#define __(BOOL|INT_(LEAST|FAST)(8|16|32|64)|INT|LONG|LLONG|SHRT)_WIDTH__"
     OR line MATCHES "^#define __GCC_HAVE_SYNC_COMPARE_AND_SWAP_(1|2|4|8)"
     OR line MATCHES "^#define __FPCLASS_"
     OR line MATCHES "^#define __FLT128_"
     OR line MATCHES "^#define __MEMORY_SCOPE_"
     OR line MATCHES "^#define __GCC_(CON|DE)STRUCTIVE_SIZE"
     OR line MATCHES "^#define __STDC_EMBED_"
     OR line MATCHES "^#define __(DBL|FLT|LDBL)_NORM_MAX__"
     OR line MATCHES "^#define NDEBUG"
     OR line MATCHES "^#define __OPTIMIZE__"
     OR line MATCHES "^#define assert"
     OR line MATCHES "^#define __NO_INLINE__"
     OR line MATCHES "^#define __U?INT.*_C\\("
   )
    continue()
  endif()
  file(APPEND ${out_predefined_macros} "${line}\n")
endforeach()
