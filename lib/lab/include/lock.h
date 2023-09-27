#pragma once

#include <zephyr.h>
#include <kernel.h>
#include <arch/cpu.h>
#include <sys/printk.h>

int do_loop(struct k_timer *timer,
            struct k_sem *semaphore,
            int *counter,
            char *src,
            k_timeout_t timeout);

void deadlock(struct k_sem *a, struct k_sem *b, int *counter);
void orphaned_lock(struct k_sem *semaphore, int *counter);