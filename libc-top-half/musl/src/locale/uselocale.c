#include "locale_impl.h"
#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
#include "pthread_impl.h"
#endif
#include "libc.h"

locale_t __uselocale(locale_t new)
{
#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
	pthread_t self = __pthread_self();
	locale_t old = self->locale;
#else
	locale_t old = &libc.global_locale;
#endif
	locale_t global = &libc.global_locale;

#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
	if (new) self->locale = new == LC_GLOBAL_LOCALE ? global : new;
#else
        if (new != NULL && new != global && new != LC_GLOBAL_LOCALE) {
            fputs("non-global locale not supported yet", stderr);
            __builtin_trap();
        }
#endif

	return old == global ? LC_GLOBAL_LOCALE : old;
}

weak_alias(__uselocale, uselocale);