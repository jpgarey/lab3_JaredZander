#include "lock.h"
#define SLEEPTIME 5

/*  0 - normal return status 
    1 - error return status 

*/
int do_loop(struct k_timer *timer,
            struct k_sem *semaphore,
            int *counter,
            char *src,
            k_timeout_t timeout)
{
    if (k_sem_take(semaphore, timeout))
        return 1;
    {
        (*counter)++;
        printk("hello world from %s! Count %d\n", src, *counter);
    }
    k_sem_give(semaphore);
    k_timer_start(timer, K_MSEC(SLEEPTIME), K_NO_WAIT);
    k_timer_status_sync(timer);
    return 0;
}


/* void deadlock(void *l, void *r, void *c) */
void deadlock(struct k_sem *a, struct k_sem *b, int *counter)
{
    int id = *counter;
    printk("\tinside deadlock %d\n", id);
    (*counter)++;
    struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
    k_sem_take(a, K_FOREVER);
    {
        (*counter)++;
        printk("\tinside first lock %d\n", id);
        k_yield();
        printk("\tpost-yield %d\n", id);
        k_timer_start(&timer, K_MSEC(100), K_NO_WAIT);
        k_timer_status_sync(&timer);
        k_sem_take(b, K_FOREVER);
        {
            printk("\tinside second lock %d\n", id);
            (*counter)++;
        }
        k_sem_give(b);
    }
    k_sem_give(a);
}


void orphaned_lock(struct k_sem *semaphore, int *counter, int *sem_count)
{

    while (1) {
        printk("loop entered with value %d\n", (*counter));
        k_sem_take(semaphore, K_FOREVER);
        (*counter)++;
        printk("Counter inceremented, new value: %d\n", (*counter));
        
        // Exit while loop if counter is ODD, so we fail to release the semaphore
        if ((*counter) % 2) {
            printk("iteration broken, sem not given \n");
            continue;
        }
        printk("Loop ended with count %d\n", (*counter));
        // release semaphore
        sem_count = k_sem_count_get(semaphore);
        k_sem_give(semaphore);
    }
}