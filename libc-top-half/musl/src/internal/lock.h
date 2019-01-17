#ifndef LOCK_H
#define LOCK_H

#if defined(__wasilibc_unmodified_upstream__) || defined(_REENTRANT)
hidden void __lock(volatile int *);
hidden void __unlock(volatile int *);
#define LOCK(x) __lock(x)
#define UNLOCK(x) __unlock(x)
#else
// No locking needed.
#define LOCK(x) ((void)(x))
#define UNLOCK(x) ((void)(x))
#endif

#endif
