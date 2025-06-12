#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "si_dynamic.h"
#include "si_queue.h"

// Set struct default values
void init_si_queue_3(si_queue* p_queue, const size_t element_size,
	const size_t initial_capacity)
{
	// Validate parameter
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	p_queue->front = 0u;
	p_queue->back  = 0u;
	p_queue->dynamic = (si_dynamic){};
	init_si_dynamic_3(&(p_queue->dynamic), element_size, initial_capacity);
	// End
END:
	return;
}
inline void init_si_queue(si_queue* p_queue, const size_t element_size)
{
	// Default initial_capacity is 0
	init_si_queue_3(p_queue, element_size, 0u);
}

// Count elements
size_t si_queue_count(const si_queue* p_queue)
{
	size_t result = 0u;
	// Validate parameter
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	if (p_queue->front <= p_queue->back)
	{
		result = p_queue->back - p_queue->front;
	}
	else
	{
		result = (p_queue->dynamic.capacity - p_queue->front) + p_queue->back;
	}
	// End
END:
	return result;
}

bool si_queue_is_empty(const si_queue* p_queue)
{
	bool is_empty = true;
	// Validate Parameter
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	is_empty = (si_queue_count(p_queue) == 0u);
	// End
END:
	return is_empty;
}

bool si_queue_is_full(const si_queue* p_queue)
{
	bool is_full = true;
	// Validate Parameter
	if (NULL == p_queue)
	{
		goto END;
	}
	if(0u == p_queue->dynamic.capacity)
	{
		goto END;
	}
	// Begin
	const size_t item_count = si_queue_count(p_queue);
	is_full = (item_count == (p_queue->dynamic.capacity - 1u));
	// End
END:
		return is_full;
}

// Returns new count of queue
size_t si_queue_enqueue(si_queue* p_queue, const void* p_item)
{
	size_t new_count = 0u;
	// Validate parameters
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	new_count = si_queue_count(p_queue);
	if (si_queue_is_full(p_queue))
	{
		si_dynamic_grow(&(p_queue->dynamic));
		if (si_queue_is_full(p_queue))
		{
			// Failed to grow.
			goto END;
		}
	}
	si_dynamic_set(&(p_queue->dynamic), p_queue->back, p_item);
	p_queue->back = (p_queue->back + 1) % p_queue->dynamic.capacity;
	// End
END:
	return new_count;
}

size_t si_queue_dequeue(si_queue* p_queue, void* p_item)
{
	size_t new_count = 0u;
	// Validate parameters
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	new_count = si_queue_count(p_queue);
	if (si_queue_is_empty(p_queue))
	{
		goto END;
	}
	si_dynamic_get(&(p_queue->dynamic), p_queue->front, p_item);
	p_queue->front = (p_queue->front + 1) % p_queue->dynamic.capacity;
	// End
END:
	return new_count;
}

void free_si_queue(si_queue* p_queue)
{
	// Validate parameter
	if (NULL == p_queue)
	{
		goto END;
	}
	// Begin
	free_si_dynamic(&(p_queue->dynamic));
	// End
END:
	return;
}
