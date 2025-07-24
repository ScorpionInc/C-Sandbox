//si_uint_utils_test.c

#include <stdio.h>

#include "unity.h"
#include "si_uint_utils.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_uint_utils_test_calls(void)
{
	//! TODO Write Library Unit Tests
	printf("si_cbig Unit Test is not yet implemented.\n");
}

void si_uint_utils_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_uint_utils_test_calls);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	si_uint_utils_test_all();
	return 0;
}
