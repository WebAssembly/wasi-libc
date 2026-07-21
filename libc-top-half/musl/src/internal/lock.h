#ifndef LOCK_H
#define LOCK_H

#include <features.h>
#include <wasi/version.h>

// Defines internal locking primitives. The main elements defined here are:
// - __lock_t: the type of a lock variable
// - __lock_ptr_t: the type of a pointer to a lock variable
// - STRONG_LOCK/STRONG_UNLOCK: functions to acquire/release a lock for a critical section that contains
//                              potential context switch points, and thus must be executed even in cooperative threading mode.
// - WEAK_LOCK/WEAK_UNLOCK: functions to acquire/release a lock for a critical section that does not contain any context switch points,
//                            and thus can be no-ops in cooperative threading mode.


#ifdef _REENTRANT
hidden void __lock(volatile int *lock);
hidden void __unlock(volatile int *lock);
# define DECLARE_STRONG_LOCK(name, ...) __VA_ARGS__ volatile int name
# define STRONG_LOCK(x) __lock(&x)
# define STRONG_UNLOCK(x) __unlock(&x)
# ifdef __wasi_cooperative_threads__
#  define DECLARE_WEAK_LOCK(name, ...)
#  define WEAK_LOCK(x) ((void)0)
#  define WEAK_UNLOCK(x) ((void)0)
# else
#  define DECLARE_WEAK_LOCK(...) DECLARE_STRONG_LOCK(__VA_ARGS__)
#  define WEAK_LOCK(x) STRONG_LOCK(x)
#  define WEAK_UNLOCK(x) STRONG_UNLOCK(x)
# endif
#else
# define DECLARE_STRONG_LOCK(name, ...)
# define DECLARE_WEAK_LOCK(name, ...)
# define STRONG_LOCK(x) ((void)0)
# define STRONG_UNLOCK(x) ((void)0)
# define WEAK_LOCK(x) ((void)0)
# define WEAK_UNLOCK(x) ((void)0)
#endif


#if defined(NDEBUG) || !defined(_REENTRANT)
# define STRONG_ASSERT_HELD(x) ((void)0)
# define STRONG_ASSERT_EMPTY(x) ((void)0)
# define WEAK_ASSERT_HELD(x) ((void)0)
# define WEAK_ASSERT_EMPTY(x) ((void)0)
#else
hidden void __wasilibc_assert_held_lock(volatile int *lock);
hidden void __wasilibc_assert_empty_lock(volatile int  *lock);
# define STRONG_ASSERT_HELD(x) __wasilibc_assert_held_lock(&x)
# define STRONG_ASSERT_EMPTY(x) __wasilibc_assert_empty_lock(&x)
# ifdef __wasi_cooperative_threads__
#  define WEAK_ASSERT_HELD(x) ((void)0)
#  define WEAK_ASSERT_EMPTY(x) ((void)0)
# else
#  define WEAK_ASSERT_HELD(x) STRONG_ASSERT_HELD(x)
#  define WEAK_ASSERT_EMPTY(x) STRONG_ASSERT_EMPTY(x)
# endif
#endif

#endif
