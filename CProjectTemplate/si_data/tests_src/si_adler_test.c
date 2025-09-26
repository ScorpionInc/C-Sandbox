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
	const char* const p_inputs[] =
	{
		"~~~",
		"abc",
		"abd",
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	};
	const uint16_t p_expected[] =
	{
		0x0680,
		0x572C,
		0x582D,
		0xA68B
	};
	const size_t inputs_count = sizeof(p_expected) / sizeof(uint16_t);

	si_adler_t hash = (si_adler_t){0};

	// Test empty case
	si_adler_new(&hash, 2u);
	si_adler_update(&hash, (uint8_t*)"", 0u);
	const uint16_t raw_result = *((uint16_t*)hash.p_hash);
	si_adler_free(&hash);
	TEST_ASSERT_EQUAL_UINT16(0x0001, raw_result);

	// Tests inputs vs expected outputs
	for (size_t iii = 0u; iii < inputs_count; iii++)
	{
		si_adler_new(&hash, 2u);
		size_t input_length = strnlen(p_inputs[iii], __INT_MAX__);
		TEST_ASSERT_GREATER_THAN_size_t(0u, input_length);
		TEST_ASSERT_LESS_THAN_size_t(__INT_MAX__, input_length);
		si_adler_update(&hash, (uint8_t*)p_inputs[iii], input_length);

		printf("'%48s':", p_inputs[iii]);
		si_adler_fprint(&hash, stdout); printf("\n");
		
		const uint16_t raw_result = *((uint16_t*)hash.p_hash);
		si_adler_free(&hash);
		TEST_ASSERT_EQUAL_UINT16(p_expected[iii], raw_result);
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
