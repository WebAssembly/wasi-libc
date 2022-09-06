#ifndef SIGNAL_H
#define SIGNAL_H

#include "../../include/signal.h"

hidden int __sigaction(int, const struct sigaction *, struct sigaction *);

#ifdef __wasilibc_unmodified_upstream
hidden void __block_all_sigs(void *);
hidden void __block_app_sigs(void *);
hidden void __restore_sigs(void *);
#endif

hidden void __get_handler_set(sigset_t *);

#endif
