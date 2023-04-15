#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <setjmp.h>

jmp_buf bufferA;

static uint8_t* test_memory = NULL;

void validate_memory(int test_memory_size)
{
    assert(test_memory);
    assert(test_memory_size == (int)(1024 * 1024 * 64));
    for (int n = 0; n < test_memory_size; n++) {
        if (test_memory[n] != (uint8_t)(n % 256)) {
            printf("memory corruption (offset=%d) - %d vs %d\n", n, test_memory[n], (uint8_t)(n % 256));
            assert(test_memory[n] == (uint8_t)(n % 256));
        }
    }
}

int example()
{
    int x = 1;

    // Allocate a large chunk of memory and fill it with test data (64MB)
    int test_memory_size = (int)(1024 * 1024 * 64);
    test_memory = (uint8_t*)malloc(test_memory_size);
    assert(test_memory);
    for (int n = 0; n < test_memory_size; n++) {
        test_memory[n] = (uint8_t)(n % 256);
    }
    
    int is_child = 100000;
    int do_fork = 200000;

    validate_memory(test_memory_size);

    int child = setjmp(bufferA);
    if (child == 0) {
        longjmp(bufferA, do_fork);
    } else if (child == do_fork) {
        child = fork();
        if (child == -1) {
            printf("failed to fork - %d", errno);
            return 1;
        }

        if (child == 0)
            sleep(1);

        validate_memory(test_memory_size);
        if (child == 0)
            printf("Child memory is good\n");
        else
            printf("Parent memory is good\n");

        if (child == 0) {
            longjmp(bufferA, is_child);
        } else {
            longjmp(bufferA, child);
        }
        printf("failed to longjmp - %d", errno);
        return 1;
    } else if (child == is_child) {
        validate_memory(test_memory_size);
        printf("Child memory after longjmp is good\n");

        printf("Child has x = %d\n", ++x);
        return 5;
    } else {
        validate_memory(test_memory_size);
        printf("Parent memory after longjmp is good\n");

        printf("Parent has x = %d\n", --x);

        int status = 0;
        waitpid(child, &status, 0);

        int exit_code = WEXITSTATUS(status);
        assert(exit_code == 5);

        printf("Child(%d) exited with %d\n", child, exit_code);
        return 0;
    }
}
int main()
{
    return example();
}
