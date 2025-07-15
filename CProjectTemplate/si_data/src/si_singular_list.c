//si_singular_list.c

#include "si_singular_list.h"

void si_singular_list_init_3(si_singular_list* const p_list,
	const bool is_circular, const size_t initial_capacity)
{
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	p_list->p_data = NULL;
	p_list->p_next = is_circular ? p_list : NULL;
	si_singular_list_grow_to(p_list, initial_capacity);
	// End
END:
	return;
}
inline void si_singular_list_init_2(si_singular_list* const p_list,
	const bool is_circular)
{
	// Default value of initial_capacity = SI_SINGULAR_LIST_DEFAULT_CAPACITY(1)
	si_singular_list_init_3(p_list, is_circular,
		SI_SINGULAR_LIST_DEFAULT_CAPACITY);
}
inline void si_singular_list_init(si_singular_list* const p_list)
{
	// Default value of is_circular = SI_SINGULAR_LIST_IS_CIRCULAR(true)
	si_singular_list_init_2(p_list, SI_SINGULAR_LIST_IS_CIRCULAR);
}

si_singular_list* si_singular_list_new_2(const bool is_circular,
	const size_t initial_capacity)
{
	// NULL values are passed along by init function().
	si_singular_list* p_new = calloc(1u, sizeof(si_singular_list));
	si_singular_list_init_3(p_new, is_circular, initial_capacity);
	return p_new;
}
inline si_singular_list* si_singular_list_new_1(const bool is_circular)
{
	// Default value of initial_capacity = SI_SINGULAR_LIST_DEFAULT_CAPACITY(1)
	return si_singular_list_new_2(is_circular, SI_SINGULAR_LIST_DEFAULT_CAPACITY);
}
inline si_singular_list* si_singular_list_new()
{
	// Default value of is_circular = SI_SINGULAR_LIST_IS_CIRCULAR(true)
	return si_singular_list_new_1(SI_SINGULAR_LIST_IS_CIRCULAR);
}

size_t si_singular_list_count(const si_singular_list* const p_list)
{
	size_t counter = 0u;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_singular_list* p_next = p_list;
	do
	{
		counter++;
		if(p_list == p_next->p_next)
		{
			// Circular linked list
			break;
		}
	} while(NULL != (p_next = p_next->p_next));
	// End
END:
	return counter;
}

size_t si_singular_list_capacity(const si_singular_list* const p_list)
{
	size_t capacity = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	const si_singular_list* p_iterator = p_list;
	while(NULL != p_iterator)
	{
		capacity++;
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_list)
		{
			// Circular linked list
			break;
		}
	}
END:
	return capacity;
}

si_singular_list* si_singular_list_node_at(const si_singular_list* const p_list,
	const size_t index)
{
	const si_singular_list* p_result = p_list;
	// Validate
	if(NULL == p_result)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < index; i++)
	{
		p_result = p_result->p_next;
		if(NULL == p_result)
		{
			break;
		}
	}
	// End
END:
	return (si_singular_list*)p_result;
}

void* si_singular_list_at(const si_singular_list* const p_list,
	const size_t index)
{
	const void* p_data = NULL;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_singular_list* const p_node = si_singular_list_node_at(p_list, index);
	if(NULL == p_node)
	{
		goto END;
	}
	p_data = p_node->p_data;
	// End
END:
	return (void*)p_data;
}

size_t si_singular_list_find_4(const si_singular_list* const p_list,
	const void* const p_data, int (*p_cmp_f)(const void* const, const void* const),
	const size_t start_index)
{
	size_t index = SIZE_MAX;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	size_t counter = 0u;
	const si_singular_list* p_iterator = p_list;
	while(NULL != p_iterator)
	{
		// Test data
		if(0 == p_cmp_f(p_iterator->p_data, p_data))
		{
			index = counter;
			break;
		}
		// Step iterator/counter
		counter++;
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_list)
		{
			// Circular linked list
			break;
		}
	}
	// End
END:
	return index;
}
inline size_t si_singular_list_find(const si_singular_list* const p_list,
	const void* const p_data, int (*p_cmp_f)(const void* const, const void* const))
{
	// Default value of start_index = 0u (Current node)
	return si_singular_list_find_4(p_list, p_data, p_cmp_f, 0u);
}

