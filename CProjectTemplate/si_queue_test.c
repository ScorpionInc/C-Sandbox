#include "si_queue.h"

void test_00()
{
	si_queue q = {};
	init_si_queue(&q, sizeof(char));
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
	free_si_queue(&q);
}


#define SI_TEMPLATE_TYPE char
#include "si_queue.template"

void test_01()
{
	char_queue q = {};
	init_char_queue(&q);
	for(int i = 0; i < 6; i++)
	{
		char_queue_enqueue(&q, 'a' + i);
		printf("Queue Count: %lu\tCapacity: %lu\n", char_queue_count(&q), q.dynamic.capacity);
	}
	for(int i = 0; i < 6; i++)
	{
		printf("Popped char: %c\tNew Count: %lu.\n", char_queue_dequeue(&q), si_queue_count(&q));
	}
	free_char_queue(&q);
}//*/
//void test_01(){}

int main(void)
{
	test_00();
	test_01();
	return 0;
}
