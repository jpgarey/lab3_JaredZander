#include <stdio.h>
#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/printk.h>
#include "lock.h"

#define STACKSIZE 2000
#define SLEEPTIME 1000

struct k_thread coop_thread;
K_THREAD_STACK_DEFINE(coop_stack, STACKSIZE);

struct k_sem semaphore;
int counter;

void setUp(void) {}
void tearDown(void) {}

void test_thread_loop(void)
{
	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);

	while (1) {
        counter = counter + 1;
		printk("hello world from %s! Count %d\n", "thread", counter);
		k_timer_start(&timer, K_MSEC(SLEEPTIME), K_NO_WAIT);
		k_timer_status_sync(&timer);
	}
}

void test_main_loop()
{
    struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);

    k_thread_create(&coop_thread,
                    coop_stack,
                    STACKSIZE,
                    (k_thread_entry_t) test_thread_loop,
                    NULL,
                    NULL,
                    NULL,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);
    
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        counter = counter + 1;
		printk("hello world from %s! Count %d\n", "main", counter);
		k_timer_start(&timer, K_MSEC(SLEEPTIME), K_NO_WAIT);
		k_timer_status_sync(&timer);
    }
}

void test_main_loop()
{
    counter = 0;
    k_sem_init(&semaphore, 1, 1);
    k_sem_take(&semaphore, K_FOREVER);
}

int main(void)
{
    UNITY_BEGIN();
    // test main loop with semaphore hogged
    // test loop code with sempaphore hogged
    // test with it normal

    RUN_TEST();

    return UNITY_END();
}