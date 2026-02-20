#ifndef LOCK_H
#define LOCK_H

#include <wasi/version.h>

// Defines internal locking primitives. The main elements defined here are:
// - __lock_t: the type of a lock variable
// - __lock_ptr_t: the type of a pointer to a lock variable
// - __LOCK_INIT: initializer for a lock variable
// - STRONG_LOCK/STRONG_UNLOCK: functions to acquire/release a lock for a critical section that contains 
//                              potential context switch points, and thus must be executed even in cooperative threading mode.
// - WEAK_LOCK/WEAK_UNLOCK: functions to acquire/release a lock for a critical section that does not contain any context switch points, 
//                            and thus can be no-ops in cooperative threading mode.


#ifdef __wasi_cooperative_threads__
    struct __waitlist_node;
    struct __coop_lock {
    	int owner;  // tid of owning thread, or 0 if unlocked
    	struct __waitlist_node *waiters;
    };
    
    typedef struct __coop_lock __lock_t;
    typedef struct __coop_lock *__lock_ptr_t;
    #define __COOP_LOCK_INIT {0, NULL}
    #define __LOCK_INIT __COOP_LOCK_INIT
    
    hidden void __lock(struct __coop_lock *lock);
    hidden void __unlock(struct __coop_lock *lock);

    #define STRONG_LOCK(x) __lock(x)
    #define STRONG_UNLOCK(x) __unlock(x)
    #define WEAK_LOCK(x) ((void)0)
    #define WEAK_UNLOCK(x) ((void)0)
#elif defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
    typedef volatile int __lock_t;
    typedef volatile int *__lock_ptr_t;

    #define __LOCK_INIT {0}
    
    hidden void __lock(volatile int *);
    hidden void __unlock(volatile int *);
    
    #define STRONG_LOCK(x) __lock(x)
    #define STRONG_UNLOCK(x) __unlock(x)
    #define WEAK_LOCK(x) __lock(x)
    #define WEAK_UNLOCK(x) __unlock(x)
#else
    // No locking needed.
    #define STRONG_LOCK(x) ((void)0)
    #define STRONG_UNLOCK(x) ((void)0)
    #define WEAK_LOCK(x) ((void)0)
    #define WEAK_UNLOCK(x) ((void)0)
#endif

#endif