#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int forkexample()
{
    int x = 1;

    int pid = fork();
    if (pid == -1) {
        printf("failed to fork - %d", errno);
        return 1;
    }

    if (pid == 0)
        printf("Child has x = %d\n", ++x);
    else {
        printf("Parent has x = %d\n", --x);

        int status = 0;
        waitpid(pid, &status, 0);
        printf("Child(%d) exited with %d\n", pid, status);
    }

    return 0;
}
int main()
{
    forkexample();
    return 0;
}
