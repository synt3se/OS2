#pragma once

int sync_init(void);

void sync_destroy(void);

void sync_thread_start(void);

void sync_thread_end(int failed);

void sync_wait_all(void);

int sync_has_failed(void);

