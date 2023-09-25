#include <stdio.h>
#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include "lock.h"
#include <unity.h>

void setUp(void) {}

void tearDown(void) {}

/*
Testing semaphore locks: do_loop should return error value (1), counter should not increment
*/
void test_loop_blocks(void)
{
    struct k_sem semaphore;
    struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
    k_sem_init(&semaphore, 1, 1);
    int counter = 0;

    k_sem_take(&semaphore, K_FOREVER);
    int result = do_loop(&timer, &semaphore, &counter, "test", K_MSEC(100));

    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(0, counter);
}

/*
Tests normal function of loop: do_loop returns normal value (0), counter increments one time 
*/
void test_loop_runs(void)
{
    struct k_sem semaphore;
    struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
    k_sem_init(&semaphore, 1, 1);
    int counter = 0;

    int result = do_loop(&timer, &semaphore, &counter, "test", K_MSEC(100));

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(1, counter);
}



int main(void)
{
    UNITY_BEGIN();
    // test main loop with semaphore hogged
    // test loop code with sempaphore hogged
    // test with it normal

    RUN_TEST(test_loop_blocks);
    RUN_TEST(test_loop_runs);

    return UNITY_END();
}