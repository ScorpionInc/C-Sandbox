// si_access_list_test.c
#include <stdio.h> // printf()

#include "unity.h"
#include "si_access_list.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs the main unit test.
 */
static void si_access_list_test_main(void)
{
	//!TODO
}

/** Doxygen
 * @brief Runs all locally defined unit tests.
 */
static void si_access_list_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_access_list_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_access_list unit test.\n");
	si_access_list_test_all();
	(void)printf("End of si_access_list unit test.\n");
	return 0;
}
