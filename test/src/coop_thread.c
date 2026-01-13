#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
void *thread_start(void *arg)
{
    // A thread local, which should have a separate address per-thread, but the same initial value.
    static _Thread_local int my_thread_local = 55;
    static int my_non_thread_local = 100;
    printf("Thread started: arg=%lu, my_thread_local(%p)=%d, my_non_thread_local=%d\n",
           (uintptr_t)arg, (void *)&my_thread_local, my_thread_local, my_non_thread_local);

    sched_yield();
    printf("Thread resumed: arg=%lu, my_thread_local(%p)=%d, my_non_thread_local=%d\n",
           (uintptr_t)arg, (void *)&my_thread_local, my_thread_local, my_non_thread_local);

    // This should not affect other threads' copies of the thread local.
    my_thread_local += (uintptr_t)arg;
    my_non_thread_local += (uintptr_t)arg;

    return (void *)((uintptr_t)arg + 100);
}

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t threads[3];

    printf("Creating threads...\n");
    pthread_create(&threads[0], &attr, &thread_start, (void *)12);
    pthread_create(&threads[1], &attr, &thread_start, (void *)13);
    pthread_create(&threads[2], &attr, &thread_start, (void *)14);
    printf("Threads created!\n");

    void *ret;
    pthread_join(threads[0], &ret);
    printf("Thread 1 returned: %lu\n", (uintptr_t)ret);

    pthread_join(threads[1], &ret);
    printf("Thread 2 returned: %lu\n", (uintptr_t)ret);

    pthread_join(threads[2], &ret);
    printf("Thread 3 returned: %lu\n", (uintptr_t)ret);
}
