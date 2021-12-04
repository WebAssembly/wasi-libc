#include <wchar.h>
#include <stdint.h>

wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n)
{
#if defined(__wasm_bulk_memory__)
	return __builtin_wmemmove(d, s, n);
#else
	wchar_t *d0 = d;
	if (d == s) return d;
	if ((uintptr_t)d-(uintptr_t)s < n * sizeof *d)
		while (n--) d[n] = s[n];
	else
		while (n--) *d++ = *s++;
	return d0;
#endif // __wasm_bulk_memory__
}
