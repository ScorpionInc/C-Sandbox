#include <stdio.h> // printf
#include <stdlib.h> // calloc, free

#include "unity.h"
#include "si_parray.h"

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
	size_t capacity = 0u;
	si_parray_t pointer_array = {0};
	si_parray_init_2(&pointer_array, capacity);
	TEST_ASSERT_EQUAL_size_t(capacity, pointer_array.array.capacity);
	TEST_ASSERT_EQUAL_size_t(sizeof(void*), pointer_array.array.element_size);
	TEST_ASSERT_NOT_NULL(pointer_array.array.p_data);

	si_parray_free(&pointer_array);

	si_parray_t* p_array = NULL;
	p_array = si_parray_new();
	TEST_ASSERT_NOT_NULL(p_array);

	si_parray_destroy(&p_array);
	TEST_ASSERT_NULL(p_array);
}

/** Doxygen
 * @brief Tests modification of contents. Add/Remove/Clear/ect.
 */
void template_test_modify(void)
{
	// TODO Finish tests
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
	printf("Start of parray unit test.\n");
	template_test_all();
	printf("End of parray unit test.\n");
}
