#include "copier.h"
#include "utils.h"
#include <fcntl.h>
#include <dirent.h>

thread_args_t *init_args(const char *src, const char *dst) {
    char src_path[PATH_MAX];
    char dst_dir[PATH_MAX];
    int err = get_realpath(src, src_path);
    if (err != 0) return NULL;
    err = get_realpath(dst, dst_dir);
    if (err != 0) return NULL;

    err = is_dir(src_path);
    if (err != 0) return NULL;
    err = is_dir(dst_dir);
    if (err != 0) return NULL;

    const char *src_name = strrchr(src_path, '/');
    src_name = (src_name != NULL) ? src_name + 1 : src_path;

    char dst_path[PATH_MAX];
    int len = snprintf(dst_path, PATH_MAX, "%s/%s", dst_dir, src_name);
    if (len < 0 || len >= PATH_MAX) {
        fprintf(stderr, "Error creating path from %s", dst_dir);
        return NULL;
    }

    thread_args_t *args = malloc(sizeof(thread_args_t));
    if (args == NULL) {
        perror("malloc");
        return NULL;
    }
    args->src_path = strdup(src_path);
    args->dst_path = strdup(dst_path);
    if (args->src_path == NULL || args->dst_path == NULL) {
        perror("strdup");
        thread_args_free(args);
        return NULL;
    }
    return args;
}

int create_task(void *(*routine)(void *), const char *src, const char *dst) {
    thread_args_t *args = malloc(sizeof(thread_args_t));
    if (args == NULL) {
        perror("malloc");
        return -1;
    }

    args->src_path = strdup(src);
    args->dst_path = strdup(dst);
    if (args->src_path == NULL || args->dst_path == NULL) {
        perror("strdup");
        thread_args_free(args);
        return -1;
    }

    pthread_attr_t attr;
    int err = pthread_attr_init(&attr);
    if (err != 0) {
        fprintf(stderr, "pthread_attr_init: %s\n", strerror(err));
        thread_args_free(args);
        return -1;
    }

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    err = pthread_create(&thread, &attr, routine, args);
    pthread_attr_destroy(&attr);

    if (err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        thread_args_free(args);
        return -1;
    }

    return 0;
}

int process_entry(const char *src_dir, const char *dst_dir, const char *name) {
    char src_path[PATH_MAX];
    char dst_path[PATH_MAX];
    struct stat stat_buf;
    int err = build_path(src_path, sizeof(src_path), src_dir, name);
    if (err != 0) return -1;
    err = build_path(dst_path, sizeof(dst_path), dst_dir, name);
    if (err != 0) return -1;
    err = lstat(src_path, &stat_buf);
    if (err != 0) {
        fprintf(stderr, "Error stating path: %s\n", src_path);
        return -1;
    }

    if (S_ISDIR(stat_buf.st_mode)) {
        return create_task(copy_dir, src_path, dst_path);
    }
    if (S_ISREG(stat_buf.st_mode)) {
        return create_task(copy_file, src_path, dst_path);
    }
    return 0;
}

void *copy_file(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    char buffer[BUFFER_SIZE];
    struct stat src_stat;
    int src_fd = -1, dst_fd = -1;
    int err = lstat(args->src_path, &src_stat);
    if (err != 0) {
        fprintf(stderr, "Error stating path: %s\n", args->src_path);
        goto cleanup;
    }

    src_fd = open_with_retry(args->src_path, O_RDONLY, 0);
    if (src_fd == -1) goto cleanup;
    dst_fd = open_with_retry(args->dst_path, O_WRONLY | O_CREAT | O_TRUNC, src_stat.st_mode);
    if (dst_fd == -1) goto cleanup;

    ssize_t bytes_read = read(src_fd, buffer, BUFFER_SIZE);
    while (bytes_read > 0) {
        char *ptr = buffer;
        ssize_t remaining = bytes_read;
        while (remaining > 0) {
            ssize_t written = write(dst_fd, ptr, (size_t)remaining);
            if (written == -1) {
                fprintf(stderr, "Error writing to file: %s\n", args->dst_path);
                goto cleanup;
            }
            ptr += written;
            remaining -= written;
        }
        bytes_read = read(src_fd, buffer, BUFFER_SIZE);
    }
    if (bytes_read == -1) {
        fprintf(stderr, "Error reading from file: %s\n", args->dst_path);
    }

    cleanup:
        if (src_fd != -1) close(src_fd);
    if (dst_fd != -1) close(dst_fd);
    thread_args_free(args);
    return NULL;
}

void *copy_dir(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    DIR *dir = NULL;
    struct stat src_stat;

    int err = lstat(args->src_path, &src_stat);
    if (err != 0) {
        fprintf(stderr, "Error stating path: %s\n", args->src_path);
        goto cleanup;
    }

    err = mkdir(args->dst_path, src_stat.st_mode);
    if (err != 0 && errno != EEXIST) {
        fprintf(stderr, "Error creating directory: %s\n", args->dst_path);
        goto cleanup;
    }

    dir = opendir_with_retry(args->src_path);
    if (dir == NULL) goto cleanup;

    struct dirent *entry;
    while (1) {
        entry = readdir(dir);
        if (entry == NULL) break;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        process_entry(args->src_path, args->dst_path, entry->d_name);
    }

    cleanup:
    if (dir != NULL) closedir(dir);
    thread_args_free(args);
    return NULL;
}
