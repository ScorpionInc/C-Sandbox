#include <stdio.h>

#include "unity.h"
#include "si_signals.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Tests si_signals
 */
static void si_signals_test_main(void)
{
	// TODO
	fprint_signal_debug(stdout);
}

/** Doxygen
 * @brief Runs all local unit tests for si_signals
 */
static void si_signals_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_signals_test_main);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_signals unit test(s).\n");
	si_signals_test_all();
	printf("End of si_signals unit test(s).\n");
	return 0;
}
