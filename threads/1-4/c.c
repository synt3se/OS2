#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SUCCESS 0
#define EXECUTE_CLEANUP 1

#define STR "hello world"

void cleanup_handler(void* arg) {
    printf("cleanup: freeing block at %p\n", arg);
    free(arg);
}

void *thread_routine() {
    char* str = malloc(strlen(STR) + 1);
    if (str == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pthread_cleanup_push(cleanup_handler, str);
    strcpy(str, STR);

    while (1) {
        printf("thread: %s\n", str);
        sleep(1);
    }

    pthread_cleanup_pop(EXECUTE_CLEANUP);
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

    sleep(5);
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

