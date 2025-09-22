#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define NTHREADS 5

int global_var = 13;

void *mythread(void *arg) {
	int local_var = 1;
	static int static_var = 2;
	const int const_var = 3;

	printf("thread %ld:\n", (long)arg);
	printf("    PID=%d, PPID=%d\n", getpid(), getppid());
	printf("    gettid()=%d, pthread_self()=%lu\n", gettid(), pthread_self());

	printf("    local_var = %d, address  = %p\n", local_var, (void*)&local_var);
	printf("    static_var = %d, address  = %p\n", static_var, (void*)&static_var);
	printf("    const_var = %d, address  = %p\n", const_var, (void*)&const_var);
	printf("    global_var = %d, address  = %p\n", global_var, (void*)&global_var);
	return NULL;
}

int main() {
	pthread_t threads[NTHREADS];
	int err;

	printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

	for (int i = 0; i < NTHREADS; i++) {
		err = pthread_create(&threads[i], NULL, mythread, (void *)i);
		if (err) {
			printf("main: pthread_create() failed: %s\n", strerror(err));
			return -1;
		}
		printf("main: thread %d created, threads[i]=%lu\n", i, threads[i]);
	}

	pthread_exit(NULL);
}

