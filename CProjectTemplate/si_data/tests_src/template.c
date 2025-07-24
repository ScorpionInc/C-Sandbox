#include <stdio.h> // printf
#include <stdlib.h> // calloc, free

#include "unity.h"
//#include "template.h"

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
void template_test_init(void)
{
	TEST_ASSERT_EQUAL(NULL, NULL);
}

/** Doxygen
 * @brief Tests modification of contents. Add/Remove/Clear/ect.
 */
void template_test_modify(void)
{
	TEST_ASSERT_NULL(NULL);
}

/** Doxygen
 * @brief Runs all unity tests available.
 */
void template_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(template_test_init);
	RUN_TEST(template_test_modify);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Start of template unit test.\n");
	template_test_all();
	printf("End of template unit test.\n");
}
