#define a_cas(p, t, s) (__sync_val_compare_and_swap((p), (t), (s)))
#define a_crash() (__builtin_trap())
#define a_clz_32 __builtin_clz
#define a_clz_64 __builtin_clzll
#define a_ctz_32 __builtin_ctz
#define a_ctz_64 __builtin_ctzll

#ifdef __wasm64__
#define a_cas_p a_cas_p
static inline void *a_cas_p(volatile void *p, void *t, void *s)
{
	__atomic_compare_exchange(&p,(volatile void **)&t,s,0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST);
	return t;
}
#endif
