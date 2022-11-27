#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    printf("Main program started\n");

    int pid = vfork();
    if (pid == -1) {
        printf("failed to fork - %d", errno);
        return 1;
    }

    if (pid == 0) {
        printf("execve: echo hi-from-child\n");
        char* argv[] = { "echo", "hi-from-child", NULL };
        char* envp[] = { NULL };
        if (execve("/bin/echo", argv, envp) == -1) {
            perror("Could not execve");
        }
    } else {
        int status = 0;
        waitpid(pid, &status, 0);
        printf("Child(%d) exited with %d\n", pid, status);

        printf("execve: echo hi-from-parent\n");
        char* argv[] = { "echo", "hi-from-parent", NULL };
        char* envp[] = { NULL };
        if (execve("/bin/echo", argv, envp) == -1) {
            perror("Could not execve");
        }
    }

    return 1;
}
