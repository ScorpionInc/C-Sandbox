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

	const bool drew = si_terminfo_fdraw_tga_at(&terminal_info, P_PATH, 1u, 1u);
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
