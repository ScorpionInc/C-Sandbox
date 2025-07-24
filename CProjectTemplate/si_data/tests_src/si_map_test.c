#include <stdio.h> // printf
#include <stdlib.h> // calloc, free

#include "unity.h"
#include "si_map.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Tests creation and destruction only.
 */
void si_map_test_init(void)
{
	si_map_t* p_map = si_map_new();
	si_map_free_at(&p_map);
}

/** Doxygen
 * @brief Tests modification of contents. Add/Remove/Clear/ect.
 */
void si_map_test_modify(void)
{
	TEST_ASSERT_NULL(NULL);
}

/** Doxygen
 * @brief Runs all unity tests available.
 */
void si_map_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_map_test_init);
	RUN_TEST(si_map_test_modify);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Start of si_map unit test.\n");
	si_map_test_all();
	printf("End of si_map unit test.\n");
}
