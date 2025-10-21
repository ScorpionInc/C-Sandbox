// si_server_test.c
#include <stdio.h> // printf()

#include "unity.h"
#include "si_server.h"

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
static void si_server_test_main(void)
{
	//!TODO
}

/** Doxygen
 * @brief Runs all locally defined unit tests.
 */
static void si_server_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_server_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_ unit test.\n");
	si_server_test_all();
	(void)printf("End of si_ unit test.\n");
	return 0;
}
