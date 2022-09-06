#ifndef __wasilibc___typedef_sigset_t_h
#define __wasilibc___typedef_sigset_t_h

#include <bits/signal.h>

#ifndef _NSIG
#define _NSIG 65
#endif

#ifndef _NSIG_BPW
#define _NSIG_BPW   32
#endif

#ifndef _NSIG_WORDS
#define _NSIG_WORDS (_NSIG / _NSIG_BPW)
#endif

/* TODO: This is just a placeholder for now. Keep this in sync with musl. */
typedef union {
    unsigned long sig[_NSIG_WORDS];
    unsigned long __bits[_NSIG_WORDS];
} sigset_t;

#endif
