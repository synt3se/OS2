#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SUCCESS 0

struct mydata {
    int num;
    char *str;
};

void *thread_routine(void *arg) {
    struct mydata *data = (struct mydata *)arg;
    printf("thread [%d] - num=%d, str=%s\n", gettid(), data->num, data->str);
    free(data);
    return ((void*)0);
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int err;

    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    err = pthread_attr_init(&attr);
    if (err != SUCCESS) {
        printf("main: pthread_attr_init() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }
    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err != SUCCESS) {
        printf("main: pthread_attr_setdetachstate() failed: %s\n", strerror(err));
        pthread_attr_destroy(&attr);
        return EXIT_FAILURE;
    }

    struct mydata *data = (struct mydata *)malloc(sizeof(struct mydata));
    data->num = 13;
    data->str = "hello from thread";

    err = pthread_create(&tid, &attr, thread_routine, data);
    if (err != SUCCESS) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(data);
        return EXIT_FAILURE;
    }
    err = pthread_attr_destroy(&attr);
    if (err != SUCCESS) {
        printf("main: pthread_attr_destroy() failed: %s\n", strerror(err));
        return EXIT_FAILURE;
    }

    printf("main: end\n");
    pthread_exit(NULL);
}

