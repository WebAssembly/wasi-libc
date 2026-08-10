/*   
 * Copyright (c) 2002, Intel Corporation. All rights reserved.
 * This file is licensed under the GPL license.  For the full content
 * of this license, see the COPYING file at the top level of this 
 * source tree.
 *
 * pthread_barrier_init()
 *
 * The pthread_barrier_init( ) function may fail if:
 * [EBUSY] The implementation has detected an attempt to reinitialize a barrier while it is
 * in use (for example, while being used in a pthread_barrier_wait( ) call) by
 * another thread.
 *
 * NOTE: This case will always pass.
 * 
 * Steps:
 * 1. Main initialize barrier with count 2
 * 2. Main create a child thread
 * 3. Child thread call pthread_barrier_wait(), should block
 * 4. Main call pthread_barrier_init()
 */
#define _XOPEN_SOURCE 600
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "posixtest.h"

static pthread_barrier_t barrier;
static int thread_state; 
#define NOT_CREATED_THREAD 1
#define ENTERED_THREAD 2
#define EXITING_THREAD 3

/* child thread */
static void* fn_chld(void *arg)
{ 
	int rc = 0;

	thread_state = ENTERED_THREAD;

	printf("thread: barrier wait\n");
	rc = pthread_barrier_wait(&barrier);
	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		printf("Error: thread: pthread_barrier_wait() got unexpected "
			"return code : %d\n" , rc);
		exit(PTS_UNRESOLVED);
	} 
	else if(rc == PTHREAD_BARRIER_SERIAL_THREAD)
	{
		printf("thread: got PTHREAD_BARRIER_SERIAL_THREAD\n");
	}
	
	thread_state = EXITING_THREAD;
	return NULL;
}

// WASI-CHANGE: Instead of sending a SIGALRM to wake the main thread, we'll 
// just wait on the barrier properly
#ifndef __wasi__
void sig_handler()
{
	printf("Interrupted by SIGALRM\n");
	printf("Test PASSED: main blocked on barrier init\n");
	exit(PTS_PASS);
}
#endif
 
int main()
{
	int cnt = 0;
	int rc;
	pthread_t child_thread;

	// WASI-CHANGE: No SIGALARM
	#ifndef __wasi__
	struct sigaction act;	

	/* Set up main thread to handle SIGALRM */
	act.sa_flags = 0;
	act.sa_handler = sig_handler;
	sigfillset(&act.sa_mask);
	sigaction(SIGALRM, &act, 0);
	#endif
	
	printf("main: Initialize barrier with count = 2\n");
	if(pthread_barrier_init(&barrier, NULL, 2) != 0)
	{
		printf("main: Error at pthread_barrier_init()\n");
		return PTS_UNRESOLVED;
	}

	printf("main: create child thread\n");
	thread_state = NOT_CREATED_THREAD;
	if(pthread_create(&child_thread, NULL, fn_chld, NULL) != 0)
	{
		printf("main: Error at pthread_create()\n");
		return PTS_UNRESOLVED;
	}
	
	/* Expect the child to block*/
	cnt = 0;
	do{
		sleep(1);
	}while (thread_state !=EXITING_THREAD && cnt++ < 2); 


	if(thread_state == EXITING_THREAD)
	{
		/* child thread did not block */
		printf("Test FAILED: child thread did not block on "
			"pthread_barrier_wait()\n");
		exit(PTS_FAIL);
	}
	else if(thread_state != ENTERED_THREAD)
	{
		printf("Unexpected thread state\n");
		exit(PTS_UNRESOLVED);
	}

	printf("main: reinitilize barrier while thread is blocking on it\n");	
	rc = pthread_barrier_init(&barrier, NULL, 2);
	
	if(rc == EBUSY)
	{
		printf("main: pthread_barrier_init() correctly got EBUSY");
		printf("Test PASSED\n");
	} 
	else
	{
		printf("main: got return code: %d, %s\n", rc, strerror(rc));
		printf("Test PASSED: Note*: Expected EBUSY, but standard says 'may' fail.\n");
	}

	// WASI-EDIT: Unblock child thread properly instead of canceling it with pthread_cancel
	printf("main: unblocking child thread by waiting on barrier\n");
	rc = pthread_barrier_wait(&barrier);
	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
	{
		printf("Warning: pthread_barrier_wait() returned: %d, %s\n", rc, strerror(rc));
	}
	
	/* Join the child thread */
	if(pthread_join(child_thread, NULL) != 0)
	{
		printf("Error: pthread_join() failed\n");
		return PTS_UNRESOLVED;
	}
	
	return PTS_PASS;
}
