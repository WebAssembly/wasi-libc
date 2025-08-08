#if defined(__wasm_simd128__) && defined(__wasilibc_simd_string)
// Skip Clang 19 and Clang 20 which have a bug (llvm/llvm-project#146574)
// which results in an ICE when inline assembly is used with a vector result.
#if __clang_major__ != 19 && __clang_major__ != 20

#include <stdint.h>
#include <string.h>
#include <wasm_simd128.h>

#if !defined(__wasm_relaxed_simd__) || !defined(__RELAXED_FN_ATTRS)
#define wasm_i8x16_relaxed_swizzle wasm_i8x16_swizzle
#endif

// SIMDized check which bytes are in a set (Geoff Langdale)
// http://0x80.pl/notesen/2018-10-18-simd-byte-lookup.html

typedef struct {
  __u8x16 lo;
  __u8x16 hi;
} __wasm_v128_bitmap256_t;

__attribute__((always_inline))
static void __wasm_v128_setbit(__wasm_v128_bitmap256_t *bitmap, int i) {
  uint8_t hi_nibble = (uint8_t)i >> 4;
  uint8_t lo_nibble = (uint8_t)i & 0xf;
  bitmap->lo[lo_nibble] |= (uint8_t)((uint32_t)1 << (hi_nibble - 0));
  bitmap->hi[lo_nibble] |= (uint8_t)((uint32_t)1 << (hi_nibble - 8));
}

__attribute__((always_inline))
static v128_t __wasm_v128_chkbits(__wasm_v128_bitmap256_t bitmap, v128_t v) {
  v128_t hi_nibbles = wasm_u8x16_shr(v, 4);
  v128_t bitmask_lookup = wasm_u8x16_const(1, 2, 4, 8, 16, 32, 64, 128,  //
                                           1, 2, 4, 8, 16, 32, 64, 128);
  v128_t bitmask = wasm_i8x16_relaxed_swizzle(bitmask_lookup, hi_nibbles);

  v128_t indices_0_7 = v & wasm_u8x16_const_splat(0x8f);
  v128_t indices_8_15 = indices_0_7 ^ wasm_u8x16_const_splat(0x80);

  v128_t row_0_7 = wasm_i8x16_swizzle(bitmap.lo, indices_0_7);
  v128_t row_8_15 = wasm_i8x16_swizzle(bitmap.hi, indices_8_15);

  v128_t bitsets = row_0_7 | row_8_15;
  return wasm_i8x16_eq(bitsets & bitmask, bitmask);
}

size_t strspn(const char *s, const char *c)
{
  // Note that reading before/after the allocation of a pointer is UB in
  // C, so inline assembly is used to generate the exact machine
  // instruction we want with opaque semantics to the compiler to avoid
  // the UB.
  uintptr_t align = (uintptr_t)s % sizeof(v128_t);
  uintptr_t addr = (uintptr_t)s - align;

  if (!c[0]) return 0;
  if (!c[1]) {
    v128_t vc = wasm_i8x16_splat(*c);
    for (;;) {
      v128_t v;
      __asm__(
          "local.get %1\n"
          "v128.load 0\n"
          "local.set %0\n"
          : "=r"(v)
          : "r"(addr)
          : "memory");
      v128_t cmp = wasm_i8x16_eq(v, vc);
      // Bitmask is slow on AArch64, all_true is much faster.
      if (!wasm_i8x16_all_true(cmp)) {
        // Clear the bits corresponding to align (little-endian)
        // so we can count trailing zeros.
        int mask = (uint16_t)~wasm_i8x16_bitmask(cmp) >> align << align;
        // At least one bit will be set, unless align cleared them.
        // Knowing this helps the compiler if it unrolls the loop.
        __builtin_assume(mask || align);
        // If the mask became zero because of align,
        // it's as if we didn't find anything.
        if (mask) {
          // Find the offset of the first one bit (little-endian).
          return addr - (uintptr_t)s + __builtin_ctz(mask);
        }
      }
      align = 0;
      addr += sizeof(v128_t);
    }
  }

  __wasm_v128_bitmap256_t bitmap = {};

  for (; *c; c++) {
    // Terminator IS NOT on the bitmap.
    __wasm_v128_setbit(&bitmap, *c);
  }

  for (;;) {
    v128_t v;
    __asm__(
        "local.get %1\n"
        "v128.load 0\n"
        "local.set %0\n"
        : "=r"(v)
        : "r"(addr)
        : "memory");
    v128_t cmp = __wasm_v128_chkbits(bitmap, v);
    // Bitmask is slow on AArch64, all_true is much faster.
    if (!wasm_i8x16_all_true(cmp)) {
      // Clear the bits corresponding to align (little-endian)
      // so we can count trailing zeros.
      int mask = (uint16_t)~wasm_i8x16_bitmask(cmp) >> align << align;
      // At least one bit will be set, unless align cleared them.
      // Knowing this helps the compiler if it unrolls the loop.
      __builtin_assume(mask || align);
      // If the mask became zero because of align,
      // it's as if we didn't find anything.
      if (mask) {
        // Find the offset of the first one bit (little-endian).
        return addr - (uintptr_t)s + __builtin_ctz(mask);
      }
    }
    align = 0;
    addr += sizeof(v128_t);
  }
}

size_t strcspn(const char *s, const char *c)
{
  if (!c[0] || !c[1]) return __strchrnul(s, *c) - s;

  // Note that reading before/after the allocation of a pointer is UB in
  // C, so inline assembly is used to generate the exact machine
  // instruction we want with opaque semantics to the compiler to avoid
  // the UB.
  uintptr_t align = (uintptr_t)s % sizeof(v128_t);
  uintptr_t addr = (uintptr_t)s - align;

  __wasm_v128_bitmap256_t bitmap = {};

  do {
    // Terminator IS on the bitmap.
    __wasm_v128_setbit(&bitmap, *c);
  } while (*c++);

  for (;;) {
    v128_t v;
    __asm__(
        "local.get %1\n"
        "v128.load 0\n"
        "local.set %0\n"
        : "=r"(v)
        : "r"(addr)
        : "memory");
    v128_t cmp = __wasm_v128_chkbits(bitmap, v);
    // Bitmask is slow on AArch64, any_true is much faster.
    if (wasm_v128_any_true(cmp)) {
      // Clear the bits corresponding to align (little-endian)
      // so we can count trailing zeros.
      int mask = wasm_i8x16_bitmask(cmp) >> align << align;
      // At least one bit will be set, unless align cleared them.
      // Knowing this helps the compiler if it unrolls the loop.
      __builtin_assume(mask || align);
      // If the mask became zero because of align,
      // it's as if we didn't find anything.
      if (mask) {
        // Find the offset of the first one bit (little-endian).
        return addr - (uintptr_t)s + __builtin_ctz(mask);
      }
    }
    align = 0;
    addr += sizeof(v128_t);
  }
}

#endif
#endif
