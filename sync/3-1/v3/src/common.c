#include "common.h"

void thread_args_free(thread_args_t *args) {
    if (args == NULL) return;
    free(args->src_path);
    free(args->dst_path);
    free(args);
}
