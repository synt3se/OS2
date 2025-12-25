#pragma once

#include "common.h"

thread_args_t *init_args(const char *src, const char *dst);

int create_task(void *(*routine)(void *), const char *src, const char *dst);

void *copy_file(void *arg);

void *copy_dir(void *arg);

