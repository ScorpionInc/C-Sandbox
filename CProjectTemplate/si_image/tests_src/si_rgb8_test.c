#include "si_rgb8.h"
#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()

#include <limits.h> // UINT8_MAX, INT64_MAX
#include <stddef.h> // size_t
#include <stdlib.h> // rand(), srand()
#include <time.h> // time()

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
	unsigned int seed = (unsigned int)time(NULL);
	srand(seed);
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
	si_rgb8_t foreground_color = {0};
	for (size_t iii = 0u; iii < str_len; iii++)
	{
		si_rgb8_randomize(&foreground_color);
		si_rgb8_apply_3(stdout, &foreground_color, true);
		(void)printf("%c", p_str[iii]);
	}
	(void)printf("\033[0m");
END:
	return;
}

/** Doxygen
 * @brief Runs unit test for applying color to ANSI FILE(stdout)
 */
static void si_rgb8_test_apply(void)
{
	rainbow_print("Hello World! This is a test of an rgb color struct!\n");
}

/** Doxygen
 * @brief Runs unit test for struct conversion functions.
 */
static void si_rgb8_test_convert(void)
{
	si_rgb8_t rgb = {0};

	si_ansi_color_t ansi_color = si_ansi_color_3(ANSI_COLOR_GREEN, true, true);
	si_rgb8_from_ansi_color(&rgb, ansi_color);
	TEST_ASSERT_EQUAL(UINT8_MAX, rgb.green);

	si_rgba8_t rgba = {0};
	rgba.red = 123u;
	rgba.green = 45u;
	rgba.blue = 67u;
	rgba.alpha = 89u;
	si_rgb8_from_rgba8(&rgb, &rgba);
	TEST_ASSERT_EQUAL_UINT8(rgba.red,   rgb.red);
	TEST_ASSERT_EQUAL_UINT8(rgba.green, rgb.green);
	TEST_ASSERT_EQUAL_UINT8(rgba.blue,  rgb.blue);
}

/** Doxygen
 * @brief Runs all available static unit tests.
 */
static void si_rgb8_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_rgb8_test_apply);
	RUN_TEST(si_rgb8_test_convert);
	UNITY_END();
}

int main(void)
{
	(void)printf("Starting unit tests for si_rgb8.\n");
	si_rgb8_test_all();
	(void)printf("End of unit tests for si_rgb8.\n");
	return 0;
}