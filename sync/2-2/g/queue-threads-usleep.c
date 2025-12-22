#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>

#include "queue.h"

#define RED "\033[41m"
#define NOCOLOR "\033[0m"
#define QUEUE_SIZE 100000
#define WORK_TIME 15

void set_cpu(int n) {
	int err;
	cpu_set_t cpuset;
	pthread_t tid = pthread_self();

	CPU_ZERO(&cpuset);
	CPU_SET(n, &cpuset);

	err = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
	if (err != SUCCESS) {
		printf("set_cpu: pthread_setaffinity failed for cpu %d\n", n);
		return;
	}

	printf("set_cpu: set cpu %d\n", n);
}

void *reader(void *arg) {
	int expected = 0;
	queue_t *q = (queue_t *)arg;
	printf("reader [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(1);

	while (1) {
		pthread_testcancel();
		int val = -1;
		int ok = queue_get(q, &val);
		if (ok == QERROR) {
			continue;
		}

		if (expected != val)
			printf(RED"ERROR: get value is %d but expected - %d" NOCOLOR "\n", val, expected);

		expected = val + 1;
	}

	return NULL;
}

void *writer(void *arg) {
	int i = 0;
	queue_t *q = (queue_t *)arg;
	printf("writer [%d %d %d]\n", getpid(), getppid(), gettid());

	set_cpu(2);

	while (1) {
		usleep(1);
		int ok = queue_add(q, i);
		if (ok == QERROR) {
			continue;
		}
		i++;
	}

	return NULL;
}

int cancel_join_thread(pthread_t tid) {
	int err = pthread_cancel(tid);
	if (err != SUCCESS) {
		printf("cancel_join_thread: pthread_cancel failed %s\n", strerror(err));
		return ERROR;
	}
	err = pthread_join(tid, NULL);
	if (err != SUCCESS) {
		printf("cancel_join_thread: pthread_join failed %s\n", strerror(err));
		return ERROR;
	}
	return SUCCESS;
}

int main() {
	pthread_t reader_tid, writer_tid;
	queue_t *q;
	int err;
	printf("main [%d %d %d]\n", getpid(), getppid(), gettid());

	q = queue_init(QUEUE_SIZE);
	if (q == NULL) {
		printf("queue_init failed\n");
		return ERROR;
	}

	err = pthread_create(&reader_tid, NULL, reader, q);
	if (err != SUCCESS) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return ERROR;
	}
	sched_yield();
	err = pthread_create(&writer_tid, NULL, writer, q);
	if (err != SUCCESS) {
		printf("main: pthread_create() failed: %s\n", strerror(err));
		return ERROR;
	}

	sleep(WORK_TIME);

	err = cancel_join_thread(reader_tid);
	if (err == ERROR) {
		return ERROR;
	}
	err = cancel_join_thread(writer_tid);
	if (err == ERROR) {
		return ERROR;
	}
	printf("main: reader and writer joined success\n");

	queue_destroy(q);
	return 0;
}
