#include <stdlib.h>
#ifdef __wasilibc_unmodified_upstream__
#else
#include "printscan.h"
#endif
#include "shgetc.h"
#include "floatscan.h"
#include "stdio_impl.h"

#ifdef __wasilibc_printscan_no_long_double
static long_double strtox(const char *s, char **p, int prec)
#else
static long double strtox(const char *s, char **p, int prec)
#endif
{
	FILE f;
	sh_fromstring(&f, s);
	shlim(&f, 0);
#ifdef __wasilibc_printscan_no_long_double
	long_double y = __floatscan(&f, prec, 1);
#else
	long double y = __floatscan(&f, prec, 1);
#endif
	off_t cnt = shcnt(&f);
	if (p) *p = cnt ? (char *)s + cnt : (char *)s;
	return y;
}

float strtof(const char *restrict s, char **restrict p)
{
	return strtox(s, p, 0);
}

double strtod(const char *restrict s, char **restrict p)
{
	return strtox(s, p, 1);
}

long double strtold(const char *restrict s, char **restrict p)
{
#ifdef __wasilibc_printscan_no_long_double
	long_double_not_supported();
#else
	return strtox(s, p, 2);
#endif
}

weak_alias(strtof, strtof_l);
weak_alias(strtod, strtod_l);
weak_alias(strtold, strtold_l);
weak_alias(strtof, __strtof_l);
weak_alias(strtod, __strtod_l);
weak_alias(strtold, __strtold_l);
