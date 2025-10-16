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

struct mydata *init_mydata(int num, const char *s) {
    if (s == NULL) {
        printf("allocate_mydata: s == NULL\n");
        return NULL;
    }

    struct mydata *data = (struct mydata *)malloc(sizeof(struct mydata));
    if (data == NULL) {
        perror("malloc struct mydata");
        return NULL;
    }

    data->num = num;
    data->str = malloc((strlen(s) + 1) * sizeof(char));
    if (data->str == NULL) {
        perror("malloc str");
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
    int err;
    printf("main: PID=%d, PPID=%d, TID=%d\n", getpid(), getppid(), gettid());

    struct mydata *data = init_mydata(NUM, STR);
    if (data == NULL) {
        return EXIT_FAILURE;
    }

    err = pthread_create(&tid, NULL, thread_routine, data);
    if (err != SUCCESS) {
        printf("main: pthread_create() failed: %s\n", strerror(err));
        free(data->str);
        free(data);
        return EXIT_FAILURE;
    }

    printf("main: end\n");
    pthread_exit(NULL);
}

