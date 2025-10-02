#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SUCCESS 0
#define STR "hello from thread"

void *thread_routine() {
    printf("thread: TID=%d\n", gettid());

    char *ret = malloc((strlen(STR) + 1) * sizeof(char));
    if (ret == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(ret, STR);
    return ((void*)ret);
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
    printf("thread returned: %s\n", (char*) res);

    free(res);
    return EXIT_SUCCESS;
}

