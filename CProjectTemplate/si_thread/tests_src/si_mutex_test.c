// si_mutex_test.c

#include "si_mutex.h"
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
 * @brief Runs main si_mutex_t unit test.
 */
static void si_mutex_test_main(void)
{
	si_mutex_t mutex = {0};
	TEST_ASSERT_EQUAL_INT(0, si_mutex_init(&mutex));
	printf("try_lock 1\n");
	TEST_ASSERT_TRUE(si_mutex_try_lock(&mutex));
	si_mutex_unlock(&mutex);
	printf("timed_lock 1\n");
	TEST_ASSERT_TRUE(si_mutex_timedlock(&mutex, 100u));
	printf("try_lock 2\n");
	TEST_ASSERT_FALSE(si_mutex_try_lock(&mutex));
	printf("timed_lock 2\n");
	TEST_ASSERT_FALSE(si_mutex_timedlock(&mutex, 100u));
	si_mutex_unlock(&mutex);
	si_mutex_free(&mutex);
}

/** Doxygen
 * @brief Runs all local si_mutex_t unit tests.
 */
static void si_mutex_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_mutex_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Begin testing of si_mutex.\n");
	si_mutex_test_all();
	(void)printf("End of si_mutex testing.\n");
	return 0;
}
