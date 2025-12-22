#include "utils.h"
#include <fcntl.h>

int get_realpath(const char* relative_path, char* full_path) {
    char* resolved_path = realpath(relative_path, full_path);
    if (resolved_path == NULL) {
        fprintf(stderr, "Error resolving path: %s", relative_path);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int build_path(char *dst, size_t size, const char *dir, const char *name) {
    int len = snprintf(dst, size, "%s/%s", dir, name);
    if (len < 0 || (size_t)len >= size) {
        fprintf(stderr, "Error creating path: %s/%s\n", dir, name);
        return -1;
    }
    return 0;
}

int is_dir(const char *path) {
    struct stat stat_buf;
    int err = lstat(path, &stat_buf);
    if (err != 0) {
        fprintf(stderr, "Error stating path: %s\n", path);
        return -1;
    }
    if (!S_ISDIR(stat_buf.st_mode)) {
        fprintf(stderr, "%s is not a directory\n", path);
        return -1;
    }
    return 0;
}

int open_with_retry(const char *path, int flags, mode_t mode) {
    int fd;
    for (int i = 0; i < MAX_RETRIES; i++) {
        fd = open(path, flags, mode);
        if (fd != -1) return fd;
        if (errno != EMFILE) {
            fprintf(stderr, "Error opening file: %s\n", path);
            return -1;
        }
        sleep(1);
    }
    fprintf(stderr, "open '%s': max retries exceeded\n", path);
    return -1;
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

size_t get_dirent_size(const char *dirpath) {
    long name_max = pathconf(dirpath, _PC_NAME_MAX);
    if (name_max == -1) name_max = 255;
    return sizeof(struct dirent) + (size_t)name_max + 1;
}
