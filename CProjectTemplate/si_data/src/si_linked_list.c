#include "si_linked_list.h"

//const pointer, const pointer & value
void si_linked_list_new(si_linked_list* const p_list,
	const si_dynamic* const p_initial_value)
{
	p_list->next = NULL;
	if(NULL != p_initial_value)
	{
		// Copy value from initial value
		si_dynamic_new_3(&(p_list->dynamic), p_initial_value->element_size, 1u);
		si_dynamic_set(&(p_list->dynamic), 0u, p_initial_value->data);
	}
	else
	{
		// Initialize to an empty dynamic
		si_dynamic_new_3(&(p_list->dynamic), 0u, 0u);
	}
}

si_linked_list* si_linked_list_next(const si_linked_list* const p_list)
{
	si_linked_list* p_next = NULL;
	// Validate paramter
	if(NULL == p_list)
	{
		goto END;
	}
	p_next = p_list->next;
END:
	return p_next;
}

size_t si_linked_list_count(const si_linked_list* const p_list)
{
	size_t counter = 0u;
	// Validate parameter
	if(NULL == p_list)
	{
		goto END;
	}
	const si_linked_list* p_next = p_list;
	do
	{
		counter++;
	} while(NULL != (p_next = si_linked_list_next(p_next)));
END:
	return counter;
}

si_dynamic* si_linked_list_at(const si_linked_list* const p_list,
	const size_t index)
{
	const si_dynamic* p_data = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	size_t counter = 0u;
	const si_linked_list* p_next = p_list;
	do
	{
		if(counter == index)
		{
			p_data = &(p_next->dynamic);
			goto END;
		}
		counter++;
	} while(NULL != (p_next = si_linked_list_next(p_next)));
END:
	return (si_dynamic*)p_data;
}

si_dynamic* si_linked_list_last(const si_linked_list* const p_list)
{
	const size_t current_size = si_linked_list_count(p_list);
	si_dynamic* p_data = NULL;
	if(0u == current_size)
	{
		// Prevent Underflow
		goto END;
	}
	p_data = si_linked_list_at(p_list, current_size - 1u);
END:
	return p_data;
}

size_t si_linked_list_find_next_3(const si_linked_list* const p_list,
	const si_dynamic* const p_data, const size_t current_index)
{
	// This recursion implementation may be slower than iterative. (Profile me)
	size_t index = current_index;
	if((NULL == p_list) || (p_data == NULL))
	{
		goto END;
	}
	// Is the node at index's value = to needle p_data?
	if(0u == si_dynamic_cmp(si_linked_list_at(p_list, index), p_data))
	{
		goto END;
	}
	// Try next list node if defined.
	if(index == SIZE_MAX)
	{
		// Prevent Overflow
		goto END;
	}
	const size_t list_count = si_linked_list_count(p_list);
	if(index + 1u < list_count)
	{
		index = si_linked_list_find_next_3(p_list, p_data, index + 1u);
	}
END:
	return index;
}
inline size_t si_linked_list_find_next(const si_linked_list* const p_list,
	const si_dynamic* const p_data)
{
	// Default parameter current_index=0 (Assumes this is the root node)
	return si_linked_list_find_next_3(p_list, p_data, 0u);
}

bool si_linked_list_insert_next(si_linked_list* const p_list,
	const si_dynamic* const p_data)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_list) || (NULL == p_data))
	{
		goto END;
	}
	// Begin
	si_linked_list* next_node = calloc(1u, sizeof(si_linked_list));
	if(NULL == next_node)
	{
		goto END;
	}
	si_linked_list_new(next_node, p_data);
	if(NULL == p_list->next)
	{
		// Is leaf node
		p_list->next = next_node;
		result = true;
	}
	else
	{
		// Not leaf node
		next_node->next = p_list->next;
		p_list->next = next_node;
		result = true;
	}
	// End
END:
	return result;
}

