// si_threadpool_test.c

#include <stdio.h> // printf()

#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()
#include "si_logger.h" // si_logger_t
#include "si_signals.h"
#include "si_threadpool.h" // si_threadpool_t

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

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
	sleep(3);
	si_logger_debug(p_param, "Example task has stopped.");
	return NULL;
}

static void* looping_task(si_logger_t* p_param)
{
	si_logger_warning(p_param, "Drink Water!");
	sleep(12);
	return NULL;
}

static void* returns_value_task(int* p_num)
{
	if (NULL == p_num)
	{
		goto END;
	}
	const int divisor = 42u;
	while (divisor <= *p_num)
	{
		*p_num -= divisor;
	}
END:
	return p_num;
}

static void handle_signal(int signal)
{
	// NOP to make -Wpedantic happy.
	(void)signal;
	si_threadpool_stop(p_threadpool);
}

static void si_threadpool_test_run(void)
{
	const bool set_handler = set_exit_signal_handler(handle_signal);
	TEST_ASSERT_TRUE(set_handler);

	const size_t priority_count = 16u;
	p_threadpool = si_threadpool_new_1(priority_count);
	si_threadpool_start(p_threadpool);
	TEST_ASSERT_NOT_NULL(p_threadpool);
	si_logger_t* p_logger = si_logger_new_1(SI_LOGGER_ALL);
	TEST_ASSERT_NOT_NULL(p_logger);

	const size_t etask_id = si_threadpool_enqueue_4(
		p_threadpool, (p_task_f)example_task, p_logger, true
	);
	si_logger_debug(p_logger, "Example Task ID: %lu.", etask_id);
	TEST_ASSERT_NOT_EQUAL_size_t(SI_THREADPOOL_TASK_ID_INVALID, etask_id);

	const size_t ltask_id = si_threadpool_enqueue_5(
		p_threadpool, (p_task_f)looping_task, p_logger,
		false, (priority_count - 1u)
	);
	si_logger_debug(p_logger, "Looping Task first ID: %lu.", ltask_id);
	TEST_ASSERT_NOT_EQUAL_size_t(SI_THREADPOOL_TASK_ID_INVALID, ltask_id);
	
	int int_parameter = INT_MAX;
	const size_t rtask_id = si_threadpool_enqueue_5(
		p_threadpool, (p_task_f)returns_value_task, &int_parameter,
		true, (priority_count - 1u)
	);
	si_logger_debug(p_logger,
		"Returns Task ID: %lu. Original: %d.", rtask_id, int_parameter
	);
	TEST_ASSERT_NOT_EQUAL_size_t(SI_THREADPOOL_TASK_ID_INVALID, rtask_id);

	si_logger_info(p_logger, "Waiting for results.");
	void* const p_result = si_threadpool_await_results(p_threadpool, rtask_id);
	if(NULL == p_result)
	{
		// Threadpool was stopped/interupted before results were calculated.
		goto END;
	}
	si_logger_info(
		p_logger, "Got task results! New value: %d.", int_parameter
	);
	si_threadpool_await(&p_threadpool);

END:
	si_logger_info(p_logger, "No longer awaiting.");
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
	(void)printf("Begin testing of si_threadpool.\n");
	si_threadpool_test_all();
	(void)printf("End of si_threadpool testing.\n");
	return 0;
}
