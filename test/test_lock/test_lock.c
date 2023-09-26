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

/*********************** Deadlock testing ************************/
#define STACK_SIZE 500
K_THREAD_STACK_DEFINE(left_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(right_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(sup_stack, STACK_SIZE);
struct k_thread left_thread, right_thread, sup_thread;
struct k_sem left, right;

int counter0 = 2;
int counter1 = 1;

void deadlock_supervisor(void)
{
    k_sem_init(&left, 1, 1);
    // Right starts locked,
    k_sem_init(&right, 1, 1);
    printf("- Creating threads\n");
    // Create threads with 2 shared semaphores (left and right), and their own counter (for status).
    // Switch the order of the 2 semaphores so that a deadlock happens.
    k_tid_t l = k_thread_create(&left_thread, left_stack, STACK_SIZE,
                                (k_thread_entry_t) deadlock,
                                &left, &right, &counter0, // 3 parameters p1, p2, p3
                                K_PRIO_COOP(6),
                                0,
                                K_NO_WAIT);
    k_tid_t r = k_thread_create(&right_thread, right_stack, STACK_SIZE,
                                (k_thread_entry_t) deadlock,
                                &right, &left, &counter1, // 3 parameters p1, p2, p3
                                K_PRIO_COOP(6),
                                0,
                                K_NO_WAIT);
    printf("- Created threads\n");

    // Attempt to join threads to this current thread. They should fail, so output how long we waited.
    int left_done = k_thread_join(l, K_MSEC(1000));
    printf("- Waited left %d\n", left_done);
    int right_done = k_thread_join(r, K_MSEC(1000));
    printf("- Waited right %d\n", right_done);

    // Test for error EAGIN (no more processes), just to show that the thread join failed.
    TEST_ASSERT_EQUAL_INT(-EAGAIN, left_done);
    TEST_ASSERT_EQUAL_INT(-EAGAIN, right_done);
    
    printf("- Killing threads\n");
    k_thread_abort(l);
    k_thread_abort(r);
    printf("- Killed threads\n");
}

void test_deadlock(void)
{
    printf("starting deadlock test\n");
    k_thread_create(&sup_thread,
                    sup_stack,
                    STACK_SIZE,
                    (k_thread_entry_t) deadlock_supervisor,
                    NULL, NULL, NULL, // optional output p1, p2, and p3
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);
    
    k_thread_join(&sup_thread, K_FOREVER); // wait until the supervisor thread is done
    printf("Finished supervisor\n");
    
    TEST_ASSERT_EQUAL_INT(0, k_sem_count_get(&left));
    TEST_ASSERT_EQUAL_INT(0, k_sem_count_get(&right));
    // Each counter should only be incremented twice
    // Counters are initialized to values 2 and 1.
    TEST_ASSERT_EQUAL_INT(4, counter0);
    TEST_ASSERT_EQUAL_INT(3, counter1);
}


int main(void)
{
    UNITY_BEGIN();
    // test main loop with semaphore hogged
    // test loop code with sempaphore hogged
    // test with it normal

    RUN_TEST(test_loop_blocks);
    RUN_TEST(test_loop_runs);
    RUN_TEST(test_deadlock);

    return UNITY_END();