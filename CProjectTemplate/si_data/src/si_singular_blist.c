//si_singular_blist.c

#include "si_singular_blist.h"

void si_singular_blist_init_4(si_singular_blist_t* const p_list,
	const bool is_circular, const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const))
{
	if(NULL == p_list)
	{
		goto END;
	}
	p_list->count = 0u;
	p_list->p_tail = NULL;
	p_list->p_head = si_singular_list_new_2(is_circular, initial_capacity);
	if(NULL == p_list->p_head)
	{
		goto END;
	}
	p_list->p_tail = si_singular_list_last_node(p_list->p_head);
	p_list->capacity = initial_capacity;
	p_list->p_cmp_f = p_cmp_f;
END:
	return;
}
inline void si_singular_blist_init_3(si_singular_blist_t* const p_list,
	const bool is_circular, const size_t initial_capacity)
{
	// Default value of p_cmp_f = NULL
	si_singular_blist_init_4(p_list, is_circular, initial_capacity, NULL);
}
inline void si_singular_blist_init_2(si_singular_blist_t* const p_list,
	const bool is_circular)
{
	// Default value of initial_capacity
	si_singular_blist_init_3(p_list, is_circular,
		SI_SINGULAR_LIST_DEFAULT_CAPACITY);
}
inline void si_singular_blist_init(si_singular_blist_t* const p_list)
{
	// Default value of is_circular
	si_singular_blist_init_2(p_list, SI_SINGULAR_LIST_IS_CIRCULAR);
}

si_singular_blist_t* si_singular_blist_new_3(const bool is_circular,
    const size_t initial_capacity,
    int (*p_cmp_f)(const void* const, const void* const))
{
	si_singular_blist_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_singular_blist_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_singular_blist_init_4(p_new, is_circular, initial_capacity, p_cmp_f);
END:
	return p_new;
}
inline si_singular_blist_t* si_singular_blist_new_2(const bool is_circular,
    const size_t initial_capacity)
{
	// Default value p_cmp_f = NULL
	return si_singular_blist_new_3(is_circular, initial_capacity, NULL);
}
inline si_singular_blist_t* si_singular_blist_new_1(const bool is_circular)
{
	// Default value initial_capacity
	return si_singular_blist_new_2(is_circular,
		SI_SINGULAR_LIST_DEFAULT_CAPACITY);
}
inline si_singular_blist_t* si_singular_blist_new()
{
	// Default is_circular value
	return si_singular_blist_new_1(SI_SINGULAR_LIST_IS_CIRCULAR);
}

si_singular_list_t* si_singular_blist_node_at(const si_singular_blist_t* const p_list,
	const size_t index)
{
	si_singular_list_t* p_result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	p_result = si_singular_list_node_at(p_list->p_head, index);
END:
	return p_result;
}

void* si_singular_blist_at(const si_singular_blist_t* const p_list,
    const size_t index)
{
	void* p_result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	p_result = si_singular_list_at(p_list->p_head, index);
END:
	return p_result;
}

size_t si_singular_blist_find_3(const si_singular_blist_t* const p_list,
	const void* const p_data, const size_t start_index)
{
	size_t result = SIZE_MAX;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_find_4(p_list->p_head, p_data,
		p_list->p_cmp_f, start_index);
END:
	return result;
}
size_t si_singular_blist_find(const si_singular_blist_t* const p_list,
	const void* const p_data)
{
	// Default start_index = 0 (Starts here)
	return si_singular_blist_find_3(p_list, p_data, 0u);
}

bool si_singular_blist_sort(si_singular_blist_t* const p_list)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_sort(p_list->p_head, p_list->p_cmp_f);
END:
	return result;
}

bool si_singular_blist_insert(si_singular_blist_t* const p_list,
	const void* const p_data, const size_t index)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_insert(p_list->p_head, p_data, index);
	if(true == result)
	{
		p_list->count++;
		p_list->capacity++;
	}
END:
	return result;
}

void si_singular_blist_set_circular_2(si_singular_blist_t* const p_list,
	const bool is_circular)
{
	if(NULL == p_list)
	{
		goto END;
	}
	if(NULL == p_list->p_tail)
	{
		goto END;
	}
	p_list->p_tail->p_next = (is_circular) ? p_list->p_head : NULL;
END:
	return;
}
inline void si_singular_blist_set_circular(si_singular_blist_t* const p_list)
{
	// Default value for is_circular
	si_singular_blist_set_circular_2(p_list, SI_SINGULAR_LIST_IS_CIRCULAR);
}

bool si_singular_blist_is_circular(si_singular_blist_t* const p_list)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	if((NULL == p_list->p_head) || (NULL == p_list->p_tail))
	{
		goto END;
	}
	result = (p_list->p_tail->p_next == p_list->p_head);
