#include <wchar.h>

wchar_t *wmemcpy(wchar_t *restrict d, const wchar_t *restrict s, size_t n)
{
#if defined(__wasm_bulk_memory__)
	return __builtin_wmemcpy(d, s, n);
#else
	wchar_t *a = d;
	while (n--) *d++ = *s++;
	return a;
#endif // __wasm_bulk_memory__
}
