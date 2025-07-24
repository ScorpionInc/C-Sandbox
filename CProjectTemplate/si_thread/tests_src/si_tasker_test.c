#include "unity.h"
#include "si_tasker.h"

#include <stdio.h>

void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void si_tasker_test_init(void)
{
}

void si_tasker_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_tasker_test_init);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Begin testing of si_tasker.\n");
	si_tasker_test_all();
	printf("End of si_tasker testing.\n");
	return 0;
}
