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
#define NTHREADS 5

void *mythread(void *arg) {
	printf("thread %d: PID=%d, PPID=%d, TID=%d\n", (int)arg, getpid(), getppid(), gettid());
	return RETVAL_SUCCESS;
}

int main() {
	pthread_t threads[NTHREADS];
	int err;

	printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

	for (int thread_num = 0; thread_num < NTHREADS; thread_num++) {
		err = pthread_create(&threads[thread_num], NULL, mythread, (void*)thread_num);
		if (err != SUCCESS) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return EXIT_FAILURE;
		}
	}

	pthread_exit(RETVAL_SUCCESS);
}

