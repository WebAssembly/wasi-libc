#include <wasi/api.h>
#include <sys/epoll.h>
#include <signal.h>
#include <errno.h>

int epoll_create(int size)
{
    int ret_val = 0;
    int error = __wasi_epoll_create(&ret_val);
    if (error == 0)
    {
        return ret_val;
    }
    errno = error;
    return -1;
}

int epoll_create1(int flags)
{
    return epoll_create(0);
}

int epoll_ctl(int fd, int op, int fd2, struct epoll_event *ev)
{
    if (ev)
    {
        struct __wasi_epoll_event_t ev2;

        ev2.events = 0;
        if ((ev->events & EPOLLIN) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLIN;
        if ((ev->events & EPOLLOUT) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLOUT;
        if ((ev->events & EPOLLRDHUP) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLRDHUP;
        if ((ev->events & EPOLLPRI) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLPRI;
        if ((ev->events & EPOLLERR) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLERR;
        if ((ev->events & EPOLLHUP) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLHUP;
        if ((ev->events & EPOLLET) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLET;
        if ((ev->events & EPOLLONESHOT) != 0)
            ev2.events |= __WASI_EPOLL_TYPE_EPOLLONESHOT;
        ev2.data.ptr = (__wasi_pointersize_t)ev->data.ptr;
        ev2.data.fd = fd2;
        ev2.data.data1 = ev->data.u32;
        ev2.data.data2 = ev->data.u64;
        return __wasi_epoll_ctl(fd, op, fd2, &ev2);
    }
    return __wasi_epoll_ctl(fd, op, fd2, NULL);
}

int epoll_pwait(int fd, struct epoll_event *ev, int cnt, int to, const sigset_t *sigs)
{
    (void)sigs;
    __wasi_size_t ret_val;
    struct __wasi_epoll_event_t ev2[64];
    __wasi_timestamp_t timeout;
    if (to < 0) {
        timeout = 0xffffffffffffffff;
    } else {
        timeout = (__wasi_timestamp_t)to * 1000000;
    }
    if (cnt > 64)
    {
        cnt = 64;
    }
    int error = __wasi_epoll_wait(fd, &ev2[0], cnt, timeout, &ret_val);
    if (error == 0)
    {
        cnt = ret_val;
        for (int c = 0; c < cnt; c++)
        {
            ev[c].events = 0;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLIN) != 0)
                ev[c].events |= EPOLLIN;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLOUT) != 0)
                ev[c].events |= EPOLLOUT;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLRDHUP) != 0)
                ev[c].events |= EPOLLRDHUP;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLPRI) != 0)
                ev[c].events |= EPOLLPRI;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLERR) != 0)
                ev[c].events |= EPOLLERR;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLHUP) != 0)
                ev[c].events |= EPOLLHUP;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLET) != 0)
                ev[c].events |= EPOLLET;
            if ((ev2[c].events & __WASI_EPOLL_TYPE_EPOLLONESHOT) != 0)
                ev[c].events |= EPOLLONESHOT;
            if (ev2[c].data.ptr)
            {
                ev[c].data.ptr = (void *)ev2[c].data.ptr;
            }
            else if (ev2[c].data.fd)
            {
                ev[c].data.fd = ev2[c].data.fd;
            }
            else if (ev2[c].data.data1)
            {
                ev[c].data.u32 = ev2[c].data.data1;
            }
            else
            {
                ev[c].data.u64 = ev2[c].data.data2;
            }
        }
        return (int)ret_val;
    }
    errno = error;
    return -1;
}

int epoll_wait(int fd, struct epoll_event *ev, int cnt, int to)
{
    return epoll_pwait(fd, ev, cnt, to, 0);
}
