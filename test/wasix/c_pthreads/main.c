
#include <pthread.h>
#include <stdio.h>

void *thread(void *ptr)
{
    long type = (long)ptr;
    fprintf(stdout,"thread %ld\n", type);
    return  ptr;
}

int main(int argc, char **argv)
{
    pthread_t thread1, thread2;
    int thr = 1;
    int thr2 = 2;
    pthread_create(&thread1, NULL, *thread, (void*)thr);
    pthread_create(&thread2, NULL, *thread, (void*)thr2);

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

    fprintf(stdout, "threads joined\n");

    return 0;
}