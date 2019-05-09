#ifdef __wasilibc_unmodified_upstream // WASI has no network interface APIs
#include <net/if.h>
#endif
#define F(t,n) {t *y = &x.n;}
#define C(n) switch(n){case n:;}
#ifdef __wasilibc_unmodified_upstream // WASI has no network interface APIs
static void f()
{
{
struct if_nameindex x;
F(unsigned, if_index)
F(char*, if_name)
}
C(IF_NAMESIZE)
{void(*p)(struct if_nameindex*) = if_freenameindex;}
{char*(*p)(unsigned,char*) = if_indextoname;}
{struct if_nameindex*(*p)(void) = if_nameindex;}
{unsigned(*p)(const char*) = if_nametoindex;}
}
#endif
