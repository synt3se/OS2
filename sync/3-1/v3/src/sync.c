#include "sync.h"
#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

static pthread_mutex_t g_mutex;
static pthread_cond_t g_cond;
static int g_active = 0;
static int g_failed = 0;

int sync_init() {
    int err = pthread_mutex_init(&g_mutex, NULL);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(err));
        return ERROR;
    }
    err = pthread_cond_init(&g_cond, NULL);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(err));
        pthread_mutex_destroy(&g_mutex);
        return ERROR;
    }
    g_active = 0;
    g_failed = 0;
    return SUCCESS;
}

void sync_destroy() {
    int err = pthread_cond_destroy(&g_cond);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_cond_destroy: %s\n", strerror(err));
    }
    err = pthread_mutex_destroy(&g_mutex);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_destroy: %s\n", strerror(err));
    }
}

void sync_thread_start() {
    int err = pthread_mutex_lock(&g_mutex);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return;
    }
    g_active++;
    err = pthread_mutex_unlock(&g_mutex);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
    }
}

void sync_thread_end(int ret) {
    pthread_mutex_lock(&g_mutex);
    if (ret != SUCCESS) g_failed = 1;
    g_active--;
    if (g_active == 0) {
        pthread_cond_signal(&g_cond);
    }
    pthread_mutex_unlock(&g_mutex);
}

void sync_wait_all() {
    int err = pthread_mutex_lock(&g_mutex);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(err));
        return;
    }
    while (g_active > 0) {
        pthread_cond_wait(&g_cond, &g_mutex);
    }
    err = pthread_mutex_unlock(&g_mutex);
    if (err != SUCCESS) {
        fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(err));
    }
}

int sync_has_failed() {
    return g_failed;
}
