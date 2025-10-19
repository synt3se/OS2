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
    int err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (err != 0) {
        printf("pthread_setcanceltype() failed: %s\n", strerror(err));
    }
    int j = 0;
    while (1) {
        ++j;
    }
    return ((void*)0);
}

int main() {
    pthread_t tid;
    void *res;
    int err;

    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    err = pthread_create(&tid, NULL, thread_routine, NULL);
    if (err != SUCCESS) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    sleep(3);
    err = pthread_cancel(tid);
    if (err != SUCCESS) {
        printf("main: pthread_cancel() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    err = pthread_join(tid, &res);
    if (err != SUCCESS) {
        printf("main: pthread_join() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    if (res != PTHREAD_CANCELED) {
        printf("main: thread was not canceled (?)\n");
        return EXIT_FAILURE;
    }
    printf("main: thread was canceled\n");

    return EXIT_SUCCESS;
}

