#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define SUCCESS 0

#define NUM 13
#define STR "hello from thread"

struct mydata {
    int num;
    char *str;
};

int init_attr_detached(pthread_attr_t *attr) {
    int err;

    err = pthread_attr_init(attr);
    if (err != SUCCESS) {
        perror("pthread_attr_init");
        return EXIT_FAILURE;
    }

    err = pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    if (err != SUCCESS) {
        perror("pthread_attr_setdetachstate");
        pthread_attr_destroy(attr);
        return EXIT_FAILURE;
    }

    return SUCCESS;
}

struct mydata *allocate_mydata(int num, const char *s) {
    if (s == NULL) {
        printf("allocate_mydata: s == NULL\n");
        return NULL;
    }

    struct mydata *data = (struct mydata *)malloc(sizeof(struct mydata));
    if (data == NULL) {
        printf("allocate_mydata: malloc failed\n");
        return NULL;
    }

    data->num = num;
    data->str = malloc(strlen(s) + 1);
    if (data->str == NULL) {
        printf("allocate_mydata: malloc failed\n");
        free(data);
        return NULL;
    }
    strcpy(data->str, s);
    return data;
}

void *thread_routine(void *arg) {
    struct mydata *data = (struct mydata *)arg;
    printf("thread [%d] - num=%d, str=%s\n", gettid(), data->num, data->str);
    free(data->str);
    free(data);
    return ((void*)0);
}

int main() {
    pthread_t tid;
    pthread_attr_t attr;
    int err;
    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    err = init_attr_detached(&attr);
    if (err != SUCCESS) {
        return EXIT_FAILURE;
    }

    struct mydata *data = allocate_mydata(NUM, STR);
    if (data == NULL) {
        pthread_attr_destroy(&attr);
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid, &attr, thread_routine, data);
    if (err != SUCCESS) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        pthread_attr_destroy(&attr);
        free(data->str);
        free(data);
        return EXIT_FAILURE;
    }
    err = pthread_attr_destroy(&attr);
    if (err != SUCCESS) {
        perror("pthread_attr_destroy");
        return EXIT_FAILURE;
    }

    printf("main: end\n");
    pthread_exit(NULL);
}

