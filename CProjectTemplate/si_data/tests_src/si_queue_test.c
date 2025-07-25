//si_queue_test.c

#include "unity.h"
#include "si_queue.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_queue_test_init(void)
{
	si_queue_t queue = (si_queue_t){};
	si_queue_init(&queue, sizeof(char));
	TEST_ASSERT_EQUAL_size_t(0u, queue.front);
	TEST_ASSERT_EQUAL_size_t(0u, queue.back);

	si_queue_free(&queue);

	si_queue_t* p_queue = si_queue_new(sizeof(char));
	TEST_ASSERT_NOT_NULL(p_queue);
	TEST_ASSERT_EQUAL_size_t(0u, p_queue->front);
	TEST_ASSERT_EQUAL_size_t(0u, p_queue->back);

	si_queue_free(p_queue);
	free(p_queue);
	p_queue = NULL;
}

void si_queue_test_modify(void)
{
	size_t data_size = 6u;
	si_queue_t* p_queue = si_queue_new(sizeof(char));
	TEST_ASSERT_NOT_NULL(p_queue);

	printf("Testing enqueue():\n");
	char c = 'a';
	for(size_t i = 0u; i < data_size; i++)
	{
		TEST_ASSERT_EQUAL_size_t(i + 1u, si_queue_enqueue(p_queue, &c));
		printf("Enqueue: %c\tNew Count: %lu\tCapacity: %lu\n", c,
			si_queue_count(p_queue), p_queue->dynamic.capacity);
		c += 1u;
	}
	printf("\n");
	
	printf("Testing dequeue():\n");
	for(size_t i = 0; i < data_size; i++)
	{
		TEST_ASSERT_EQUAL_size_t(data_size - i - 1u, si_queue_dequeue(p_queue, &c));
		printf("Dequeue: %c\tNew Count: %lu\tCapacity: %lu\n", c,
			si_queue_count(p_queue), p_queue->dynamic.capacity);
	}

	si_queue_free_at(&p_queue);
}


#define SI_TEMPLATE_TYPE char
#include "si_queue.template"

void si_queue_test_template(void)
{
	size_t data_size = 6u;
	char_queue_t* p_queue = char_queue_new();
	TEST_ASSERT_NOT_NULL(p_queue);

	printf("Testing enqueue():\n");
	for(size_t i = 0; i < data_size; i++)
	{
		char c = 'a' + i;
		char_queue_enqueue(p_queue, c);
		size_t new_count = char_queue_count(p_queue);
		TEST_ASSERT_EQUAL_size_t(i + 1u, new_count);
		printf("Enqueue: %c\tNew Count: %lu\tCapacity: %lu\n", c, new_count, p_queue->dynamic.capacity);
	}
	printf("\n");

	printf("Testing dequeue():\n");
	for(size_t i = 0; i < data_size; i++)
	{
		char c = char_queue_dequeue(p_queue);
		size_t new_count = char_queue_count(p_queue);
		TEST_ASSERT_EQUAL_size_t(data_size - i - 1u, new_count);
		printf("Dequeue: %c\tNew Count: %lu\tCapacity: %lu\n", c, new_count, p_queue->dynamic.capacity);
	}

	char_queue_free(p_queue);
	free(p_queue);
	p_queue = NULL;
}

void si_queue_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_queue_test_init);
	RUN_TEST(si_queue_test_modify);
	RUN_TEST(si_queue_test_template);
	UNITY_END();
}

int main(void)
{
	si_queue_test_all();
	return 0;
}
