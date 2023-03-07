#include "pthread_impl.h"

void *__tls_get_addr(tls_mod_off_t *v)
{
#ifdef __wasilibc_unmodified_upstream
	pthread_t self = __pthread_self();
	return (void *)(self->dtv[v[0]] + v[1]);
#else
	uintptr_t *dtv = __builtin_wasm_tls_base();
	return (void *)(dtv[v[0]] + v[1]);
#endif
}
