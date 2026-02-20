#include "pthread_impl.h"
#include <wasi/api.h>

#ifndef __wasip3__
#error "Unknown WASI version"
#endif

void __waitlist_wait_on(struct __waitlist_node **list)
{
    struct __waitlist_node node = {
        .tid = wasip3_thread_index(),
        .next = *list,
    };
    *list = &node;
    
    wasip3_thread_suspend();
}

void __waitlist_wake_one(struct __waitlist_node **list)
{
    if (*list == NULL) {
        return;
    }
    struct __waitlist_node *node = *list;
    *list = node->next;
    wasip3_thread_yield_to_suspended(node->tid);
}

void __waitlist_wake_all(struct __waitlist_node **list)
{
    struct __waitlist_node **prev = list;
    struct __waitlist_node *curr = *list;

    while (curr) {
        uint32_t tid = curr->tid;
        *prev = curr->next;
        // As a scheduling optimization, we always yield directly to the last
        // suspended thread instead of just scheduling it to run at some point.
        if (curr->next == NULL) {
            wasip3_thread_yield_to_suspended(tid);
        }
        else {
            wasip3_thread_unsuspend(tid);
        }
        curr = *prev;
    }
}