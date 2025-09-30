// si_priority_queue.c
#include "si_priority_queue.h"

void si_priority_queue_init(si_priority_queue_t* const p_pqueue,
	const size_t priority_count)
{
	if (NULL == p_pqueue)
	{
		goto END;
	}
	if (0u >= priority_count)
	{
		goto END;
	}
	p_pqueue->p_settings = NULL;
	p_pqueue->p_free_value = NULL;
	si_parray_init_2(&(p_pqueue->locks), priority_count);
	si_parray_init_2(&(p_pqueue->queues), priority_count);

	// Initialize queue mutexs
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_mutex_new();
		if (NULL == p_lock)
		{
			break;
		}
		const size_t append_index = si_parray_append(&(p_pqueue->locks), p_lock);
		if (priority_count <= append_index)
		{
			break;
		}
	}
END:
	return;
}

si_priority_queue_t* si_priority_queue_new(const size_t priority_count)
{
	si_priority_queue_t* p_new = calloc(1u, sizeof(si_priority_queue_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_priority_queue_init(p_new, priority_count);
END:
	return p_new;
}

size_t si_priority_queue_priority_count(
	const si_priority_queue_t* const p_pqueue)
{
	size_t result = 0u;
	if (NULL == p_pqueue)
	{
		goto END;
	}
	result = p_pqueue->locks.array.capacity;
	if (p_pqueue->queues.array.capacity < result)
	{
		result = p_pqueue->queues.array.capacity;
	}
END:
	return result;
}

size_t si_priority_queue_count(const si_priority_queue_t* const p_pqueue)
{
	size_t result = SIZE_MAX;
	if (NULL == p_pqueue)
	{
		goto END;
	}
	result = 0u;
	const size_t priority_count = si_priority_queue_priority_count(p_pqueue);
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), iii);
		if (NULL == p_lock)
		{
			break;
		}
		si_mutex_lock(p_lock);
	}
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_queue_t* p_queue = si_parray_at(&(p_pqueue->queues), iii);
		if (NULL == p_queue)
		{
			continue;
		}
		result += si_queue_count(p_queue);
	}
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), iii);
		if (NULL == p_lock)
		{
			break;
		}
		si_mutex_unlock(p_lock);
	}
END:
	return result;
}

bool si_priority_queue_is_empty(const si_priority_queue_t* const p_pqueue)
{
	bool result = true;
	if (NULL == p_pqueue)
	{
		goto END;
	}
	const size_t priority_count = si_priority_queue_priority_count(p_pqueue);
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), iii);
		if (NULL == p_lock)
		{
			break;
		}
		si_mutex_lock(p_lock);
	}
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_queue_t* p_queue = si_parray_at(&(p_pqueue->queues), iii);
		if (NULL == p_queue)
		{
			continue;
		}
		const size_t next_count = si_queue_count(p_queue);
		if (0 < next_count)
		{
			result = false;
			break;
		}
	}
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), iii);
		if (NULL == p_lock)
		{
			break;
		}
		si_mutex_unlock(p_lock);
	}
END:
	return result;
}

/** Doxygen
 * @brief Increases the priority of all tasks in the queue from priority by
 *        amount to prevent starvation.
 * 
 * @param p_pqueue Pointer to the priority queue to preform reprioritization on
 * @param priority Priority level to change from as type size_t.
 * @param amount How much to increase the priority from that level by.
 * 
 * @return Returns number of entrys that have had their priority level changed.
 */
