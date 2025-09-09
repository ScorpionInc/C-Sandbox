// si_threadpool_test.c

#include <stdio.h> // printf()

#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()
#include "si_logger.h" // si_logger_t
#include "si_signals.h"
#include "si_threadpool.h" // si_threadpool_t

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

si_threadpool_t* p_threadpool = NULL;

static void si_threadpool_test_init(void)
{
	const size_t priority_count = 16u;
	si_threadpool_t* p_threadpool = si_threadpool_new_1(priority_count);
	TEST_ASSERT_NOT_NULL(p_threadpool);

	si_threadpool_destroy(&p_threadpool);
	TEST_ASSERT_NULL(p_threadpool);
}

static void* example_task(si_logger_t* p_param)
{
	si_logger_debug(p_param, "Example task has started.");
	// Do work
	si_logger_debug(p_param, "Example task has stopped.");
	return NULL;
}

static void* looping_task(si_logger_t* p_param)
{
	si_logger_warning(p_param, "DRINK WATER!");
	sleep(60);
	return NULL;
}

static void handle_signal(int signal)
{
	if(NULL != p_threadpool)
	{
		si_threadpool_destroy(&p_threadpool);
	}
}

static void si_threadpool_test_run(void)
{
	const bool set_handler = set_exit_signal_handler(handle_signal);
	TEST_ASSERT_TRUE(set_handler);

	const size_t priority_count = 16u;
	p_threadpool = si_threadpool_new_1(priority_count);
	si_threadpool_start(p_threadpool);
	TEST_ASSERT_NOT_NULL(p_threadpool);
	si_logger_t* p_logger = si_logger_new_2(stdout, SI_LOGGER_ALL);
	TEST_ASSERT_NOT_NULL(p_logger);

	const size_t etask_id = si_threadpool_enqueue_4(
		p_threadpool, (p_task_f)example_task, p_logger, true
	);
	printf("Example Task ID: %lu.\n", etask_id);
	TEST_ASSERT_NOT_EQUAL_size_t(SI_THREADPOOL_TASK_ID_INVALID, etask_id);

	const size_t ltask_id = si_threadpool_enqueue_5(
		p_threadpool, (p_task_f)looping_task, p_logger,
		false, (priority_count - 1u)
	);
	printf("Looping Task first ID: %lu.\n", ltask_id);
	TEST_ASSERT_NOT_EQUAL_size_t(SI_THREADPOOL_TASK_ID_INVALID, ltask_id);
	
	si_threadpool_await(&p_threadpool);

	si_logger_destroy(&p_logger);
	TEST_ASSERT_NULL(p_logger);
	si_threadpool_destroy(&p_threadpool);
	TEST_ASSERT_NULL(p_threadpool);
}

static void si_threadpool_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_threadpool_test_init);
	RUN_TEST(si_threadpool_test_run);
	UNITY_END();
}

int main(void)
{
	printf("Begin testing of si_threadpool.\n");
	printf("si_threadpool_t size: %lu.\n", sizeof(si_threadpool_t));
	printf("si_parray_t size: %lu.\n", sizeof(si_parray_t));
	printf("si_priority_queue_t size: %lu.\n", sizeof(si_priority_queue_t));
	si_threadpool_test_all();
	printf("End of si_threadpool testing.\n");
	return 0;
}