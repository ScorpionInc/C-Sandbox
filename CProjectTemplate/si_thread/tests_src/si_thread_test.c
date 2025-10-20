// si_thread_test.c

#include "si_thread.h"
#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()

#include <stdio.h> // printf()

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Local function for testing si_thread_t.
 * 
 * @param p_void Pointer value passed into thread function.
 */
static void* test_runner(void* p_void)
{
	int* p_value = NULL;
	if (NULL == p_void)
	{
		goto END;
	}
	p_value = (int*)p_void;
	// Sleep until it's time to die :)
	sleep(10);
END:
	return (void*)p_value;
}

/** Doxygen
 * @brief Runs main unit test for si_thread_t.
 */
static void si_thread_test_main(void)
{
	size_t core_count = si_cpu_core_count();
	printf("Core count: %lu.\n", core_count);
	TEST_ASSERT_GREATER_THAN_size_t(0u, core_count);

	int value = 0;
	si_thread_t thread = {0};
	si_thread_create(&thread, test_runner, &value);
	si_thread_timedjoin_3(&thread, 1000, true); // 1-sec
}

/** Doxygen
 * @brief Runs all local si_thread_t unit tests.
 */
static void si_thread_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_thread_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Begin testing of si_thread.\n");
	si_thread_test_all();
	(void)printf("End of si_thread testing.\n");
	return 0;
}