bool si_linked_list_insert(si_linked_list* const p_list,
	const si_dynamic* const p_data, const size_t index)
{
	bool result = false;
	if((NULL == p_list) || (NULL == p_data))
	{
		goto END;
	}
	if(0u >= index)
	{
		// Self-Insert
		si_linked_list* next_node = calloc(1u, sizeof(si_linked_list));
		if(NULL == next_node)
		{
			goto END;
		}
		si_linked_list_new(next_node, p_data);
		void* current_data = p_list->dynamic.data;
		p_list->dynamic.data = next_node->dynamic.data;
		next_node->dynamic.data = current_data;
		if(NULL == p_list->next)
		{
			// Is lead node
			p_list->next = next_node;
		}
		else
		{
			// Not leaf node
			next_node->next = p_list->next;
			p_list->next = next_node;
		}
		result = true;
		goto END;
	}
	// Insert Next
	size_t current_index = 0u;
	si_linked_list* p_node = p_list;
	while(NULL != p_node)
	{
		if(current_index >= (index - 1u))
		{
			// Stop on node before index.
			break;
		}
		p_node = p_node->next;
		current_index += 1u;
	}
	result = si_linked_list_insert_next(p_node, p_data);
END:
	return result;
}

bool si_linked_list_append(si_linked_list* const p_list, const si_dynamic* const p_data)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_list) || (NULL == p_data))
	{
		goto END;
	}
	// Begin
	// Get last node to insert data after it(next)
	// Note: si_linked_list_last returns last value not last node.
	si_linked_list* last_node = p_list;
	while(NULL != last_node)
	{
		if(NULL == last_node->next)
		{
			break;
		}
		last_node = last_node->next;
	}
	result = si_linked_list_insert_next(last_node, p_data);
	// End
END:
	return result;
}

bool si_linked_list_remove_next(si_linked_list* const p_list)
{
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	if(NULL == p_list->next)
	{
		// Already Removed
		goto END;
	}
	si_linked_list* next_node = p_list->next;
	si_dynamic_free(&(next_node->dynamic));
	if(NULL != next_node->next)
	{
		// Continue the chain
		p_list->next = next_node->next;
		next_node->next = NULL;
	}
	free(next_node);
	result = true;
	// End
END:
	return result;
}

bool si_linked_list_remove_at(si_linked_list* const p_list, const size_t index)
{
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const size_t element_size = p_list->dynamic.element_size;
	si_linked_list* next_node = p_list->next;
	if(0u == index)
	{
		// Self-Remove
		if(NULL == next_node)
		{
			// Last value in list. Can't free self.
			memset(p_list->dynamic.data, 0, element_size);
			goto END;
		}
		void* next_data = next_node->dynamic.data;
		memcpy(p_list->dynamic.data, next_data, element_size);
		p_list->next = p_list->next->next;
		next_node->next = NULL;
		si_dynamic_free(&(next_node->dynamic));
		free(next_node);
		result = true;
		goto END;
	}
	// Next-Remove
	next_node = p_list;
	size_t index_counter = 1u;
	while(NULL != next_node)
	{
		if(index == index_counter)
		{
			break;
		}
		next_node = next_node->next;
		index_counter++;
	}
	if(NULL != next_node)
	{
		result = si_linked_list_remove_next(next_node);
	}
	// End
END:
	return result;
}

void si_linked_list_free(si_linked_list* const p_list)
{
	if(NULL == p_list)
	{
		goto END;
	}
	si_dynamic_free(&(p_list->dynamic));
	if(NULL != p_list->next)
	{
		si_linked_list_free(p_list->next);
		free(p_list->next);
	}
END:
	return;
}

void si_linked_list_fprint(const si_linked_list* const p_list, FILE* const p_file)
{
	// Validate parameters
	if((NULL == p_list) || (NULL == p_file))
	{
		goto END;
	}
	// Begin
	const si_linked_list* iterator = p_list;
	const size_t list_count = si_linked_list_count(iterator);
	fprintf(p_file, "{");
	for(size_t i = 0u; i < list_count; i++)
	{
		if(NULL == iterator->dynamic.data)
		{
			// Undefined data element.
			iterator = iterator->next;
			continue;
		}
		if(0u >= iterator->dynamic.capacity)
		{
			// Empty alloc
			iterator = iterator->next;
			continue;
		}
		// Ignores Byte order for simplicity.
		const size_t element_size = iterator->dynamic.element_size;
		for(size_t ii = 0u; ii < element_size; ii++)
		{
			const uint8_t next_byte = ((uint8_t*)iterator->dynamic.data)[ii];
			fprintf(p_file, "%02x", next_byte);
		}
		if((list_count - 1u) > i)
		{
			fprintf(p_file, ", ");
		}
		iterator = iterator->next;
	}
	fprintf(p_file, "}:%lu", list_count);
	// End
END:
	return;
}
