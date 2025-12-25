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

    int err = create_task(copy_dir, args->src_path, args->dst_path);
    thread_args_free(args);
    if (err != SUCCESS) {
        return EXIT_FAILURE;
    }

    pthread_exit(NULL);
}