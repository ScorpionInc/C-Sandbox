// si_poll_test.c
#include <stdio.h> // printf()

#include "unity.h"
#include "si_poll.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_poll_test_main(void)
{
	//!TODO
	const size_t capacity = 16u;
	si_poll_t* p_poll = NULL;
	TEST_ASSERT_NULL(si_poll_new_2(0u, 0));
	TEST_ASSERT_EQUAL_size_t(0u, si_poll_count(NULL));

	p_poll = si_poll_new_2(capacity, 0);
	TEST_ASSERT_NOT_NULL(p_poll);
	TEST_ASSERT_EQUAL_size_t(0u, si_poll_count(p_poll));
	
	si_poll_destroy(&p_poll);
	TEST_ASSERT_NULL(p_poll);
}

/** Doxygen
 * @brief Runs all locally defined unit tests.
 */
static void si_poll_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_poll_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_poll unit test.\n");
	si_poll_test_all();
	(void)printf("End of si_poll unit test.\n");
	return 0;
}
