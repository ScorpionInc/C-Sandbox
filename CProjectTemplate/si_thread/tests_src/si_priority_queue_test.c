// si_priority_queue_test.c

#include <stdio.h> // printf()

#include "unity.h" // RUN_TEST(), UNITY_BEGIN(), UNITY_END()
#include "si_priority_queue.h" // si_priority_queue_t

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}

/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_priority_queue_test_init(void)
{
	const size_t priority_count = 16u;
	si_priority_queue_t* p_queue = NULL;
	p_queue = si_priority_queue_new(priority_count);
	TEST_ASSERT_NOT_NULL(p_queue);

	si_priority_queue_free(p_queue);
	TEST_ASSERT_NOT_NULL(p_queue);
	si_priority_queue_destroy(&p_queue);
	TEST_ASSERT_NULL(p_queue);
}

void si_priority_queue_test_modify(void)
{
	const int p_data[] =     { 0,    1, 2,  3, 5, 42, 69, 420 };
	const int p_priority[] = { 0,    0, 2,  6, 5,  7,  9,  10 };
	const int p_expected[] = { 420, 69, 42, 3, 5,  2,  0,   1 };
	const size_t data_count = sizeof(p_data) / sizeof(int);
	const size_t priority_count = 16u;

	si_priority_queue_t* p_queue = si_priority_queue_new(priority_count);
	TEST_ASSERT_NOT_NULL(p_queue);

	printf("p_data:%lu @ %p {\n", data_count, p_data);
	for(size_t iii = 0u; iii < data_count; iii++)
	{
		printf("\t[%lu]: %d @ %p\n", iii, p_data[iii], &p_data[iii]);
	}printf("}\n");

	for(size_t iii = 0u; iii < data_count; iii++)
	{
		const int* p_next = &p_data[iii];
		const bool enq_result = si_priority_queue_enqueue(
			p_queue, (void*)p_next, p_priority[iii]
		);
		TEST_ASSERT_TRUE(enq_result);
	}

	printf("p_expected:%lu @ %p {\n", data_count, p_expected);
	for(size_t iii = 0u; iii < data_count; iii++)
	{
		int* p_next = si_priority_queue_dequeue(p_queue);
		TEST_ASSERT_NOT_NULL(p_next);
		TEST_ASSERT_EQUAL_INT(p_expected[iii], *p_next);
		printf("\t[%lu]: %d @ %p\n", iii, *p_next, p_next);
	}printf("}\n");
	TEST_ASSERT_EQUAL_size_t(0u, si_priority_queue_count(p_queue));

	si_priority_queue_destroy(&p_queue);
	TEST_ASSERT_NULL(p_queue);
}

void si_priority_queue_test_all(void)
{
	UNITY_BEGIN();
	//RUN_TEST(si_priority_queue_test_init);
	RUN_TEST(si_priority_queue_test_modify);
	UNITY_END();
}

int main(void)
{
	printf("Begin testing of si_priority_queue.\n");
	si_priority_queue_test_all();
	printf("End of si_priority_queue testing.\n");
	return 0;
}