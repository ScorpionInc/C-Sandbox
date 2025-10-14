#include "si_ansi_color.h"
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
 * @brief Runs unit test for struct initializations
 */
static void si_ansi_color_test_init(void)
{
	si_ansi_color_t ansi_color = si_ansi_color_3((si_ansi_hue_t)-1, true, true);
	TEST_ASSERT_EQUAL(0, ansi_color);
	bool is_valid = si_ansi_color_is_valid(ansi_color);
	TEST_ASSERT_FALSE(is_valid);

	ansi_color = si_ansi_color_3(ANSI_COLOR_INVALID, true, true);
	TEST_ASSERT_EQUAL(0, ansi_color);
	is_valid = si_ansi_color_is_valid(ansi_color);
	TEST_ASSERT_FALSE(is_valid);

	ansi_color = si_ansi_color_3(ANSI_COLOR_BLUE, true, true);
	(void)printf("ANSI_COLOR: %hhu\n", ansi_color);
	TEST_ASSERT_NOT_EQUAL(0, ansi_color);
	is_valid = si_ansi_color_is_valid(ansi_color);
	TEST_ASSERT_TRUE(is_valid);
	const bool is_foreground = si_ansi_color_is_foreground(ansi_color);
	const bool is_background = si_ansi_color_is_background(ansi_color);
	const bool is_intense = si_ansi_color_is_intense(ansi_color);
	TEST_ASSERT_TRUE(is_foreground);
	TEST_ASSERT_FALSE(is_background);
	TEST_ASSERT_TRUE(is_intense);
}

/** Doxygen
 * @brief Runs unit test for ansi color type conversion functions.
 */
static void si_ansi_color_test_convert(void)
{
	si_ansi_color_t color = si_ansi_color(ANSI_COLOR_BLUE);
	bool is_valid = si_ansi_color_is_valid(color);
	TEST_ASSERT_TRUE(is_valid);

	si_rgb8_t rgb = {0};
	rgb.red = 255u;
	rgb.green = 255u;
	rgb.blue = 255u;
	color = si_ansi_color_from_rgb8(&rgb);
	is_valid = si_ansi_color_is_valid(color);
	TEST_ASSERT_TRUE(is_valid);
	si_ansi_hue_t hue = si_ansi_color_hue(color);
	(void)printf("ANSI_COLOR hue: %d.\n", hue);
	TEST_ASSERT_EQUAL(ANSI_COLOR_WHITE, hue);

	si_rgba8_t rgba = {0};
	color = si_ansi_color_from_rgba8(&rgba);
	is_valid = si_ansi_color_is_valid(color);
	TEST_ASSERT_TRUE(is_valid);
	hue = si_ansi_color_hue(color);
	TEST_ASSERT_EQUAL(ANSI_COLOR_BLACK, hue);
}

/** Doxygen
 * @brief Runs all available static unit tests.
 */
static void si_ansi_color_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_ansi_color_test_init);
	RUN_TEST(si_ansi_color_test_convert);
	UNITY_END();
}

int main(void)
{
	(void)printf("Starting unit tests for si_ansi_color.\n");
	si_ansi_color_test_all();
	(void)printf("End of unit tests for si_ansi_color.\n");
	return 0;
}
