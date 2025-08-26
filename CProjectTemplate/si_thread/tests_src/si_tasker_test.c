#include "unity.h"

#include "si_logger.h"
#include "si_signals.h"
#include "si_tasker.h"

#include <stdio.h>

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

si_logger_t* p_logger = NULL;
si_tasker_t* p_tasker = NULL;

/** Doxygen
 * @brief Handles graceful exit from an exit signal
 *
 * @param signal Signal code value received.
 */
static void handle_exit(const int signal)
{
	if(NULL != p_tasker)
	{
		si_tasker_free(p_tasker);
		free(p_tasker);
		p_tasker = NULL;
	}
	if(NULL != p_logger)
	{
		si_logger_destroy(&p_logger);
	}
	exit(0);
}

/** Doxygen
 * @brief High priority example task
 */
static void task_high(si_singular_list_t* optional_arguments, si_array_t* stores_results)
{
	si_logger_critical(p_logger, "ALERT! IMPORTANT MESSAGE!");
	sleep(5);
}

/** Doxygen
 * @brief Example task
 */
static void task_a(si_singular_list_t* optional_arguments, si_array_t* stores_results)
{
	const int aaa = 321;
	const int bbb = 987;
	int ccc = (aaa * bbb) % (aaa + bbb);
	si_logger_debug(p_logger, "Task completed calculated: %d.", ccc);
	sleep(1);
	return;
}

void si_tasker_test_main(void)
{
	set_exit_signal_handler(handle_exit);

	p_logger = si_logger_new();
	p_logger->logging_level = SI_LOGGER_ALL;

	p_tasker = si_tasker_new();
	si_task_t* p_task = si_task_new_2(task_a, SI_TASK_LOOPS);
	if(NULL == p_task)
	{
		goto END;
	}
	si_tasker_enqueue_task(p_tasker, p_task);
	si_tasker_start_2(p_tasker, 4u);
	si_task_t* p_task_h = si_task_new_2(task_high, SI_TASK_LOOPS | SI_TASK_PRIORITY_MAX);
	if(NULL == p_task_h)
	{
		goto END;
	}
	si_tasker_enqueue_task(p_tasker, p_task_h);
	si_tasker_await(p_tasker);

	si_task_free(p_task);
	free(p_task);
	p_task = NULL;
	si_task_free(p_task_h);
	free(p_task_h);
	p_task_h = NULL;
	si_logger_destroy(&p_logger);
END:
	return;
}

void si_tasker_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_tasker_test_main);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Begin testing of si_tasker.\n");
	si_tasker_test_all();
	printf("End of si_tasker testing.\n");
	return 0;
}
