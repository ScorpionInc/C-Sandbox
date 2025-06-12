#include "si_queue.h"

int main(void)
{
	si_queue q = {};
	init_si_queue(&q);
	char c = 'a';
	si_queue_enqueue(&q, &c);
	free_si_queue(&q);
	return 0;
}
