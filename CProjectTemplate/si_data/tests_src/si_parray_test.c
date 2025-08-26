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
void parray_test_init(void)
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
void parray_test_modify(void)
{
	// TODO Finish tests
	const int data[] = { 5, 4, 3, 2, 1, 0, 42 };
	const size_t data_size = 7u;
	printf("Initial data conditions:\n");
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		printf("\tdata[%lu]: %d @ %p\n", iii, data[iii], &data[iii]);
	}

	si_realloc_settings_t settings = {0};
	si_realloc_settings_new(&settings);

	si_parray_t* p_array = NULL;
	p_array = si_parray_new();
	TEST_ASSERT_NOT_NULL(p_array);
	p_array->p_settings = &settings;

	printf("Testing append/count/grow: ");
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, si_parray_append(NULL, NULL));
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, si_parray_append(p_array, NULL));
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, si_parray_append(NULL, &data[0u]));
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		TEST_ASSERT_EQUAL_size_t(iii, si_parray_count(p_array));
		TEST_ASSERT_EQUAL_size_t(iii, si_parray_append(p_array, &data[iii]));
	}
	TEST_ASSERT_NOT_EQUAL_size_t(0u, p_array->array.capacity);
	TEST_ASSERT_EQUAL_size_t(0u,
		(p_array->array.capacity % (size_t)settings.grow_value));
	TEST_ASSERT_EQUAL_size_t(data_size, si_parray_count(p_array));
	TEST_ASSERT_NULL(p_array->p_free_value);
	printf("Done.\n");
	si_parray_fprint(stdout, p_array, NULL); fprintf(stdout, "\n");

	printf("Testing at/find/contains: ");
	TEST_ASSERT_NULL(si_parray_at(NULL, SIZE_MAX));
	TEST_ASSERT_NULL(si_parray_at(NULL, 0u));
	TEST_ASSERT_NULL(si_parray_at(p_array, SIZE_MAX));
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		TEST_ASSERT_EQUAL_PTR(&data[iii], si_parray_at(p_array, iii));
		TEST_ASSERT_TRUE(si_parray_contains(p_array, &data[iii], NULL));
		TEST_ASSERT_EQUAL_size_t(iii, si_parray_find(p_array, &data[iii], NULL));
	}
	printf("Done.\n");
	si_parray_fprint(stdout, p_array, NULL); fprintf(stdout, "\n");

	printf("Testing remove_at/count/shrink: ");
	TEST_ASSERT_FALSE(si_parray_remove_at(NULL, SIZE_MAX));
	TEST_ASSERT_FALSE(si_parray_remove_at(p_array, SIZE_MAX));
	TEST_ASSERT_FALSE(si_parray_remove_at(NULL, 0u));
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		TEST_ASSERT_EQUAL_size_t(data_size - iii, si_parray_count(p_array));
		TEST_ASSERT_TRUE(si_parray_remove_at(p_array, 0u));
	}
	TEST_ASSERT_EQUAL_size_t(0u, p_array->array.capacity);
	printf("Done.\n");
	si_parray_fprint(stdout, p_array, NULL); fprintf(stdout, "\n");

	//* TODO
	printf("Testing pointer ownership:");
	const size_t direct_count = 3u;
	TEST_ASSERT_NOT_EQUAL_size_t(direct_count, data_size);
	for(size_t iii = 0u; iii < direct_count; iii++)
	{
		TEST_ASSERT_EQUAL_size_t(iii, si_parray_append(p_array, &data[iii]));
	}
	si_parray_fprint(stdout, p_array, NULL); fprintf(stdout, "\n");
	// Now should become owner after cloning.
	for(size_t iii = direct_count; iii < data_size; iii++)
	{
		TEST_ASSERT_EQUAL_size_t(iii, si_parray_append_clone(
			p_array,
			&data[iii],
			sizeof(int))
		);
	}
	TEST_ASSERT_EQUAL_size_t(data_size, si_parray_count(p_array));
	printf("Done.\n");
	si_parray_fprint(stdout, p_array, NULL); fprintf(stdout, "\n");
	//*/

	si_parray_destroy(&p_array);
	TEST_ASSERT_NULL(p_array);
}

/** Doxygen
 * @brief Runs all unity tests available.
 */
void parray_test_all(void)
{
	UNITY_BEGIN();
	//RUN_TEST(parray_test_init);
	RUN_TEST(parray_test_modify);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Start of parray unit test.\n");
	parray_test_all();
	printf("End of parray unit test.\n");
}