bool si_singular_list_sort(si_singular_list* const p_list,
	int (*p_cmp_f)(const void*, const void*))
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	const size_t capacity = si_singular_list_capacity(p_list);
	if(1u >= capacity)
	{
		goto END;
	}
	si_singular_list* p_small_node = p_list;
	si_singular_list* p_test_node = NULL;
	// Simple sort
	for(size_t i = 0u; i < capacity; i++)
	{
		// i is the index for the next smallest value (ascending)
		for(size_t ii = 1u; ii < (capacity - i); ii++)
		{
			// ii is the number of possible indexs after i for smaller values
			p_test_node = si_singular_list_node_at(p_small_node, ii);
			if(0 > p_cmp_f(p_small_node->p_data, p_test_node->p_data))
			{
				// Swap values
				void* p_tmp = p_test_node->p_data;
				p_test_node->p_data = p_small_node->p_data;
				p_small_node->p_data = p_tmp;
			}
		}
		p_small_node = p_small_node->p_next;
	}
END:
	return result;
}

bool si_singular_list_insert_next(si_singular_list* const p_list,
	const void* const p_data)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_singular_list* next_node = si_singular_list_new_2(false, 1u);
	if(NULL == next_node)
	{
		goto END;
	}
	next_node->p_data = (void*)p_data;
	if(NULL == p_list->p_next)
	{
		// Is leaf node (assign next)
		p_list->p_next = next_node;
	}
	else
	{
		// Not leaf node (insert next)
		next_node->p_next = p_list->p_next;
		p_list->p_next = next_node;
	}
	result = true;
	// End
END:
	return result;
}

bool si_singular_list_insert(si_singular_list* const p_list,
	const void* const p_data, const size_t index)
{
	bool result = false;
	si_singular_list* p_node = si_singular_list_node_at(p_list, index);
	// Validation
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	result = si_singular_list_insert_next(p_node, p_data);
	// End
END:
	return result;
}

si_singular_list* si_singular_list_last_node(const si_singular_list* const p_list)
{
	const si_singular_list* p_node = p_list;
	// Validation
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	while(NULL != p_node->p_next)
	{
		if(p_list == p_node->p_next)
		{
			// Circular linked list
			break;
		}
		p_node = p_node->p_next;
	}
	// End
END:
	return (si_singular_list*)p_node;
}

void si_singular_list_set_circular_2(si_singular_list* const p_list,
	const bool is_circular)
{
	si_singular_list* p_last = si_singular_list_last_node(p_list);
	if(NULL == p_last)
	{
		goto END;
	}
	p_last->p_next = is_circular ? p_list : NULL;
END:
	return;
}
inline void si_singular_list_set_circular(si_singular_list* const p_list)
{
	// Default value of is_circular = SI_SINGULAR_LIST_IS_CIRCULAR(true)
	si_singular_list_set_circular_2(p_list, SI_SINGULAR_LIST_IS_CIRCULAR);
}

bool si_singular_list_is_circular(si_singular_list* const p_list)
{
	bool result = false;
	si_singular_list* p_last = si_singular_list_last_node(p_list);
	if(NULL == p_last)
	{
		goto END;
	}
	result = (p_last->p_next == p_list);
END:
	return result;
}

size_t si_singular_list_grow_by(si_singular_list* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	si_singular_list* p_last = si_singular_list_last_node(p_list);
	if(NULL == p_last)
	{
		goto END;
	}
	for(size_t i = 0u; i < amount; i++)
	{
		const bool success = si_singular_list_insert_next(p_last, NULL);
		if(!success)
		{
			break;
		}
		result++;
	}
END:
	return result;
}

size_t si_singular_list_grow_to(si_singular_list* const p_list,
	const size_t capacity)
{
	size_t result = 0u;
	if((NULL == p_list) || (capacity <= 1u))
	{
		goto END;
	}
	result = si_singular_list_capacity(p_list);
	if(result >= capacity)
	{
		goto END;
	}
	result += si_singular_list_grow_by(p_list, capacity - result);
END:
	return result;
}

size_t si_singular_list_shrink_by(si_singular_list* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	for(size_t i = 0u; i < amount; i++)
	{
		si_singular_list* const p_last = si_singular_list_last_node(p_list);
		if(NULL == p_last)
		{
			break;
		}
		const bool success = si_singular_list_remove_next(p_last);
		if(!success)
		{
			break;
		}
		result++;
	}
END:
	return result;
}

size_t si_singular_list_shrink_to(si_singular_list* const p_list,
	const size_t capacity)
{
	size_t result = 0u;
	if((NULL == p_list) || (capacity <= 1u))
	{
		goto END;
	}
	result = si_singular_list_capacity(p_list);
	if(result <= capacity)
	{
		goto END;
	}
	result -= si_singular_list_shrink_by(p_list, result - capacity);
END:
	return result;
}

void si_singular_list_resize(si_singular_list* const p_list,
	const size_t capacity)
{
	if(NULL == p_list)
	{
		goto END;
	}
	const size_t current_capacity = si_singular_list_capacity(p_list);
	if(current_capacity >= capacity)
	{
		si_singular_list_shrink_by(p_list, current_capacity - capacity);
	}
	else
	{
		si_singular_list_grow_by(p_list, capacity - current_capacity);
	}
END:
	return;
}

