#include <stdio.h> //FILE printf()

#include "unity.h"
#include "si_adler.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_adler_test_main(void)
{
	// TODO Use the expected results to validate.
	const char* const p_inputs[] =
	{
		"",
		"~~~",
		"abc",
		"abd",
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	};
	const char* const p_expected[] =
	{
		"0x0001",
		"0x0680",
		"0x572C",
		"0x582D",
		"0xA68B"
	};
	const size_t inputs_count = 5u;

	si_adler_t hash = (si_adler_t){0};
	for (size_t iii = 0u; iii < inputs_count; iii++)
	{
		si_adler_new(&hash, 2u);
		size_t input_length = strlen(p_inputs[iii]);
		si_adler_update(&hash, (uint8_t*)p_inputs[iii], input_length);
		printf("'%48s':", p_inputs[iii]);
		si_adler_fprint(&hash, stdout); printf("\n");
		si_adler_free(&hash);
	}
}

void si_adler_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_adler_test_main);
	UNITY_END();
}

int main(void)
{
	si_adler_test_all();
	return 0;
}
