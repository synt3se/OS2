#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SUCCESS 0

void *thread_routine() {
    printf("thread: TID=%d\n", gettid());
    return ((void*)0);
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int err;
    int count_threads = 0;

    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    err = pthread_attr_init(&attr);
    if (err != SUCCESS) {
        perror("pthread_attr_init");
        return EXIT_FAILURE;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err != SUCCESS) {
        perror("pthread_attr_setdetachstate");
        pthread_attr_destroy(&attr);
        return EXIT_FAILURE;
    }

    while (1) {
        err = pthread_create(&tid, &attr, thread_routine, NULL);
        if (err != SUCCESS) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            break;
        }
        ++count_threads;
    }

    printf("main: threads created %d\n", count_threads);
    err = pthread_attr_destroy(&attr);
    if (err != SUCCESS) {
        perror("pthread_attr_destroy");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

