//gcc eventfd.c -o eventfd -lpthread
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/eventfd.h>

int efd = -1;

void *read_thread(void *dummy)
{
    while (1)
    {
        fd_set set;
        struct timeval timeout;

        /* Initialize the file descriptor set. */
        FD_ZERO (&set);
        FD_SET (efd, &set);

        /* Initialize the timeout data structure. */
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        /* select returns 0 if timeout, 1 if input available, -1 if error. */
        int ret = select (FD_SETSIZE,
                &set, NULL, NULL,
                &timeout);
        if (ret == 1) {
            if (FD_ISSET(efd, &set))
            {
                uint64_t count = 0;
                ret = read(efd, &count, sizeof(count));
                if (ret < 0)
                {
                    perror("read fail:");
                    goto fail;
                }
                else
                {
                    struct timeval tv;

                    gettimeofday(&tv, NULL);
                    printf("success read from efd, read %d bytes(%llu) at %llds %lldus\n",
                        ret, (unsigned long long)count, (long long)tv.tv_sec, (long long)tv.tv_usec);
                }
            }
        } else if (ret == 0) {
            /* time out */
            printf("poll wait timed out.\n");
            break;
        } else {
            perror("poll wait error:");
            goto fail;
        }
    }

fail:
    if (efd >= 0)
    {
        close(efd);
        efd = -1;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int pid = 0;
    uint64_t count = 0;
    int ret = 0;
    int i = 0;

    //efd = eventfd(0, 0);
    printf("EFD_NONBLOCK:%d\n",EFD_NONBLOCK);
    efd = eventfd(0, EFD_NONBLOCK);
    if (efd < 0)
    {
        perror("eventfd failed.");
        goto fail;
    }

    ret = fork();
    if (ret < 0)
    {
        perror("fork failed:");
        goto fail;
    }
    if (ret == 0) {
        read_thread(NULL);
        return 0;
    }

    for (i = 0; i < 5; i++)
    {
        count = 4;
        ret = write(efd, &count, sizeof(count));
        if (ret < 0)
        {
            perror("write event fd fail:");
            goto fail;
        }
        else
        {
            struct timeval tv;

            gettimeofday(&tv, NULL);
            printf("success write to efd, write %d bytes(%llu) at %llds %lldus\n",
                   ret, (unsigned long long)count, (long long)tv.tv_sec, (long long)tv.tv_usec);
        }

        sleep(1);
    }

fail:
    if (0 != pid)
    {
        int status = 0;
        waitpid(pid, &status, 0);
        if (status != 0 && ret == 0) {
            ret = status;
        }
        pid = 0;
    }

    if (efd >= 0)
    {
        close(efd);
        efd = -1;
    }
    return ret;
}