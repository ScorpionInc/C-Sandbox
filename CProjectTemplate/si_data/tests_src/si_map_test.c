#include <stdio.h> // printf
#include <stdlib.h> // calloc, free
#include <string.h>// strlen, strncpy

#include "unity.h"
#include "si_map.h"
#include "si_realloc_settings.h"

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
	TEST_ASSERT_NOT_NULL(p_map);
	TEST_ASSERT_NOT_NULL(p_map->entries.p_data);
	TEST_ASSERT_EQUAL_size_t(0u, p_map->entries.capacity);
	TEST_ASSERT_EQUAL_size_t(sizeof(void*), p_map->entries.element_size);
	TEST_ASSERT_NOT_NULL(p_map->p_cmp_key_f);
	TEST_ASSERT_NOT_NULL(p_map->p_cmp_value_f);
	TEST_ASSERT_NULL(p_map->p_free_key_f);
	TEST_ASSERT_NULL(p_map->p_free_value_f);

	si_map_free_at(&p_map);
}

static void* n_heap_string(const char* p_str, const size_t num)
{
	void* p_hstr = calloc(num, sizeof(char));
	if(NULL != p_hstr)
	{
		strncpy(p_hstr, p_str, num);
	}
	return p_hstr;
}
static void* heap_string(const char* p_str)
{
	return n_heap_string(p_str, strlen(p_str) + 1u);
}

/** Doxygen
 * @brief Tests modification of contents. Add/Remove/Clear/ect.
 */
void si_map_test_modify(void)
{
	si_realloc_settings_t settings = {0};
	si_realloc_settings_new(&settings);

	const char* keys[] = { "name", "age" };
	const char* values[] = { "bob", "42" };
	const size_t data_size = 2u;

	si_map_t* p_map = si_map_new();
	p_map->p_cmp_key_f = (int (*)(const void * const,  const void * const))strcmp;
	p_map->p_cmp_value_f = (int (*)(const void * const,  const void * const))strcmp;
	p_map->p_free_key_f = free;
	p_map->p_free_value_f = free;
	p_map->p_settings = &settings;

	printf("Testing insert():\n");
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		void* p_next_key = heap_string(keys[iii]);
		void* p_next_value = heap_string(values[iii]);
		TEST_ASSERT_TRUE(si_map_insert(p_map, p_next_key, p_next_value));
	}

	printf("Testing at()/find():\n");
	TEST_ASSERT_NULL(si_map_at(NULL, NULL));
	TEST_ASSERT_NULL(si_map_at(NULL, keys[0u]));
	TEST_ASSERT_NULL(si_map_at(p_map, NULL));
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		char* p_next_str = si_map_at(p_map, keys[iii]);
		TEST_ASSERT_NOT_NULL(p_next_str);
		TEST_ASSERT_EQUAL_INT(0, strcmp(p_next_str, values[iii]));
		TEST_ASSERT_EQUAL_size_t(iii, si_map_find(p_map, values[iii]));
	}

	printf("Testing remove()/count():\n");
	TEST_ASSERT_FALSE(si_map_remove(NULL, NULL));
	TEST_ASSERT_FALSE(si_map_remove(NULL, keys[0u]));
	TEST_ASSERT_FALSE(si_map_remove(p_map, NULL));
	for(size_t iii = 0u; iii < data_size; iii++)
	{
		TEST_ASSERT_TRUE(si_map_remove(p_map, keys[iii]));
		TEST_ASSERT_EQUAL_size_t(data_size - (iii + 1u), si_map_count(p_map));
	}

	si_map_free_at(&p_map);
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
