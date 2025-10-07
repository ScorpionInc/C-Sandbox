// si_to_string_test.c

#include "si_to_string.h"
#include "unity.h"

#include <stdbool.h> // bool, false, true
#include <stdio.h> // printf()

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs a signed integer types to string unit test.
 */
static void si_to_string_test_signed(void)
{
	const char c_value = '$';
	const bool b_value = false;
	const short s_value = 12345;
	const int i_value = 1234567890;
	const long int l_value = 1234567890123;
	const long long int ll_value = 1234567890123456;

	char* p_data = NULL;

	p_data = char_to_string(c_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Char to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = bool_to_string(b_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Bool to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = short_to_string(s_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Short to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = int_to_string(i_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Int to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = long_to_string(l_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Long Int to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = llong_to_string(ll_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Long Long Int to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;

	p_data = signed_to_string(&ll_value, sizeof(ll_value));
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Signed int to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;
}

/** Doxygen
 * @brief Runs a unsigned integer types to string unit test.
 */
static void si_to_string_test_unsigned(void)
{
	//! TODO Finish testing all types
	//const unsigned char c_value = 123u;
	//const unsigned short s_value = 12345u;
	//const unsigned int i_value = 1234567890u;
	//const unsigned long int l_value = 1234567890123U;
	const unsigned long long int ll_value = 1234567890123456U;

	char* p_data = NULL;

	p_data = ullong_to_string(ll_value);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Unsigned Long Long Int to string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;
}

static void si_to_string_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_to_string_test_signed);
	RUN_TEST(si_to_string_test_unsigned);
	UNITY_END();
}

int main(void)
{
	printf("Starting unit tests for si_to_string.\n");
	si_to_string_test_all();
	printf("End of unit tests for si_to_string.\n");
	return 0;
}