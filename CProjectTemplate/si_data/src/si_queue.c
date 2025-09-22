#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "si_queue.h"

void si_queue_init_4(si_queue_t* const p_queue, const size_t element_size,
    const size_t initial_capacity, const si_realloc_settings_t* p_settings)
{
	if (NULL == p_queue)
	{
		goto END;
	}
	p_queue->front = 0u;
	p_queue->back  = 0u;
	p_queue->p_settings = p_settings;
	p_queue->array = (si_array_t){0};
	si_array_init_3(
		&(p_queue->array), element_size, (initial_capacity + 1u)
	);
END:
	return;
}
inline void si_queue_init_3(si_queue_t* const p_queue,
	const size_t element_size, const size_t initial_capacity)
{
	// Default p_settings value is NULL (initializes with defaults)
	si_queue_init_4(p_queue, element_size, initial_capacity, NULL);
}
inline void si_queue_init(si_queue_t* const p_queue, const size_t element_size)
{
	// Default initial_capacity is 0
	si_queue_init_3(p_queue, element_size, 0u);
}

si_queue_t* si_queue_new_3(const size_t element_size,
	const size_t initial_capacity, const si_realloc_settings_t* p_settings)
{
	si_queue_t* p_new = calloc(1u, sizeof(si_queue_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_queue_init_4(p_new, element_size, initial_capacity, p_settings);
END:
	return p_new;
}
inline si_queue_t* si_queue_new_2(const size_t element_size,
	const size_t initial_capacity)
{
	// Default p_settings value is NULL (initializes with defaults)
	return si_queue_new_3(element_size, initial_capacity, NULL);
}
inline si_queue_t* si_queue_new(const size_t element_size)
{
	// Default initial_capacity is 0
	return si_queue_new_2(element_size, 0u);
}

size_t si_queue_count(const si_queue_t* const p_queue)
{
	size_t result = 0u;
	if (NULL == p_queue)
	{
		goto END;
	}
	if (p_queue->front <= p_queue->back)
	{
		result = p_queue->back - p_queue->front;
	}
	else
	{
		result = (p_queue->array.capacity - p_queue->front) + p_queue->back;
	}
END:
	return result;
}

size_t si_queue_capacity(const si_queue_t* const p_queue)
{
	/* Queue's internal array uses one extra capacity to track the end of the
	 * queue. (Kinda like a NULL terminator) So the capacity of items that can
	 * be stored is the array.capacity - 1u.
	 */
	size_t result = 0u;
	if (NULL == p_queue)
	{
		goto END;
	}
	if (1u >= p_queue->array.capacity)
	{
		goto END;
	}
	result = (p_queue->array.capacity - 1u);
END:
	return result;
}

bool si_queue_is_empty(const si_queue_t* const p_queue)
{
	bool is_empty = true;
	if (NULL == p_queue)
	{
		goto END;
	}
	if (1u >= p_queue->array.capacity)
	{
		goto END;
	}
	const size_t item_count = si_queue_count(p_queue);
	is_empty = (0u == item_count);
END:
	return is_empty;
}

bool si_queue_is_full(const si_queue_t* const p_queue)
{
	bool is_full = true;
	if (NULL == p_queue)
	{
		goto END;
	}
	if (1u >= p_queue->array.capacity)
	{
		goto END;
	}
	const size_t item_count = si_queue_count(p_queue);
	is_full = (item_count >= (p_queue->array.capacity - 1u));
END:
		return is_full;
}

size_t si_queue_enqueue(si_queue_t* const p_queue, const void* const p_item)
{
	size_t new_count = 0u;
	if (NULL == p_queue)
	{
		goto END;
	}
	new_count = si_queue_count(p_queue);
	const bool needs_to_grow = si_queue_is_full(p_queue);
	if (true == needs_to_grow)
	{
		const bool did_settings_grow = si_realloc_settings_grow(
			p_queue->p_settings, &(p_queue->array)
		);
		bool is_still_full = si_queue_is_full(p_queue);
		if ((true == did_settings_grow) && (true == is_still_full))
		{
			// Failed to grow with defined settings.
			// Or growth amount didn't increase capacity(0).
			goto END;
		}
		else if (true != did_settings_grow)
		{
			// Settings failed or missing, try using a default grow by 1.
			const bool did_grow = si_array_resize(
				&(p_queue->array), p_queue->array.capacity + 1u
			);
			is_still_full = si_queue_is_full(p_queue);
			if ((true != did_grow) || (true == is_still_full))
			{
				// All else has failed.
				goto END;
			}
		}
	}
	si_array_set(&(p_queue->array), p_queue->back, p_item);
	p_queue->back = (p_queue->back + 1) % p_queue->array.capacity;
	new_count++;
END:
	return new_count;
}

size_t si_queue_dequeue(si_queue_t* const p_queue, void* const p_item)
{
	size_t new_count = 0u;
	if ((NULL == p_queue) || (NULL == p_item))
	{
		goto END;
	}
	new_count = si_queue_count(p_queue);
	if (0u >= new_count)
	{
		goto END;
	}
	si_array_get(&(p_queue->array), p_queue->front, p_item);
	p_queue->front = (p_queue->front + 1) % p_queue->array.capacity;
	new_count--;
END:
	return new_count;
}

void si_queue_free(si_queue_t* const p_queue)
{
	if (NULL == p_queue)
	{
		goto END;
	}
	si_array_free(&(p_queue->array));
END:
	return;
}

void si_queue_free_at(si_queue_t** pp_queue)
{
	if (NULL == pp_queue)
	{
		goto END;
	}
	if (NULL == *pp_queue)
	{
		// Already freed
		goto END;
	}
	si_queue_free(*pp_queue);
	free(*pp_queue);
	*pp_queue = NULL;
END:
	return;
}

void si_queue_fprint(FILE* const p_file, const si_queue_t* const p_queue)
{
	if (NULL == p_file)
	{
		goto END;
	}
	if (NULL == p_queue)
	{
		fprintf(p_file, "NULL");
		goto END;
	}
	const size_t count = si_queue_count(p_queue);
	fprintf(p_file, "{front: %lu;", p_queue->front);
	fprintf(p_file, " back: %lu;", p_queue->back);
	fprintf(p_file, "}:%lu/%lu@%p", count, p_queue->array.capacity, p_queue);
END:
	return;
}