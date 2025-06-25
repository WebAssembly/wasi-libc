#include <string.h>
#include <stdint.h>
#include <limits.h>

#ifdef __wasm_simd128__
#include <wasm_simd128.h>
#endif

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

size_t strlen(const char *s)
{
#if defined(__wasm_simd128__) && defined(__wasilibc_simd_string)
	// strlen must stop as soon as it finds the terminator.
	// Aligning ensures loads beyond the terminator are safe.
	// Casting through uintptr_t makes this implementation-defined,
	// rather than undefined behavior.
	uintptr_t align = (uintptr_t)s % sizeof(v128_t);
	const v128_t *v = (v128_t *)((uintptr_t)s - align);

	for (;;) {
		// Bitmask is slow on AArch64, all_true is much faster.
		if (!wasm_i8x16_all_true(*v)) {
			const v128_t cmp = wasm_i8x16_eq(*v, (v128_t){});
			// Clear the bits corresponding to alignment (little-endian)
			// so we can count trailing zeros.
			int mask = wasm_i8x16_bitmask(cmp) >> align << align;
			// At least one bit will be set, unless we cleared them.
			// Knowing this helps the compiler.
			__builtin_assume(mask || align);
			// If the mask is zero because of alignment,
			// it's as if we didn't find anything.
			if (mask) {
				// Find the offset of the first one bit (little-endian).
				return (char *)v - s + __builtin_ctz(mask);
			}
		}
		align = 0;
		v++;
	}
#endif

	const char *a = s;
#ifdef __GNUC__
	typedef size_t __attribute__((__may_alias__)) word;
	const word *w;
	for (; (uintptr_t)s % ALIGN; s++) if (!*s) return s-a;
	for (w = (const void *)s; !HASZERO(*w); w++);
	s = (const void *)w;
#endif
	for (; *s; s++);
	return s-a;
}
