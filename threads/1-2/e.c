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
    int err = pthread_detach(pthread_self());
    if (err != SUCCESS) {
        perror("pthread_detach");
        exit(EXIT_FAILURE);
    }
    return ((void*)0);
}

int main() {
    pthread_t tid;
    int err;
    int count_threads = 0;

    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    while (1) {
        err = pthread_create(&tid, NULL, thread_routine, NULL);
        if (err != SUCCESS) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            break;
        }

        // err = pthread_join(tid, NULL);
        // if (err != SUCCESS) {
        //     printf("main: pthread_join() failed: %s\n", strerror(err));
        //     return EXIT_FAILURE;
        // }
        ++count_threads;
    }

    printf("main: threads created %d\n", count_threads);
    return EXIT_SUCCESS;
}

