#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;
	int err;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		err = pthread_spin_lock(&q->lock);
		if (err != SUCCESS) {
			printf("qmonitor: pthread_spin_lock failed: %s\n", strerror(err));
			continue;
		}
		queue_print_stats(q);
		pthread_spin_unlock(&q->lock);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (q == NULL) {
		printf("Cannot allocate memory for a queue\n");
		return NULL;
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

	err = pthread_spin_init(&q->lock, PTHREAD_PROCESS_PRIVATE);
	if (err != SUCCESS) {
		printf("queue_init: pthread_spin_init() failed: %s\n", strerror(err));
		free(q);
		return NULL;
	}

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err != SUCCESS) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		pthread_spin_destroy(&q->lock);
		free(q);
		return NULL;
	}

	return q;
}

void queue_destroy(queue_t *q) {
	if (q == NULL) return;

	int err = pthread_cancel(q->qmonitor_tid);
	if (err != SUCCESS) {
		printf("queue_destroy: pthread_cancel() failed: %s\n", strerror(err));
		// pthread_cancel has ESRCH as a pthread_join, continue
	}
	err = pthread_join(q->qmonitor_tid, NULL);
	if (err != SUCCESS) {
		printf("queue_destroy: pthread_join() failed: %s\n", strerror(err));
	}

	pthread_spin_destroy(&q->lock);

	while (q->first != NULL) {
		qnode_t *tmp = q->first->next;
		free(q->first);
		q->first = tmp;
	}
	free(q);
}

int queue_add(queue_t *q, int val) {
	if (q == NULL) return QERROR;

	int err = pthread_spin_lock(&q->lock);
	if (err != SUCCESS) {
		printf("queue_add: pthread_spin_lock() failed: %s\n", strerror(err));
		return QERROR;
	}

	q->add_attempts++;
	if (q->count == q->max_count) {
		pthread_spin_unlock(&q->lock);
		return QERROR;
	}

	qnode_t *new = malloc(sizeof(qnode_t));
	if (new == NULL) {
		printf("Cannot allocate memory for new node\n");
		pthread_spin_unlock(&q->lock);
		return QERROR;
	}

	new->val = val;
	new->next = NULL;
	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;
	q->add_count++;

	pthread_spin_unlock(&q->lock);
	return QSUCCESS;
}

int queue_get(queue_t *q, int *val) {
	if (q == NULL) return QERROR;
	int err = pthread_spin_lock(&q->lock);
	if (err != SUCCESS) {
		printf("queue_get: pthread_spin_lock() failed: %s\n", strerror(err));
		return QERROR;
	}

	q->get_attempts++;

	if (q->count == 0) {
		pthread_spin_unlock(&q->lock);
		return QERROR;
	}

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;

	free(tmp);
	q->count--;
	q->get_count++;

	pthread_spin_unlock(&q->lock);
	return QSUCCESS;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}

