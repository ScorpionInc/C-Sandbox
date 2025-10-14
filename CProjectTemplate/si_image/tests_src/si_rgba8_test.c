#include "si_rgba8.h"
#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Prints a string to stdout changing the color each character randomly.
 * 
 * @param p_str C String to be printed to stout.
 */
static void rainbow_print(const char* const p_str)
{
	size_t str_len = strnlen(p_str, INT64_MAX);
	if (str_len >= INT64_MAX)
	{
		goto END;
	}
	si_rgba8_t foreground_color = {0};
	si_rgb8_t truncated_color = {0};
	for (size_t iii = 0u; iii < str_len; iii++)
	{
		si_rgba8_randomize(&foreground_color);
		si_rgb8_from_rgba8(&truncated_color, &foreground_color);
		si_rgb8_apply_3(stdout, &truncated_color, true);
		(void)printf("%c", p_str[iii]);
	}
	(void)printf("\033[0m");
END:
	return;
}

/** Doxygen
 * @brief Runs unit test for applying color to ANSI FILE(stdout)
 */
static void si_rgba8_test_apply(void)
{
	rainbow_print("Hello World! This is a test of an rgba color struct!\n");
}

/** Doxygen
 * @brief Runs unit test for struct conversions
 */
static void si_rgba8_test_convert(void)
{
	si_rgba8_t rgba = {0};

	si_ansi_color_t ansi_color = si_ansi_color_3(ANSI_COLOR_BLUE, true, true);
	si_rgba8_from_ansi_color_3(&rgba, ansi_color, 123u);
	TEST_ASSERT_EQUAL_UINT8(UINT8_MAX, rgba.blue);
	TEST_ASSERT_EQUAL_UINT8(123u, rgba.alpha);
	si_rgba8_init_5(&rgba, 0u, 0u, 0u, 0u);

	si_rgb8_t rgb = {0};
	rgb.red = 123u;
	rgb.green = 45u;
	rgb.blue = 67u;
	si_rgba8_from_rgb8_3(&rgba, &rgb, 89u);
	TEST_ASSERT_EQUAL_UINT8(rgb.red, rgba.red);
	TEST_ASSERT_EQUAL_UINT8(rgb.green, rgba.green);
	TEST_ASSERT_EQUAL_UINT8(rgb.blue, rgba.blue);
	TEST_ASSERT_EQUAL_UINT8(89u, rgba.alpha);
}

/** Doxygen
 * @brief Runs all available static unit tests.
 */
static void si_rgba8_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_rgba8_test_apply);
	RUN_TEST(si_rgba8_test_convert);
	UNITY_END();
}

int main(void)
{
	(void)printf("Starting unit tests for si_rgba8.\n");
	si_rgba8_test_all();
	(void)printf("End of unit tests for si_rgba8.\n");
	return 0;
}