bool si_singular_list_append(si_singular_list* const p_list,
	const void* const p_data)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_singular_list* p_node = si_singular_list_last_node(p_list);
	if(NULL == p_node)
	{
		goto END;
	}
	result = si_singular_list_insert_next(p_node, p_data);
	// End
END:
	return result;
}

bool si_singular_list_remove_next(si_singular_list* const p_list)
{
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	if(NULL == p_list->p_next)
	{
		// Already Removed
		goto END;
	}
	si_singular_list* next_node = p_list->p_next;
	if(NULL != next_node->p_next)
	{
		// Continue the chain
		p_list->p_next = next_node->p_next;
		next_node->p_next = NULL;
	}
	free(next_node);
	result = true;
	// End
END:
	return result;
}

bool si_singular_list_remove(si_singular_list* p_list)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		// Already Removed
		goto END;
	}
	// Begin Self-Remove
	p_list->p_data = NULL;
	si_singular_list* p_next = p_list->p_next;
	if(NULL == p_next)
	{
		// Can't redefine self as next. We free self.
		// Alternively could just leave as empty node?
		free(p_list);
		p_list = NULL;
	}
	else
	{
		// Redefine self via next.
		// Shallow copy
		p_list->p_data = p_next->p_data;
		p_next->p_data = NULL;
		p_list->p_next = p_next->p_next;
		p_next->p_next = NULL;
		// Free old next.
		free(p_next);
	}
	result = true;
	// End
END:
	return result;
}

bool si_singular_list_remove_at(si_singular_list* p_list,
	const size_t index)
{
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	if(0u == index)
	{
		// Self-Remove
		result = si_singular_list_remove(p_list);
		goto END;
	}
	// Next-Remove
	si_singular_list* const p_parent = si_singular_list_node_at(p_list, index - 1);
	result = si_singular_list_remove_next(p_parent);
	// End
END:
	return result;
}

size_t si_singular_list_push(si_singular_list* const p_list,
	const void* const p_data)
{
	size_t result = SIZE_MAX;
	if(NULL == p_list)
	{
		goto END;
	}
	si_singular_list* p_iterator = p_list;
	do
	{
		result++;
		if(NULL == p_iterator->p_data)
		{
			// Assign
			p_iterator->p_data = p_data;
			goto END;
		}
		p_iterator = p_iterator->p_next;
		if(p_list == p_iterator)
		{
			// Circular linked list
			break;
		}
	} while(NULL != p_iterator);
	// Can't assign to a node, so instead we append.
	si_singular_list_append(p_list, p_data);
END:
	return result;
}

void* si_singular_list_pop(si_singular_list* const p_list)
{
	void* result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	si_singular_list* p_iterator = p_list;
	si_singular_list* p_next = p_list->p_next;
	while(NULL != p_next)
	{
		if(NULL == p_next->p_data)
		{
			// p_iterator is the last contiguous node with data
			result = p_iterator->p_data;
			p_iterator->p_data = NULL;
			goto END;
		}
		p_iterator = p_next;
		p_next = p_iterator->p_next;
	}
	// Reached end of list
	result = p_iterator->p_data;
	p_iterator->p_data = NULL;
END:
	return result;
}

void si_singular_list_free(si_singular_list* const p_list)
{
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	free(p_list->p_data);
	p_list->p_data = NULL;
	if(NULL != p_list->p_next)
	{
		si_singular_list_free(p_list->p_next);
		p_list->p_next = NULL;
	}
	free(p_list);
	// End
END:
	return;
}

void si_singular_list_free_at(si_singular_list** const pp_list)
{
	if(NULL == pp_list)
	{
		goto END;
	}
	si_singular_list_free(*pp_list);
	*pp_list = NULL;
END:
	return;
}

void si_singular_list_fprint(const si_singular_list* const p_list, FILE* const p_file)
{
	// Validate parameters
	if((NULL == p_list) || (NULL == p_file))
	{
		goto END;
	}
	// Begin
	const si_singular_list* p_iterator = p_list;
	size_t print_count = 0u;
	const size_t list_count = si_singular_list_count(p_iterator);
	const size_t list_capacity = si_singular_list_capacity(p_iterator);
	fprintf(p_file, "{");
	for(size_t i = 0u; i < list_capacity; i++)
	{
		// Ignores Byte order for simplicity.
		if(NULL == p_iterator->p_data)
		{
			continue;
		}
		fprintf(p_file, "%p", p_iterator->p_data);
		if(print_count < list_count)
		{
			fprintf(p_file, ", ");
		}
		p_iterator = p_iterator->p_next;
	}
	fprintf(p_file, "}:%lu", list_count);
	// End
END:
	return;
}
