// si_terminfo_test.c

#include <stdio.h> // printf()

#include "unity.h"
#include "si_terminfo.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs local unit tests for si_terminfo file_clone
 */
static void si_terminfo_test_print(void)
{
	si_terminfo_t terminal_information = {0};
	si_terminfo_init(&terminal_information, stdout);
	si_terminfo_printf_centered(&terminal_information, "Hello World!\n");
}

/** Doxygen
 * @brief Runs all local unit tests for si_terminfo
 */
static void si_terminfo_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_terminfo_test_print);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_terminfo unit test(s).\n");
	si_terminfo_test_all();
	printf("End of si_terminfo unit test(s).\n");
	return 0;
}