END:
	return result;
}

size_t si_singular_blist_grow_by(si_singular_blist_t* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	if(NULL == p_list->p_tail)
	{
		goto END;
	}
	for(size_t i = 0u; i < amount; i++)
	{
		const bool success = si_singular_list_insert_next(p_list->p_tail, NULL);
		if(!success)
		{
			break;
		}
		result++;
	}
	p_list->p_tail = si_singular_list_last_node(p_list->p_tail);
END:
	return result;
}

size_t si_singular_blist_grow_to(si_singular_blist_t* const p_list,
	const size_t capacity)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	result = p_list->capacity;
	if(result >= capacity)
	{
		goto END;
	}
	result += si_singular_blist_grow_by(p_list, capacity - result);
END:
	return result;
}

size_t si_singular_blist_shrink_by(si_singular_blist_t* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_shrink_by(p_list->p_head, amount);
	if(0u < result)
	{
		p_list->p_tail = si_singular_list_last_node(p_list->p_head);
		p_list->capacity -= result;
		if(p_list->count > p_list->capacity)
		{
			p_list->count = p_list->capacity;
		}
	}
END:
	return result;
}

size_t si_singular_blist_shrink_to(si_singular_blist_t* const p_list,
    const size_t capacity)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_shrink_to(p_list->p_head, capacity);
	if(result != p_list->capacity)
	{
		p_list->p_tail = si_singular_list_last_node(p_list->p_head);
		p_list->capacity = result;
		if(p_list->count > p_list->capacity)
		{
			p_list->count = p_list->capacity;
		}
	}
END:
	return result;
}

void si_singular_blist_resize(si_singular_blist_t* const p_list,
	const size_t capacity)
{
	if(NULL == p_list)
	{
		goto END;
	}
	if(capacity >= p_list->capacity)
	{
		si_singular_blist_grow_to(p_list, capacity);
		goto END;
	}
	si_singular_blist_shrink_to(p_list, capacity);
END:
	return;
}

bool si_singular_blist_append(si_singular_blist_t* const p_list,
	const void* const p_data)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_insert_next(p_list->p_tail, p_data);
	if(result)
	{
		p_list->count++;
		p_list->capacity++;
	}
END:
	return result;
}

bool si_singular_blist_remove_at(si_singular_blist_t* p_list,
	const size_t index)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_singular_list_remove_at(p_list->p_head, index);
	if(result)
	{
		p_list->count--;
		p_list->capacity--;
	}
END:
	return result;
}

size_t si_singular_blist_push(si_singular_blist_t* const p_list,
	const void* const p_data)
{
	size_t result = SIZE_MAX;
	if(NULL == p_list)
	{
		goto END;
	}
	if(NULL == p_list->p_head)
	{
		goto END;
	}
	bool assigned = false;
	si_singular_list_t* p_iterator = p_list->p_head;
	do
	{
		if(false == assigned)
		{
			result++;
			if(NULL == p_iterator->p_data)
			{
				// Available spot. Assign data.
				p_iterator->p_data = (void*)p_data;
				p_list->count++;
				assigned = true;
			}
		}
		if((NULL == p_iterator->p_next) ||
			(p_list->p_head == p_iterator->p_next))
		{
			// This is the last node
			if(true == assigned)
			{
				p_list->p_tail = p_iterator;
				goto END;
			}
			break;
		}
		p_iterator = p_iterator->p_next;
	} while(NULL != p_iterator);
	// No available spot to assign. Append data instead.
	const bool success = si_singular_blist_append(p_list, p_data);
	if(success)
	{
		result = (p_list->count - 1u);
	}
	else
	{
		result = SIZE_MAX;
	}
END:
	return result;
}

void* si_singular_blist_pop(si_singular_blist_t* const p_list)
{
	void* p_result = NULL;
	if(NULL == p_result)
	{
		goto END;
	}
	p_result = si_singular_list_pop(p_list->p_head);
	if(NULL != p_result)
	{
		p_list->count--;
	}
END:
	return p_result;
}

void si_singular_blist_free(si_singular_blist_t* p_list)
{
	if(NULL == p_list)
	{
		goto END;
	}
	p_list->count = 0u;
	p_list->capacity = 0u;
	if(NULL != p_list->p_head)
	{
		si_singular_list_free(p_list->p_head);
	}
	p_list->p_head = NULL;
	p_list->p_tail = NULL;
	p_list->p_cmp_f = NULL;
	free(p_list);
END:
	return;
}

void si_singular_blist_free_at(si_singular_blist_t** const pp_list)
{
	if(NULL == pp_list)
	{
		goto END;
	}
	si_singular_blist_free(*pp_list);
	*pp_list = NULL;
END:
	return;
}
