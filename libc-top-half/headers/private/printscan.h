#if defined(__wasilibc_printscan_no_floating_point)
#define __wasilibc_printscan_has_double 0
#define __wasilibc_printscan_has_long_double 0
#elif defined(__wasilibc_printscan_no_long_double)
#define __wasilibc_printscan_has_double 1
#define __wasilibc_printscan_has_long_double 0
#else
#define __wasilibc_printscan_has_double 1
#define __wasilibc_printscan_has_long_double 1
#endif

#define __wasilibc_printscan_unsupported_arg(type)        \
  (((type) == DBL && !__wasilibc_printscan_has_double) || \
   ((type) == LDBL && !__wasilibc_printscan_has_long_double))

#if defined(__wasilibc_printscan_no_floating_point)

#elif defined(__wasilibc_printscan_no_long_double)

typedef double long_double;
#undef LDBL_TRUE_MIN
#define LDBL_TRUE_MIN DBL_DENORM_MIN
#undef LDBL_MIN
#define LDBL_MIN DBL_MIN
#undef LDBL_MAX
#define LDBL_MAX DBL_MAX
#undef LDBL_EPSILON
#define LDBL_EPSILON DBL_EPSILON
#undef LDBL_MANT_DIG
#define LDBL_MANT_DIG DBL_MANT_DIG
#undef LDBL_MIN_EXP
#define LDBL_MIN_EXP DBL_MIN_EXP
#undef LDBL_MAX_EXP
#define LDBL_MAX_EXP DBL_MAX_EXP
#undef LDBL_DIG
#define LDBL_DIG DBL_DIG
#undef LDBL_MIN_10_EXP
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP
#undef LDBL_MAX_10_EXP
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#undef frexpl
#define frexpl(x, exp) frexp(x, exp)
#undef copysignl
#define copysignl(x, y) copysign(x, y)
#undef fmodl
#define fmodl(x, y) fmod(x, y)
#undef scalbnl
#define scalbnl(arg, exp) scalbn(arg, exp)

#else

// Full long double support.
typedef long double long_double;

#endif
