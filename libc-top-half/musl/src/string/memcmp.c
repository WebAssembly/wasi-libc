#include <string.h>
#include <stdint.h>

#ifdef __wasm_simd128__
#include <wasm_simd128.h>
#endif

int memcmp(const void *vl, const void *vr, size_t n)
{
	const unsigned char *l=vl, *r=vr;

#if defined(__wasm_simd128__) && defined(__wasilibc_simd_string)
	if (n >= sizeof(v128_t)) {
		// memcmp is allowed to read up to n bytes from each object.
		// Find the first different character in the objects.
		// Unaligned loads handle the case where the objects
		// have mismatching alignments.
		const v128_t *v1 = (v128_t *)vl;
		const v128_t *v2 = (v128_t *)vr;
		while (n) {
			const v128_t cmp = wasm_i8x16_eq(wasm_v128_load(v1), wasm_v128_load(v2));
			// Bitmask is slow on AArch64, all_true is much faster.
			if (!wasm_i8x16_all_true(cmp)) {
				// Find the offset of the first zero bit (little-endian).
				size_t ctz = __builtin_ctz(~wasm_i8x16_bitmask(cmp));
				const unsigned char *u1 = (unsigned char *)v1 + ctz;
				const unsigned char *u2 = (unsigned char *)v2 + ctz;
				// This may help the compiler if the function is inlined.
				__builtin_assume(*u1 - *u2 != 0);
				return *u1 - *u2;
			}
			// This makes n a multiple of sizeof(v128_t)
			// for every iteration except the first.
			size_t align = (n - 1) % sizeof(v128_t) + 1;
			v1 = (v128_t *)((char *)v1 + align);
			v2 = (v128_t *)((char *)v2 + align);
			n -= align;
		}
		return 0;
	}
#else
	// Compare 8 bytes at a time.
	if (n >= sizeof(uint64_t)) {
		do {
			uint64_t a, b;
			__builtin_memcpy(&a, l, sizeof(a));
			__builtin_memcpy(&b, r, sizeof(b));
			if (a != b) {
				// The lowest set bit is in the first byte that differs.
				int shift = __builtin_ctzll(a ^ b) & ~7;
				return (int)((a >> shift) & 0xff) - (int)((b >> shift) & 0xff);
			}
			l += sizeof(a);
			r += sizeof(b);
			n -= sizeof(a);
		} while (n >= sizeof(uint64_t));
	}
#endif

	for (; n && *l == *r; n--, l++, r++);
	return n ? *l-*r : 0;
}
