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
    void* res;
    int err;

    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, thread_routine, NULL);
    if (err != SUCCESS) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_join(tid, &res);
    if (err != SUCCESS) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }
    printf("thread returned %ld\n", (size_t) res);

    return EXIT_SUCCESS;
}

