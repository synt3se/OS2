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

int global_var = 13;

void *mythread(void *arg) {
	int thread_num = (int)arg;

	int local_var = 1;
	static int static_var = 2;
	const int const_var = 3;

	printf("thread %d: PID=%d, PPID=%d, gettid()=%d, pthread_self()=%lu\n",
		thread_num, getpid(), getppid(), gettid(), pthread_self());

	printf("thread %d: local[%p], static[%p], const[%p], global[%p]\n", thread_num, (void*)&local_var,
		(void*)&static_var, (void*)&const_var, (void*)&global_var);

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
		printf("main: thread %d created, threads[thread_num]=%lu\n", thread_num, threads[thread_num]);
	}

	pthread_exit(RETVAL_SUCCESS);
}

