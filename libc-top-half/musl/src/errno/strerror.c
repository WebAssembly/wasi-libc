#include <errno.h>
#include <string.h>
#ifdef __wasilibc_unmodified_upstream__ // locales
#include "locale_impl.h"
#endif

#define E(a,b) ((unsigned char)a),
static const unsigned char errid[] = {
#include "__strerror.h"
};

#undef E
#define E(a,b) b "\0"
static const char errmsg[] =
#include "__strerror.h"
;

#ifdef __wasilibc_unmodified_upstream__ // locales
char *__strerror_l(int e, locale_t loc)
#else
static char *__strerror(int e)
#endif
{
	const char *s;
	int i;
	/* mips has one error code outside of the 8-bit range due to a
	 * historical typo, so we just remap it. */
	if (EDQUOT==1133) {
		if (e==109) e=-1;
		else if (e==EDQUOT) e=109;
	}
	for (i=0; errid[i] && errid[i] != e; i++);
	for (s=errmsg; i; s++, i--) for (; *s; s++);
#ifdef __wasilibc_unmodified_upstream__ // locales
	return (char *)LCTRANS(s, LC_MESSAGES, loc);
#else
	return (char *)s;
#endif
}

char *strerror(int e)
{
#ifdef __wasilibc_unmodified_upstream__ // locales
	return __strerror_l(e, CURRENT_LOCALE);
#else
	return __strerror(e);
#endif
}

#ifdef __wasilibc_unmodified_upstream__ // locales
weak_alias(__strerror_l, strerror_l);
#endif
