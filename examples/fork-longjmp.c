#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>

jmp_buf bufferA;

int example()
{
    int x = 1;
    
    int is_child = 100000;
    int do_fork = 200000;

    int child = setjmp(bufferA);
    if (child == 0) {
        longjmp(bufferA, do_fork);
    } else if (child == do_fork) {
        child = fork();
        if (child == -1) {
            printf("failed to fork - %d", errno);
            return 1;
        }
        if (child == 0) {
            longjmp(bufferA, is_child);
        } else {
            longjmp(bufferA, child);
        }
        printf("failed to longjmp - %d", errno);
        return 1;
    } else if (child == is_child) {
        printf("Child has x = %d\n", ++x);
        return 5;
    } else {
        printf("Parent has x = %d\n", --x);

        int status = 0;
        waitpid(child, &status, 0);
        printf("Child(%d) exited with %d\n", child, status);
        return 0;
    }
}
int main()
{
    return example();
}
