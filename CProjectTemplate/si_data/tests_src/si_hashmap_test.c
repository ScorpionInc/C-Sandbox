#include <stdio.h>

#include "unity.h"
#include "si_hashmap.h"
#include "si_realloc_settings.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_hashmap_test_print_map(const si_map_t* p_map)
{
	if(NULL == p_map)
	{
		goto END;
	}
	const size_t count = si_map_count(p_map);
	const size_t capacity = p_map->entries.capacity;
	size_t counter = 0u;
	printf("[");
	for(size_t i = 0u; i < capacity; i++)
	{
		si_map_pair_t** pp_pair = si_array_at(&(p_map->entries), i);
		if(NULL == pp_pair)
		{
			break;
		}
		if(NULL == *pp_pair)
		{
			continue;
		}
		printf("{%p: %p(0x%lx)->%p}", *pp_pair, (*pp_pair)->p_key,
			*((size_t*)(*pp_pair)->p_key), (*pp_pair)->p_value);
		if(count > (counter + 1u))
		{
			printf(", ");
		}
	}
	printf("]%lu/%lu", count, capacity);
END:
	return;
}

void si_hashmap_test_print(const si_hashmap_t* const p_hashmap)
{
	if(NULL == p_hashmap)
	{
		goto END;
	}
	printf("{");
	for(size_t i = 0u; i < p_hashmap->maps.capacity; i++)
	{
		si_map_t** pp_map = NULL;
		pp_map = si_array_at(&(p_hashmap->maps), i);
		if(NULL == pp_map)
		{
			break;
		}
		printf("%lu:", i);
		if(NULL == *pp_map)
		{
			printf("NULL");
		}
		else
		{
			si_hashmap_test_print_map(*pp_map);
		}
		if((p_hashmap->maps.capacity - 1u) > i)
		{
			printf(", ");
		}
	}
	printf("}%lu\n", si_hashmap_count(p_hashmap));
END:
	return;
}

void si_hashmap_test_init(void)
{
	const size_t capacity = 32u;
	si_hashmap_t* p_hashmap = si_hashmap_new(0u);
	TEST_ASSERT_NULL(p_hashmap);
	p_hashmap = si_hashmap_new(capacity);
	TEST_ASSERT_NOT_NULL(p_hashmap);
	TEST_ASSERT_NOT_NULL(p_hashmap->maps.p_data);
	TEST_ASSERT_EQUAL_size_t(capacity, p_hashmap->maps.capacity);
	TEST_ASSERT_EQUAL_size_t(sizeof(void*), p_hashmap->maps.element_size);
	si_hashmap_free_at(&p_hashmap);
	TEST_ASSERT_NULL(p_hashmap);
}

void si_hashmap_test_modify(void)
{
	si_realloc_settings_t settings = {0};
	si_realloc_settings_new(&settings);

	const size_t capacity = 32u;
	si_hashmap_t* p_hashmap = si_hashmap_new(capacity);
	TEST_ASSERT_NOT_NULL(p_hashmap);
	p_hashmap->p_settings = &settings;
	si_hashmap_update_settings(p_hashmap);

	size_t keys[] = { 1, 2, 3, 4, 5 };
	int    data[] = { 6, 7, 8, 9, 42 };
	const size_t data_size = 5u;
	const size_t fake_hash = 420u;

	printf("Testing count() 1/2\n");
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, si_hashmap_count(NULL));
	TEST_ASSERT_EQUAL_size_t(0u, si_hashmap_count(p_hashmap));

	printf("Testing insert(), count() 2/2:\n");
	for(size_t i = 0u; i < data_size; i++)
	{
		TEST_ASSERT_TRUE(si_hashmap_insert(
			p_hashmap, &(keys[i]), sizeof(size_t), &data[i]
		));
	}
	TEST_ASSERT_EQUAL_size_t(data_size, si_hashmap_count(p_hashmap));
	si_hashmap_test_print(p_hashmap);

	printf("Testing hash()\n");
	size_t zed_hash = si_hashmap_hash(NULL, NULL, SIZE_MAX);
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, zed_hash);
	zed_hash = si_hashmap_hash(NULL, &(keys[0]), sizeof(size_t));
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, zed_hash);
	zed_hash = si_hashmap_hash(p_hashmap, NULL, SIZE_MAX);
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, zed_hash);
	zed_hash = si_hashmap_hash(p_hashmap, &(keys[0]), sizeof(size_t));
	TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, zed_hash);

	printf("Testing at_hash():\n");
	TEST_ASSERT_NULL(si_hashmap_at_hash(NULL, fake_hash));
	TEST_ASSERT_NULL(si_hashmap_at_hash(p_hashmap, fake_hash));
	TEST_ASSERT_NOT_NULL(si_hashmap_at_hash(p_hashmap, zed_hash));

	printf("Testing at():\n");
	TEST_ASSERT_NULL(si_hashmap_at(NULL, NULL, SIZE_MAX));
	TEST_ASSERT_NULL(si_hashmap_at(NULL, &(keys[0]), sizeof(size_t)));
	TEST_ASSERT_NULL(si_hashmap_at(p_hashmap, NULL, SIZE_MAX));
	TEST_ASSERT_NOT_NULL(si_hashmap_at(p_hashmap, &(keys[0]), sizeof(size_t)));

	si_hashmap_free_at(&p_hashmap);
	TEST_ASSERT_NULL(p_hashmap);
}

void si_hashmap_test_all(void)
{
	UNITY_BEGIN();
	//RUN_TEST(si_hashmap_test_init);
	RUN_TEST(si_hashmap_test_modify);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	si_hashmap_test_all();
	return 0;
}