static size_t si_priority_queue_feed_at(si_priority_queue_t* const p_pqueue,
	const size_t priority, const size_t amount)
{
	size_t result = 0u;
	if ((NULL == p_pqueue) || (0u >= amount))
	{
		goto END;
	}
	if (p_pqueue->locks.array.capacity <= priority)
	{
		goto END;
	}
	size_t sink_index = priority + amount;
	// Handle overflow for ridiculous number of priority levels
	if (sink_index <= priority)
	{
		sink_index = SIZE_MAX;
	}

	si_mutex_t* const p_source_lock = si_parray_at(
		&(p_pqueue->locks), priority
	);
	si_mutex_t* const p_sink_lock = si_parray_at(
		&(p_pqueue->locks), sink_index
	);
	if ((NULL == p_source_lock) || (NULL == p_sink_lock))
	{
		goto END;
	}
	si_queue_t* const p_source_queue = si_parray_at(
		&(p_pqueue->queues), priority
	);
	si_queue_t* const p_sink_queue = si_parray_at(
		&(p_pqueue->queues), priority
	);
	if ((NULL == p_source_queue) || (NULL == p_sink_queue))
	{
		goto END;
	}

	// Lock source and sink queues
	si_mutex_lock(p_source_lock);
	si_mutex_lock(p_sink_lock);

	// Moves items from source to sink
	const size_t count_items = si_queue_count(p_source_queue);
	for (size_t iii = 0u; iii < count_items; iii++)
	{
		void* p_data = NULL;
		si_queue_dequeue(p_source_queue, &p_data);
		if (NULL == p_data)
		{
			continue;
		}
		si_queue_enqueue(p_sink_queue, &p_data);
		result++;
	}

	// Unlock source and sink queues
	si_mutex_unlock(p_sink_lock);
	si_mutex_unlock(p_source_lock);
END:
	return result;
}

size_t si_priority_queue_feed(si_priority_queue_t* const p_pqueue,
	const size_t amount)
{
	size_t result = 0u;
	if ((NULL == p_pqueue) || (0 >= amount))
	{
		goto END;
	}
	if (0 >= p_pqueue->locks.array.capacity)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < (p_pqueue->locks.array.capacity - 1u); iii++)
	{
		result += si_priority_queue_feed_at(p_pqueue, iii, amount);
	}
END:
	return result;
}

bool si_priority_queue_enqueue(si_priority_queue_t* const p_pqueue,
	void* const p_data, const size_t priority)
{
	bool result = false;
	if ((NULL == p_pqueue) || (NULL == p_data))
	{
		goto END;
	}
	if (p_pqueue->queues.array.capacity <= priority)
	{
		goto END;
	}
	si_queue_t* p_queue = si_parray_at(&(p_pqueue->queues), priority);
	si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), priority);
	if (NULL == p_lock)
	{
		goto END;
	}
	si_mutex_lock(p_lock);

	size_t queue_count = 0u;
	if (NULL == p_queue)
	{
		// Initialize queue
		p_queue = si_queue_new_3(sizeof(void*), 1u, p_pqueue->p_settings);
		si_parray_set(&(p_pqueue->queues), priority, p_queue);
	}
	else
	{
		queue_count = si_queue_count(p_queue);
	}
	// Validate queue was initialized.
	if (NULL == p_queue)
	{
		goto UNLOCK;
	}
	const size_t new_count = si_queue_enqueue(p_queue, &p_data);

	result = (new_count > queue_count);
UNLOCK:;
	si_mutex_unlock(p_lock);
END:
	return result;
}

/** Doxygen
 * @brief Attempts to dequeue data at priority level in p_pqueue.
 * 
 * @param p_pqueue Pointer to priority queue to dequeue from.
 * @param priority Priority level size_t to dequeue from.
 * 
 * @return Returns pointer value on success. Returns NULL otherwise.
 */
static void* si_priority_queue_dequeue_at(si_priority_queue_t* const p_pqueue,
	const size_t priority)
{
	void* p_result = NULL;
	if (NULL == p_pqueue)
	{
		goto END;
	}
	if (p_pqueue->locks.array.capacity <= priority)
	{
		goto END;
	}
	si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), priority);
	si_queue_t* const p_queue = si_parray_at(&(p_pqueue->queues), priority);
	if ((NULL == p_lock) || (NULL == p_queue))
	{
		goto END;
	}
	si_mutex_lock(p_lock);

	const size_t count = si_queue_count(p_queue);
	if (0 >= count)
	{
		goto UNLOCK;
	}
	si_queue_dequeue(p_queue, &p_result);
