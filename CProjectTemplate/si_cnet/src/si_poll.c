// si_poll.c
#include "si_poll.h"

void si_poll_init_3(si_poll_t* const p_poll, const size_t capacity,
	const short events)
{
	if (NULL == p_poll)
	{
		goto END;
	}
	p_poll->capacity = capacity;
	si_array_init_3((si_array_t*)p_poll, sizeof(si_poll_info), capacity);
	for (size_t iii = 0u; iii < capacity; iii++)
	{
		si_poll_info* const p_next = si_array_at((si_array_t*)p_poll, iii);
		if (NULL == p_next)
		{
			break;
		}
		p_next->fd = si_socket_invalid;
		p_next->events = events;
	}
END:
	return;
}
inline void si_poll_init(si_poll_t* const p_poll, const size_t capacity)
{
	// Default value of events is 0(Poll for no event types)
	si_poll_init_3(p_poll, capacity, 0);
}

si_poll_t* si_poll_new_2(const size_t capacity, const short events)
{
	si_poll_t* p_result = NULL;
	if (0u >= capacity)
	{
		goto END;
	}
	p_result = calloc(capacity, sizeof(si_poll_info));
	if (NULL == p_result)
	{
		goto END;
	}
	si_poll_init_3(p_result, capacity, events);
END:
	return p_result;
}
inline si_poll_t* si_poll_new(const size_t capacity)
{
	// Default value of events is 0(Poll for no event types)
	return si_poll_new_2(capacity, 0);
}

size_t si_poll_count_4(const si_poll_t* const p_poll, const short mask,
	const size_t start, const size_t count)
{
	size_t result = 0u;
	if (NULL == p_poll)
	{
		goto END;
	}
	if (p_poll->capacity <= start)
	{
		goto END;
	}
	size_t mut_count = count;
	const size_t remainder = (p_poll->capacity - start);
	if (remainder < count)
	{
		mut_count = remainder;
	}
	for (size_t iii = start; iii < mut_count; iii++)
	{
		const si_poll_info* const p_next = si_array_at(
			(si_array_t*)p_poll, iii
		);
		if (NULL == p_next)
		{
			break;
		}
		const bool is_valid = si_socket_is_valid(&(p_next->fd));
		bool pass_filter = true;
		if (0 != mask)
		{
			pass_filter = (0 < (mask & p_next->revents));
		}
		if ((true == is_valid) && (true == pass_filter))
		{
			result++;
		}
	}
END:
	return result;
}
inline size_t si_poll_count_3(const si_poll_t* const p_poll, const short mask,
	const size_t start)
{
	// Default value of count is SIZE_MAX(All array elements after start)
	return si_poll_count_4(p_poll, mask, start, SIZE_MAX);
}
inline size_t si_poll_count_2(const si_poll_t* const p_poll, const short mask)
{
	// Default value of start is 0(Start from beginning)
	return si_poll_count_3(p_poll, mask, 0u);
}
inline size_t si_poll_count(const si_poll_t* const p_poll)
{
	// Default value of mask is 0(Count all valid sockets)
	return si_poll_count_2(p_poll, 0);
}

size_t si_poll_find(const si_poll_t* const p_poll,
	const si_socket_t* p_socket)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_poll) || (NULL == p_socket))
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_poll->capacity; iii++)
	{
		const si_poll_info* const p_next = si_array_at((si_array_t*)p_poll, iii);
		if (NULL == p_next)
		{
			break;
		}
		if (*p_socket == p_next->fd)
		{
			result = iii;
			break;
		}
	}
END:
	return result;
}

