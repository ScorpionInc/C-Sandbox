#include <stdbool.h> // bool, false, true
#include <stdio.h> // FILE

#include "si_targa.h" // si_tga_t
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
 * @brief Local function generates example 255 color data for an RGB image.
 *
 * @param p_data Pointer to pixel data buffer to be modified.
 * @param width X-Dimension of the pixel buffer.
 * @param height Y-Dimension of the pixel buffer.
 * @param channels Number of color channels per pixel.
 */
static void generate_image(uint8_t* const p_data,
	const uint16_t width, const uint16_t height, const uint8_t channels)
{
	if ((NULL == p_data) || (0u >= width) || (0u >= height) || (0u >= channels))
	{
		goto END;
	}
	uint8_t* byte_iter = p_data;
	for (uint16_t yyy = 0u; yyy < height; yyy++)
	{
		for (uint16_t xxx = 0u; xxx < width; xxx++)
		{
			*byte_iter++ = (uint8_t)(255.0 * ((float)yyy / height));
			if(2u <= channels)
			{
				*byte_iter++ = (uint8_t)(255.0 * ((float)xxx / width));
			}
			if(3u <= channels)
			{
				*byte_iter++ = (uint8_t)(255.0 * ((float)yyy / height));
			}
			if(4u <= channels)
			{
				*byte_iter++ = (uint8_t)(255.0 * ((float)yyy / height));
			}
		}
	}
END:
	return;
}

static void test_generate(void)
{
	const char* const P_PATH   = "./example.tga";
	const uint16_t    width    = 500u;
	const uint16_t    height   = 200u;
	const uint8_t     channels = 3u;

	si_tga_t tga = {0};
	si_tga_init_4(&tga, width, height, channels);
	generate_image(tga.p_data, width, height, channels);
	const bool write_result = si_tga_fwrite_to(&tga, P_PATH);
	si_tga_free(&tga);
	TEST_ASSERT_TRUE(write_result);
}

static void test_clone(void)
{
	const uint16_t    new_width  = 50u;
	const uint16_t    new_height = 20u;
	const char* const P_PATH   = "./example.tga";
	const char* const P_OUTPUT = "./example2.tga";

	si_tga_t tga = {0};
	const bool read_result = si_tga_fread_from(&tga, P_PATH);
	TEST_ASSERT_TRUE(read_result);
	si_tga_header_fprint(stdout, &tga.header);

	const bool resize_result = si_tga_resize(&tga, new_width, new_height);
	TEST_ASSERT_TRUE(resize_result);

	const bool write_result = si_tga_fwrite_to(&tga, P_OUTPUT);
	si_tga_free(&tga);
	TEST_ASSERT_TRUE(write_result);
}

static void test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_generate);
	RUN_TEST(test_clone);
	UNITY_END();
}

int main(void)
{
	printf("Starting unit tests for si_targa.\n");
	test_all();
	printf("End of unit tests for si_targa.\n");
	return 0;
}
