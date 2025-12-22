#pragma once

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <pthread.h>

#define BUFFER_SIZE 4096
#define MAX_RETRIES 10

typedef struct {
    char *src_path;
    char *dst_path;
} thread_args_t;

void thread_args_free(thread_args_t *args);

