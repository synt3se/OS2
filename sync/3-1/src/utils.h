#pragma once

#include "common.h"
#include <dirent.h>

int get_realpath(const char *path, char *out);

int build_path(char *dst, size_t size, const char *dir, const char *name);

int is_dir(const char *path);

int open_with_retry(const char *path, int flags, mode_t mode);
DIR *opendir_with_retry(const char *path);

size_t get_dirent_size(const char *dirpath);

