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
	size_t thread_num = (size_t)arg;

	int local_var = 1;

	printf("thread %zu before changes: local[%p]=%d, global[%p]=%d\n", thread_num,
		(void*)&local_var, local_var, (void*)&global_var, global_var);

	++local_var;
	--global_var;

	printf("thread %zu after changes: local[%p]=%d, global[%p]=%d\n", thread_num,
		(void*)&local_var, local_var, (void*)&global_var, global_var);

	return RETVAL_SUCCESS;
}

int main() {
	pthread_t threads[NTHREADS];
	int err;

	printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

	for (size_t thread_num = 0; thread_num < NTHREADS; thread_num++) {
		err = pthread_create(&threads[thread_num], NULL, mythread, (void*)thread_num);
		if (err != SUCCESS) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return EXIT_FAILURE;
		}
		printf("main: thread %zu created, threads[thread_num]=%lu\n",
			thread_num, threads[thread_num]);
	}

	printf("main: cat /proc/%d/maps\n", getpid());
	printf("main: Press any key to exit...\n");
	getchar();

	pthread_exit(RETVAL_SUCCESS);
}