bool si_poll_append_4(si_poll_t* const p_poll,
	const si_socket_t* const p_socket, const short events,
	const si_realloc_settings_t* const p_settings)
{
	bool result = false;
	if ((NULL == p_poll) || (NULL == p_socket))
	{
		goto END;
	}
	const si_socket_t invalid_socket = si_socket_invalid;
	size_t open_index = si_poll_find(p_poll, &invalid_socket);
	if (SIZE_MAX == open_index)
	{
		// No available index. Attempt to grow capacity.
		bool did_grow = false;
		if (NULL == p_settings)
		{
			// Linear grow 1 capacity
			did_grow = si_array_resize((si_array_t*)p_poll, p_poll->capacity + 1u);
		}
		else
		{
			// Use provided si_realloc_settings to grow
			did_grow = si_realloc_settings_grow(p_settings, (si_array_t*)p_poll);
		}
		if (true != did_grow)
		{
			// Failed to grow.
			goto END;
		}
		open_index = si_poll_find(p_poll, &invalid_socket);
		if (SIZE_MAX == open_index)
		{
			// Still can't find open index.
			goto END;
		}
	}
	// Available index
	si_poll_info* const p_next = si_array_at((si_array_t*)p_poll, open_index);
	if (NULL == p_next)
	{
		// Out of bounds
		goto END;
	}
	p_next->fd = *p_socket;
	p_next->events = events;
	result = true;
END:
	return result;
}
inline bool si_poll_append(si_poll_t* const p_poll,
	const si_socket_t* const p_socket, const short events)
{
	// Default value of p_settings is NULL(Grow capacity linearly by one)
	return si_poll_append_4(p_poll, p_socket, events, NULL);
}

si_poll_info* si_poll_at(const si_poll_t* const p_poll, const size_t index)
{
	si_poll_info* p_result = NULL;
	if (NULL == p_poll)
	{
		goto END;
	}
	if (p_poll->capacity <= index)
	{
		goto END;
	}
	p_result = si_array_at((si_array_t*)p_poll, index);
END:
	return p_result;
}

int si_poll_update(si_poll_t* const p_poll, const int timeout)
{
	int result = si_poll_error;
	if (NULL == p_poll)
	{
		goto END;
	}
	if ((0u >= p_poll->capacity) || (NULL == p_poll->p_data))
	{
		goto END;
	}
#ifdef SI_UNIX
	result =    poll(p_poll->p_data, p_poll->capacity, timeout);
#elif defined(_WIN32)
	result = WSAPoll(p_poll->p_data, p_poll->capacity, timeout);
#else
#warning Unknown/Unsupported OS
#endif// OS Specific implementation
END:
	return result;
}

void si_poll_close_at(si_poll_t* const p_poll, const size_t index)
{
	si_poll_info* const p_target = si_poll_at(p_poll, index);
	if (NULL == p_target)
	{
		goto END;
	}
	si_socket_close(&(p_target->fd));
END:
	return;
}

bool si_poll_close(si_poll_t* const p_poll, const si_socket_t socket)
{
	bool result = true;
	if (NULL == p_poll)
	{
		goto END;
	}
	size_t target_index = si_poll_find(p_poll, &socket);
	while (SIZE_MAX != target_index)
	{
		result = true;
		si_poll_close_at(p_poll, target_index);
		target_index = si_poll_find(p_poll, &socket);
	}
END:
	return result;
}

void si_poll_close_all(si_poll_t* const p_poll)
{
	if (NULL == p_poll)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_poll->capacity; iii++)
	{
		si_poll_close_at(p_poll, iii);
	}
END:
	return;
}

void si_poll_free(si_poll_t* const p_poll)
{
	if (NULL == p_poll)
	{
		goto END;
	}
	si_poll_close_all(p_poll);
	si_array_free((si_array_t*)p_poll);
END:
	return;
}

void si_poll_destroy(si_poll_t** const pp_poll)
{
	if (NULL == pp_poll)
	{
		goto END;
	}
	if (NULL == *pp_poll)
	{
		// Already freed
		goto END;
	}
	si_poll_free(*pp_poll);
	free(*pp_poll);
	*pp_poll = NULL;
END:
	return;
}