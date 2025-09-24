// si_term_targa_test.c

#include <stdio.h> // printf()

#include "si_term_targa.h"
#include "unity.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

static void test_draw(void)
{
	const char* const P_PATH = "./example.tga";
	si_terminfo_t terminal_info = {0};
	si_terminfo_init(&terminal_info, stdout);
	si_tga_t tga = {0};
	si_tga_init(&tga);
	const bool read_result = si_tga_fread_from(&tga, P_PATH);
	TEST_ASSERT_TRUE(read_result);

	size_t x_pos = (terminal_info.COLUMNS / 2) - (tga.header.width  / 2);
	size_t y_pos = (terminal_info.ROWS    / 2) - (tga.header.height / 2);
	if(terminal_info.COLUMNS <= x_pos)
	{
		x_pos = 1u;
	}
	if(terminal_info.ROWS <= y_pos)
	{
		y_pos = 1u;
	}
	const bool drew = si_terminfo_draw_tga_at(
		&terminal_info, &tga, x_pos + 1u, y_pos + 1u
	);

	si_tga_free(&tga);
	TEST_ASSERT_TRUE(drew);
}

static void test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_draw);
	UNITY_END();
}

int main(void)
{
	printf("Start of unit test for si_term_targa.\n");
	test_all();
	printf("End of unit test for si_term_targa.\n");
	return 0;
}
