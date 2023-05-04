#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

static uint8_t* test_memory = NULL;

int forkexample()
{
    int x = 1;

    // Allocate a large chunk of memory and fill it with test data (64MB)
    int test_memory_size = (int)(1024 * 1024 * 64);
    test_memory = (uint8_t*)malloc(test_memory_size);
    assert(test_memory);
    for (int n = 0; n < test_memory_size; n++) {
        test_memory[n] = (uint8_t)(n % 256);
    }

    // Now fork the process
    int pid = fork();
    if (pid == -1) {
        printf("failed to fork - %d", errno);
        return 1;
    }
    if (pid == 0)
        sleep(1);
    if (pid == 0)
        fprintf(stderr, "Child has x = %d\n", ++x);
    else
        printf("Parent has x = %d\n", --x);

    // Check that the memory is correctly copied over
    assert(test_memory);
    assert(test_memory_size == (int)(1024 * 1024 * 64));
    for (int n = 0; n < test_memory_size; n++) {
        if (test_memory[n] != (uint8_t)(n % 256)) {
            printf("memory corruption (offset=%d) - %d vs %d\n", n, test_memory[n], (uint8_t)(n % 256));
            assert(test_memory[n] == (uint8_t)(n % 256));
        }
    }
    if (pid == 0)
        fprintf(stderr, "Child memory is good\n");
    else
        printf("Parent memory is good\n");

    // The parent should wait for the child
    if (pid != 0) {
        int status = 0;
        waitpid(pid, &status, 0);

        int exit_code = WEXITSTATUS(status);
        printf("Child(%d) exited with %d\n", pid, exit_code);

        assert(exit_code == 3);
    }

    if (pid == 0)
        return 3;
    else
        return 0;
}
int main()
{
    return forkexample();
}
