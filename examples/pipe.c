#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main()
{
    int x = 1;
    int pipefd[2];
    int ret = pipe(pipefd);
    if (ret == -1) {
        printf("failed to create pipe - %d\n", errno);
        return 1;
    }

    int pid = fork();

    // The child will write the data to the pipe
    if (pid == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], 1);
        close(pipefd[0]);

        // Make sure the pipe is transferred over
        char buf[16];
        memset(&buf, 0, 16);
        if (write(pipefd[0], &buf, 16) >= 0) {
            printf("failed as the pipe end was moved to stdout however its still open\n");
            return 1;
        }

        // Use the stdout to send bytes to the parent
        printf("this text should be printed by the parent\n");
    } else {
        printf("this text should be printed by the child\n");

        // The parent will read the data from the pipe and write it to stdout
        close(pipefd[0]);
        dup2(pipefd[1], 0);
        close(pipefd[1]);

        // Make sure the pipe is transferred over
        char buf[16];
        if (read(pipefd[1], &buf, 16) >= 0) {
            printf("failed as the pipe end was moved to stdin however its still open\n");
            return 1;
        }

        for (;;) {
            ssize_t r = read(0, &buf, 16);
            if (r < 0) {
                printf("failed to read from pipe - %d\n", errno);
                return 1;
            }
            if (r == 0) {
                break;
            }
            write(1, &buf, r);
        }
    }
    return 0;
}