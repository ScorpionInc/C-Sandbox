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

void si_queue_test_modify(void)
{
	// TODO Use TEST macro from unity.
	si_queue q = {};
	si_queue_new(&q, sizeof(char));
	char c = 'a';
	for(int i = 0; i < 6; i++)
	{
		si_queue_enqueue(&q, &c);
		c += 1u;
		printf("Queue Count: %lu\tCapacity: %lu\n", si_queue_count(&q), q.dynamic.capacity);
	}
	for(int i = 0; i < 6; i++)
	{
		si_queue_dequeue(&q, &c);
		printf("Popped char: %c\tNew Count: %lu.\n", c, si_queue_count(&q));
	}
	si_queue_free(&q);
}


#define SI_TEMPLATE_TYPE char
#include "si_queue.template"

void si_queue_test_template(void)
{
	// TODO Use TEST macro from unity.
	char_queue q = {};
	char_queue_new(&q);
	for(int i = 0; i < 6; i++)
	{
		char_queue_enqueue(&q, 'a' + i);
		printf("Queue Count: %lu\tCapacity: %lu\n", char_queue_count(&q), q.dynamic.capacity);
	}
	for(int i = 0; i < 6; i++)
	{
		printf("Popped char: %c\tNew Count: %lu.\n", char_queue_dequeue(&q), si_queue_count(&q));
	}
	char_queue_free(&q);
}

void si_queue_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_queue_test_modify);
	RUN_TEST(si_queue_test_template);
	UNITY_END();
}

int main(void)
{
	si_queue_test_all();
	return 0;
}
