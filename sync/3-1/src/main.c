#include "common.h"
#include "copier.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src_dir> <dst_dir>\n", argv[0]);
        return EXIT_FAILURE;
    }

    thread_args_t *args = init_args(argv[1], argv[2]);
    if (args == NULL) {
        return EXIT_FAILURE;
    }

    pthread_attr_t attr;
    int err = pthread_attr_init(&attr);
    if (err != 0) {
        fprintf(stderr, "pthread_attr_init: %s\n", strerror(err));
        thread_args_free(args);
        return EXIT_FAILURE;
    }

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t tid;
    err = pthread_create(&tid, &attr, copy_dir, args);
    pthread_attr_destroy(&attr);

    if (err != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(err));
        thread_args_free(args);
        return EXIT_FAILURE;
    }

    pthread_exit(NULL);
}