#define _GNU_SOURCE
#include <pthread.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;
	if (q == NULL) return NULL;
	int err;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		err = pthread_mutex_lock(&q->lock);
		if (err != SUCCESS) {
			printf("qmonitor: pthread_mutex_lock failed: %s\n", strerror(err));
			break;
		}
		queue_print_stats(q);
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS) {
			printf("qmonitor: pthread_mutex_unlock failed: %s\n", strerror(err));
			break;
		}
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
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

	int err = pthread_mutex_init(&q->lock, NULL);
	if (err != SUCCESS) {
		printf("queue_init: pthread_mutex_init failed: %s\n", strerror(err));
		free(q);
		return NULL;;
	}
	err = pthread_cond_init(&q->cond, NULL);
	if (err != SUCCESS) {
		printf("queue_init: pthread_cond_init failed: %s\n", strerror(err));
		err = pthread_mutex_destroy(&q->lock);
		if (err != SUCCESS) printf("queue_init: pthread_mutex_destroy failed: %s\n", strerror(err));
		free(q);
		return NULL;;
	}
	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err != SUCCESS) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		err = pthread_cond_destroy(&q->cond);
		if (err != SUCCESS) printf("queue_init: pthread_cond_destroy failed: %s\n", strerror(err));
		err = pthread_mutex_destroy(&q->lock);
		if (err != SUCCESS) printf("queue_init: pthread_mutex_destroy failed: %s\n", strerror(err));
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

	err = pthread_mutex_destroy(&q->lock);
	if (err != SUCCESS)
		printf("queue_destroy: pthread_mutex_destroy() failed: %s\n", strerror(err));
	err = pthread_cond_destroy(&q->cond);
	if (err != SUCCESS)
		printf("queue_destroy: pthread_cond_destroy() failed: %s\n", strerror(err));

	while (q->first != NULL) {
		qnode_t *tmp = q->first->next;
		free(q->first);
		q->first = tmp;
	}
	free(q);
}

int queue_add(queue_t *q, int val) {
	if (q == NULL) return QERROR;

	qnode_t *new = malloc(sizeof(qnode_t));
	if (new == NULL) {
		printf("Cannot allocate memory for new node\n");
		return QERROR;
	}

	int err = pthread_mutex_lock(&q->lock);
	if (err != SUCCESS) {
		printf("queue_add: pthread_mutex_lock() failed: %s\n", strerror(err));
		free(new);
		return QERROR;
	}
	err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (err != SUCCESS) {
		printf("queue_add: pthread_setcancelstate() failed: %s\n", strerror(err));
		free(new);
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS) printf("queue_add: pthread_mutex_unlock() failed: %s\n", strerror(err));
		return QERROR;
	}

	q->add_attempts++;
	while (q->count == q->max_count) {
		pthread_cond_wait(&q->cond, &q->lock);
	}

	err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (err != SUCCESS) {
		printf("queue_add: pthread_setcancelstate() failed: %s\n", strerror(err));
		free(new);
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS) printf("queue_add: pthread_mutex_unlock() failed: %s\n", strerror(err));
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

	pthread_cond_broadcast(&q->cond);
	err = pthread_mutex_unlock(&q->lock);
	if (err != SUCCESS)
		printf("queue_add: pthread_mutex_unlock failed: %s\n", strerror(err));
	return QSUCCESS;
}

int queue_get(queue_t *q, int *val) {
	if (q == NULL) return QERROR;

	q->get_attempts++;
	int err = pthread_mutex_lock(&q->lock);
	if (err != SUCCESS) {
		printf("queue_get: pthread_mutex_lock failed: %s\n", strerror(err));
		return QERROR;
	}
	err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
	if (err != SUCCESS) {
		printf("queue_get: pthread_setcancelstate failed: %s\n", strerror(err));
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS)
			printf("queue_get: pthread_mutex_unlock failed: %s\n", strerror(err));
		return QERROR;
	}
	while (q->count == 0) {
		pthread_cond_wait(&q->cond, &q->lock);
	}
	err = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (err != SUCCESS) {
		printf("queue_get: pthread_setcancelstate() failed: %s\n", strerror(err));
		err = pthread_mutex_unlock(&q->lock);
		if (err != SUCCESS)
			printf("queue_get: pthread_mutex_unlock failed: %s\n", strerror(err));
		return QERROR;
	}

	qnode_t *tmp = q->first;
	*val = tmp->val;
	q->first = q->first->next;
	if (q->first == NULL) q->last = NULL;
	free(tmp);
	q->count--;
	q->get_count++;

	pthread_cond_broadcast(&q->cond);
	err = pthread_mutex_unlock(&q->lock);
	if (err != SUCCESS)
		printf("queue_add: pthread_mutex_unlock failed: %s\n", strerror(err));
	return QSUCCESS;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}

