#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define RETVAL_SUCCESS ((void*)0)
#define SUCCESS 0

void *mythread(void *arg) {
	printf("thread: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());
	return RETVAL_SUCCESS;
}

int main() {
	pthread_t tid;
	int err;

	printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

	err = pthread_create(&tid, NULL, mythread, NULL);
	if (err != SUCCESS) {
	    printf("main: pthread_create() failed: %s\n", strerror(err));
		return EXIT_FAILURE;
	}

	pthread_exit(RETVAL_SUCCESS);
}

