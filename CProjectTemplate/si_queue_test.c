#include "si_queue.h"

int main(void)
{
	si_queue q = {};
	init_si_queue(&q);
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
	return 0;
}
