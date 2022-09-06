#include <unistd.h>

#ifdef __wasilibc_unmodified_upstream // This is dealt with elsewhere
char **__environ = 0;
weak_alias(__environ, ___environ);
weak_alias(__environ, _environ);
weak_alias(__environ, environ);
#endif
