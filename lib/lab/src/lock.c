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

