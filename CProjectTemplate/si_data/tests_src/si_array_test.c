#include <stdio.h>

#include "unity.h"

#define SI_TEMPLATE_TYPE char
#include "si_array.template"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_array_test_main(void)
{
	// TODO Use TEST macros.
	char_array_t array = {0};
	char_array_init_2(&array, 69u);
	char_array_set(&array, 0u, '$');
	printf("Value at 0: %c\n", char_array_get(&array, 0u));
	char_array_free(&array);
}

void si_array_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_array_test_main);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	si_array_test_all();
	return 0;
}
