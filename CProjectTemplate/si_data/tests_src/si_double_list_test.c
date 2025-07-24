#include <stdio.h>
#include <stdlib.h>

#include "unity.h"
#include "si_double_list.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void print_int_double_list(const si_double_list_t* const p_list)
{
	if(NULL == p_list)
	{
		goto END;
	}
	printf("{");
	size_t print_counter = 0u;
	for(size_t i = 0u; i < p_list->capacity; i++)
	{
		int* p_data = si_double_list_at(p_list, i);
		if(NULL != p_data)
		{
			printf("%d", *p_data);
			print_counter++;
			if(print_counter < (p_list->capacity))
			{
				printf(", ");
			}
		}
	}
	printf("}:%lu/%lu", p_list->count, p_list->capacity);
END:
	return;
}

/** Doxygen
 * @brief Tests creation and destruction only.
 */
void double_list_test_init(void)
{
	const size_t initial_capacity = 2u;
	si_double_list_t* p_list = calloc(1u, sizeof(si_double_list_t));
	TEST_ASSERT_NOT_NULL(p_list);
	si_double_list_init_4(p_list, true, initial_capacity, NULL);
	TEST_ASSERT_EQUAL_size_t(initial_capacity, p_list->capacity);
	TEST_ASSERT_EQUAL_size_t(0u, p_list->count);
	TEST_ASSERT_NOT_NULL(p_list->p_head);
	TEST_ASSERT_TRUE(p_list->is_circular);
	TEST_ASSERT_NOT_EQUAL(p_list->p_head, p_list->p_tail);

	si_double_list_free(p_list);
	TEST_ASSERT_NULL(p_list->p_head);
	TEST_ASSERT_NULL(p_list->p_tail);
	free(p_list);
	p_list = NULL;

	p_list = si_double_list_new();
	TEST_ASSERT_NOT_NULL(p_list);
	si_double_list_free_at(&p_list);
	TEST_ASSERT_NULL(p_list);
}

/** Doxygen
 * @brief Tests modification of contents. Add/Remove/Clear/ect.
 */
void double_list_test_modify(void)
{
	const int data[] = { -1, -2, -3, -4, 0, 1, 2, 3, 4, 42 };
	const size_t data_size = sizeof(data) / sizeof(data[0]);
	TEST_ASSERT_EQUAL_size_t(10u, data_size);

	si_double_list_t* p_list = si_double_list_new();
	TEST_ASSERT_NOT_NULL(p_list);
	TEST_ASSERT_TRUE(p_list->is_circular);

	printf("Testing is_empty() & is_full()\n");
	TEST_ASSERT_TRUE(si_double_list_is_empty(NULL));
	TEST_ASSERT_TRUE(si_double_list_is_empty(p_list));
	TEST_ASSERT_TRUE(si_double_list_is_full(NULL));
	TEST_ASSERT_TRUE(si_double_list_is_full(p_list));

	printf("Testing insert()\n");
	TEST_ASSERT_FALSE(si_double_list_insert(NULL, NULL, 0u));
	TEST_ASSERT_TRUE(si_double_list_insert(p_list, &data[0], 0u));
	TEST_ASSERT_EQUAL_size_t(1u, p_list->count);
	TEST_ASSERT_EQUAL_size_t(1u, p_list->capacity);
	TEST_ASSERT_NOT_NULL(p_list->p_head);
	TEST_ASSERT_EQUAL(p_list->p_head, p_list->p_tail);
	TEST_ASSERT_EQUAL(&data[0], si_double_list_at(p_list, 0u));
	TEST_ASSERT_FALSE(si_double_list_is_empty(p_list));

	printf("Testing remove_at()\n");
	TEST_ASSERT_FALSE(si_double_list_remove_at(NULL, 0u));
	TEST_ASSERT_TRUE(si_double_list_remove_at(p_list, 0u));
	TEST_ASSERT_EQUAL_size_t(0u, p_list->count);
	TEST_ASSERT_EQUAL_size_t(0u, p_list->capacity);
	TEST_ASSERT_TRUE(si_double_list_is_empty(p_list));

	printf("Testing append()\n");
	TEST_ASSERT_FALSE(si_double_list_append(NULL, &data[0u]));
	TEST_ASSERT_TRUE(si_double_list_append(p_list, &data[0u]));
	TEST_ASSERT_EQUAL_size_t(1u, p_list->count);
	TEST_ASSERT_EQUAL_size_t(1u, p_list->capacity);
	// Clean up/validate node for next test(s)
	TEST_ASSERT_TRUE(si_double_list_remove_at(p_list, 0u));
	TEST_ASSERT_EQUAL_size_t(0u, p_list->count);
	TEST_ASSERT_EQUAL_size_t(0u, p_list->capacity);
	TEST_ASSERT_NULL(p_list->p_head);
	TEST_ASSERT_NULL(p_list->p_tail);

	printf("Testing push():");
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, si_double_list_push(NULL, &data[0u]));
	for(size_t i = 0u; i < data_size; i++)
	{
		TEST_ASSERT_EQUAL_size_t(i, si_double_list_push(p_list, &data[i]));
		TEST_ASSERT_EQUAL(&data[i], si_double_list_at(p_list, i));
	}
	TEST_ASSERT_EQUAL_size_t(data_size, p_list->count);
	TEST_ASSERT_EQUAL_size_t(data_size, p_list->capacity);

	printf("Testing find()\n");
	// TODO

	printf("Testing pop()\n");
	for(size_t i = 0u; i < data_size; i++)
	{
		int* p_data = si_double_list_pop(p_list);
		TEST_ASSERT_NOT_NULL(p_data);
		TEST_ASSERT_EQUAL_INT(*p_data, data[i]);
		TEST_ASSERT_NULL(si_double_list_at(p_list, i));
	}
	TEST_ASSERT_EQUAL_size_t(0u, p_list->count);
	TEST_ASSERT_EQUAL_size_t(data_size, p_list->capacity);

	printf("Testing sort()\n");
	// Re-add values to be sorted.
	TEST_ASSERT_FALSE(si_double_list_sort(NULL));
	for(size_t i = 0u; i < data_size; i++)
	{
		TEST_ASSERT_EQUAL_size_t(i, si_double_list_push(p_list, &data[i]));
		TEST_ASSERT_EQUAL(&data[i], si_double_list_at(p_list, i));
	}
	TEST_ASSERT_NULL(p_list->p_cmp_f);
	TEST_ASSERT_FALSE(si_double_list_sort(p_list));
	// TODO define and test with cmp

	si_double_list_free_at(&p_list);
}

/** Doxygen
 * @brief Runs all unity tests available.
 */
void double_list_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(double_list_test_init);
	RUN_TEST(double_list_test_modify);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	printf("Start of si_double_list unit test.\n");
	double_list_test_all();
	printf("End of si_double_list unit test.\n");
}
