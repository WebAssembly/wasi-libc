#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

int main() {
    int pid = vfork();
    if (pid == -1) {
        printf("failed to fork - %d", errno);
        return 1;
    }

    if (pid == 0)
        _exit(10);
    else {
        printf("Parent waiting on Child(%d)\n", pid);

        int status = 0;
        waitpid(pid, &status, 0);

        int exit_code = WEXITSTATUS(status);
        printf("Child(%d) exited with %d\n", pid, exit_code);

        assert(exit_code == 10);
    }
    return 0;
}