UNLOCK:;
	si_mutex_unlock(p_lock);
END:
	return p_result;
}

void* si_priority_queue_dequeue(si_priority_queue_t* const p_pqueue)
{
	void* p_result = NULL;
	if (NULL == p_pqueue)
	{
		goto END;
	}
	const size_t priority_count = si_priority_queue_priority_count(p_pqueue);
	if (0u >= priority_count)
	{
		goto END;
	}
	for (size_t iii = (priority_count - 1u);; iii--)
	{
		p_result = si_priority_queue_dequeue_at(p_pqueue, iii);
		if (NULL == p_result)
		{
			if (0u == iii)
			{
				break;
			}
			continue;
		}
		else
		{
			break;
		}
		if (0u == iii)
		{
			break;
		}
	}
END:
	return p_result;
}

void si_priority_queue_update_settings(si_priority_queue_t* const p_pqueue)
{
	if (NULL == p_pqueue)
	{
		goto END;
	}
	const size_t priority_count = si_priority_queue_priority_count(p_pqueue);
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_mutex_t* const p_lock = si_parray_at(&(p_pqueue->locks), iii);
		si_queue_t* const p_queue = si_parray_at(&(p_pqueue->queues), iii);
		if ((NULL == p_lock) || (NULL == p_queue))
		{
			continue;
		}
		si_mutex_lock(p_lock);
		p_queue->p_settings = p_pqueue->p_settings;
		si_mutex_unlock(p_lock);
	}
END:
	return;
}

/** Doxygen
 * @brief Attempts to free data at priority level in p_pqueue.
 * 
 * @param p_pqueue Pointer to priority queue to free at.
 * @param priority Priority level size_t to free at.
 */
static void si_priority_queue_free_at(si_priority_queue_t* const p_pqueue,
	const size_t priority)
{
	if (NULL == p_pqueue)
	{
		goto END;
	}
	si_mutex_t* p_lock = si_parray_at(&(p_pqueue->locks), priority);
	si_queue_t* p_queue = si_parray_at(&(p_pqueue->queues), priority);
	if (NULL != p_lock)
	{
		si_mutex_lock(p_lock);
	}
	if ((NULL != p_pqueue->p_free_value) && (NULL != p_queue))
	{
		void* p_data = NULL;
		const size_t count = si_queue_count(p_queue);
		for (size_t iii = 0u; iii < count; iii++)
		{
			si_queue_dequeue(p_queue, &p_data);
			if (NULL == p_data)
			{
				continue;
			}
			p_pqueue->p_free_value(p_data);
		}
	}
	si_queue_destroy(&p_queue);
	si_parray_set(&(p_pqueue->queues), priority, NULL);
	if (NULL != p_lock)
	{
		si_mutex_unlock(p_lock);
		si_mutex_destroy(&p_lock);
		si_parray_set(&(p_pqueue->locks), priority, NULL);
	}
END:
	return;
}

void si_priority_queue_free(si_priority_queue_t* const p_pqueue)
{
	if (NULL == p_pqueue)
	{
		goto END;
	}
	const size_t priority_count = si_priority_queue_priority_count(p_pqueue);
	for (size_t iii = 0u; iii < priority_count; iii++)
	{
		si_priority_queue_free_at(p_pqueue, iii);
	}
	si_parray_free(&(p_pqueue->locks));
	si_parray_free(&(p_pqueue->queues));
END:
	return;
}

void si_priority_queue_destroy(si_priority_queue_t** pp_pqueue)
{
	if (NULL == pp_pqueue)
	{
		goto END;
	}
	if (NULL == *pp_pqueue)
	{
		// Already freed
		goto END;
	}
	si_priority_queue_free(*pp_pqueue);
	free(*pp_pqueue);
	*pp_pqueue = NULL;
END:
	return;
}
