#include "utils.h"
#include <fcntl.h>

int get_realpath(const char* relative_path, char* full_path) {
    char* resolved_path = realpath(relative_path, full_path);
    if (resolved_path == NULL) {
        fprintf(stderr, "Error resolving path: %s", relative_path);
        return ERROR;
    }
    return SUCCESS;
}

int build_path(char *dst, size_t size, const char *dir, const char *name) {
    int len = snprintf(dst, size, "%s/%s", dir, name);
    if (len < 0 || (size_t)len >= size) {
        fprintf(stderr, "Error creating path: %s/%s\n", dir, name);
        return ERROR;
    }
    return SUCCESS;
}

int is_dir(const char *path) {
    struct stat stat_buf;
    int err = lstat(path, &stat_buf);
    if (err != SUCCESS) {
        fprintf(stderr, "Error stating path: %s\n", path);
        return ERROR;
    }
    if (!S_ISDIR(stat_buf.st_mode)) {
        fprintf(stderr, "%s is not a directory\n", path);
        return ERROR;
    }
    return SUCCESS;
}

int open_with_retry(const char *path, int flags, mode_t mode) {
    int fd;
    for (int i = 0; i < MAX_RETRIES; i++) {
        fd = open(path, flags, mode);
        if (fd != ERROR) return fd;
        if (errno != EMFILE) {
            fprintf(stderr, "Error opening file: %s\n", path);
            return ERROR;
        }
        sleep(1);
    }
    fprintf(stderr, "open '%s': max retries exceeded\n", path);
    return ERROR;
}

DIR *opendir_with_retry(const char *path) {
    DIR *dir;
    for (int i = 0; i < MAX_RETRIES; i++) {
        dir = opendir(path);
        if (dir != NULL) return dir;
        if (errno != EMFILE) {
            fprintf(stderr, "Error opening directory: %s\n", path);
            return NULL;
        }
        sleep(1);
    }
    fprintf(stderr, "opendir '%s': max retries exceeded\n", path);
    return NULL;
}
