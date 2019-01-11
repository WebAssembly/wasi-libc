#include <stdlib.h>
#ifdef __wasm_musl_unmodified_upstream__ // locales
#include "locale_impl.h"

size_t __ctype_get_mb_cur_max()
{
	return MB_CUR_MAX;
}
#endif
