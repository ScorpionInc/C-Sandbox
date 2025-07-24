#include "si_linked_list.h"

bool si_linked_list_grow_by(si_linked_list_t* const p_list,
	const size_t amount)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_linked_list_t* p_tail = si_linked_list_last_node(p_list);
	for(size_t i = 0u; i < amount; i++)
	{
		result = si_linked_list_insert_next(p_tail, NULL);
		if(false == result)
		{
			break;
		}
		p_tail = p_tail->next;
	}
	result = true;
	// End
END:
	return result;
}

bool si_linked_list_grow_to(si_linked_list_t* const p_list,
	const size_t capacity)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const size_t current_capacity = si_linked_list_capacity(p_list);
	if(current_capacity >= capacity)
	{
		goto END;
	}
	result = si_linked_list_grow_by(p_list, capacity - current_capacity);
	// End
END:
	return result;
}

void si_linked_list_new_2(si_linked_list_t* const p_list,
	const size_t initial_capacity)
{
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	p_list->dynamic = (si_dynamic_t){};
	p_list->dynamic.data = NULL;
	p_list->next = NULL;
	si_linked_list_grow_to(p_list, initial_capacity);
	// End
END:
	return;
}
inline void si_linked_list_new(si_linked_list_t* const p_list)
{
	// Default initial_capacity = 1u
	si_linked_list_new_2(p_list, 1u);
}

size_t si_linked_list_count(const si_linked_list_t* const p_list)
{
	size_t counter = 0u;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* p_next = p_list;
	do
	{
		if(NULL != p_next->dynamic.data)
		{
			counter++;
		}
		if(p_list == p_next->next)
		{
			// Circular linked list
			break;
		}
	} while(NULL != (p_next = p_next->next));
	// End
END:
	return counter;
}

size_t si_linked_list_capacity(const si_linked_list_t* const p_list)
{
	size_t counter = 0u;
	// Validate parameter
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* p_next = p_list;
	do
	{
		counter++;
		if(p_list == p_next->next)
		{
			// Circular linked list
			break;
		}
	} while(NULL != (p_next = p_next->next));
	// End
END:
	return counter;
}

bool si_linked_list_is_empty(const si_linked_list_t* const p_list)
{
	bool result = true;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* p_node = p_list;
	do
	{
		if(NULL != p_node->dynamic.data)
		{
			result = false;
			break;
		}
		if(p_list == p_node->next)
		{
			// Circular linked list
			break;
		}
	} while(NULL != (p_node = p_node->next));
	// End
END:
	return result;
}

bool si_linked_list_is_full(const si_linked_list_t* const p_list)
{
	bool result = true;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* p_node = p_list;
	do
	{
		if(NULL == p_node->dynamic.data)
		{
			result = false;
			break;
		}
		if(p_list == p_node->next)
		{
			// Circular linked list
			break;
		}
	} while(NULL != (p_node = p_node->next));
	// End
END:
	return result;
}

si_linked_list_t* si_linked_list_node_at(const si_linked_list_t* const p_list,
	const size_t index)
{
	const si_linked_list_t* p_result = p_list;
	// Validate
	if(NULL == p_result)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < index; i++)
	{
		p_result = p_result->next;
		if(NULL == p_result)
		{
			break;
		}
	}
	// End
END:
	return p_result;
}

si_dynamic_t* si_linked_list_at(const si_linked_list_t* const p_list,
	const size_t index)
{
	const si_dynamic_t* p_data = NULL;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* const p_node = si_linked_list_node_at(p_list, index);
	if(NULL == p_node)
	{
		goto END;
	}
	p_data = &(p_node->dynamic);
	// End
END:
	return (si_dynamic_t*)p_data;
}

void* si_linked_list_raw_at(const si_linked_list_t* const p_list,
	const size_t index)
{
	void* p_result = NULL;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_dynamic_t* const p_dynamic = si_linked_list_at(p_list, index);
	if(NULL == p_dynamic)
	{
		goto END;
	}
	p_result = p_dynamic->data;
	// End
END:
	return p_result;
}

size_t si_linked_list_find_3(const si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t start_index)
{
	// This recursion implementation may be slower than iterative.
	// TODO (Profile function for speed / stack overflows)
	size_t index = SIZE_MAX;
	if((NULL == p_list) || (p_data == NULL))
	{
		goto END;
	}
	// Is the node at index's value = to needle p_data?
	if(0u == si_dynamic_cmp(si_linked_list_at(p_list, start_index), p_data))
	{
		index = start_index;
		goto END;
	}
	// Prevent Overflow
	if(start_index == SIZE_MAX)
	{
		goto END;
	}
	const size_t list_count = si_linked_list_count(p_list);
	if(start_index + 1u < list_count)
	{
		index = si_linked_list_find_3(p_list, p_data, start_index + 1u);
	}
END:
	return index;
}
inline size_t si_linked_list_find(const si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data)
{
	// Default parameter current_index=0 (Assumes this is the root node)
	return si_linked_list_find_3(p_list, p_data, 0u);
}

size_t si_linked_list_find_raw_4(const si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size, const size_t start_index)
{
	size_t index = SIZE_MAX;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	{
		si_dynamic_t needle_dynamic = (si_dynamic_t){};
		si_dynamic_new_3(&needle_dynamic, data_size, 1u);
		si_dynamic_set(&needle_dynamic, 0u, p_data);
		index = si_linked_list_find_3(p_list, &needle_dynamic, start_index);
	}
	// End
END:
	return index;
}
inline size_t si_linked_list_find_raw(const si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size)
{
	// Default value of start_index = 0u
	return si_linked_list_find_raw_4(p_list, p_data, data_size, 0u);
}

bool si_linked_list_assign_5(si_linked_list_t* const p_list,
    const si_dynamic_t* const p_dynamic, const bool deep_copy,
    const bool do_free, const size_t index)
{
	bool result = false;
	si_linked_list_t* p_node = si_linked_list_node_at(p_list, index);
	// Validate
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	// Remove old value at node.
	if((NULL != p_node->dynamic.data) && do_free)
	{
		si_dynamic_free(&(p_node->dynamic));
	}
	p_node->dynamic.data = NULL;
	// Assign new value at node.
	if(NULL != p_dynamic)
	{
		const size_t dynamic_size = si_dynamic_size(p_dynamic);
		si_dynamic_new_3(&(p_node->dynamic),
			p_dynamic->element_size, p_dynamic->capacity);
		if(true == deep_copy)
		{
			// Deep copy
			memcpy(p_node->dynamic.data, p_dynamic->data, dynamic_size);
		}
		else
		{
			// Shallow copy
			p_node->dynamic.data = p_dynamic->data;
		}
	}
	result = true;
	// End
END:
	return result;
}
inline bool si_linked_list_assign_4(si_linked_list_t* const p_list,
    const si_dynamic_t* const p_dynamic, const bool deep_copy,
    const bool do_free)
{
	// Default value of index = 0u
	return si_linked_list_assign_5(p_list, p_dynamic, deep_copy, do_free, 0u);
}
inline bool si_linked_list_assign_3(si_linked_list_t* const p_list,
    const si_dynamic_t* const p_dynamic, const bool deep_copy)
{
	// Default value of do_free = DI_LL_DEF_DO_FREE(true)
	return si_linked_list_assign_4(p_list, p_dynamic, deep_copy,
		SI_LINKED_LIST_DEFAULT_DO_FREE);
}
inline bool si_linked_list_assign(si_linked_list_t* const p_list,
    const si_dynamic_t* const p_dynamic)
{
	// Default value of deep_copy = SI_LL_DEF_DEEP_CPY_DYN(false)
	return si_linked_list_assign_3(p_list, p_dynamic,
		SI_LINKED_LIST_DEFAULT_DEEP_COPY);
}

bool si_linked_list_assign_raw_4(si_linked_list_t* const p_list,
    const void* const p_data, const size_t data_size, const bool do_free)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	// Remove old value at the node
	if((NULL != p_list->dynamic.data) && do_free)
	{
		si_dynamic_free(&(p_list->dynamic));
	}
	p_list->dynamic.data = NULL;
	// Assign new value if it exists
	if(p_data != NULL)
	{
		si_dynamic_new_3(&(p_list->dynamic), data_size, 1u);
		si_dynamic_set(&(p_list->dynamic), 0u, p_data);
	}
	result = true;
	// End
END:
	return result;
}
bool si_linked_list_assign_raw(si_linked_list_t* const p_list,
    const void* const p_data, const size_t data_size)
{
	// Default value of do_free = DI_LL_DEF_DO_FREE(true)
	return si_linked_list_assign_raw_4(p_list, p_data, data_size,
		SI_LINKED_LIST_DEFAULT_DO_FREE);
}

bool si_linked_list_insert_next_3(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const bool deep_copy)
{
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_linked_list_t* next_node = calloc(1u, sizeof(si_linked_list_t));
	if(NULL == next_node)
	{
		goto END;
	}
	si_linked_list_new(next_node);
	si_linked_list_assign_4(next_node, p_data, deep_copy, false);
	if(NULL == p_list->next)
	{
		// Is leaf node (assign next)
		p_list->next = next_node;
	}
	else
	{
		// Not leaf node (insert next)
		next_node->next = p_list->next;
		p_list->next = next_node;
	}
	result = true;
	// End
END:
	return result;
}
inline bool si_linked_list_insert_next(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data)
{
	// Default value of deep_copy = SI_LL_DFLT_DEEP_CPY_DYNAMICS(false)
	return si_linked_list_insert_next_3(p_list, p_data,
		SI_LINKED_LIST_DEFAULT_DEEP_COPY);
}

bool si_linked_list_insert_next_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_dynamic_t* p_new_dynamic = NULL;
	if((NULL != p_data) && (0u < data_size))
	{
		p_new_dynamic = calloc(1u, sizeof(si_dynamic_t));
	}
	if(NULL != p_new_dynamic)
	{
		si_dynamic_new_3(p_new_dynamic, data_size, 1u);
		si_dynamic_set(p_new_dynamic, 0u, p_data);
	}
	si_linked_list_insert_next_3(p_list, p_new_dynamic, true);
	// End
END:
	return result;
}

bool si_linked_list_insert_4(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t index, const bool deep_copy)
{
	bool result = false;
	si_linked_list_t* p_node = si_linked_list_node_at(p_list, index);
	// Validate
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	result = si_linked_list_insert_next_3(p_node, p_data, deep_copy);
	// End
END:
	return result;
}
inline bool si_linked_list_insert(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t index)
{
	// Default value of deep_copy = SI_LL_DFLT_DEEP_CPY_DYNAMICS(false)
	return si_linked_list_insert_4(p_list, p_data, index,
		SI_LINKED_LIST_DEFAULT_DEEP_COPY);
}

bool si_linked_list_insert_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size, const size_t index)
{
	bool result = false;
	si_linked_list_t* p_node = si_linked_list_node_at(p_list, index);
	// Validation
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	result = si_linked_list_insert_next_raw(p_node, p_data, data_size);
	// End
END:
	return result;
}

si_linked_list_t* si_linked_list_last_node(const si_linked_list_t* const p_list)
{
	si_linked_list_t* p_node = p_list;
	// Validation
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	while(NULL != p_node->next)
	{
		if(p_list == p_node->next)
		{
			// Circular linked list
			break;
		}
		p_node = p_node->next;
	}
	// End
END:
	return p_node;
}

si_linked_list_t* si_linked_list_next_available(const si_linked_list_t* const p_list)
{
	si_linked_list_t* p_node = p_list;
	// Validation
	if(NULL == p_node)
	{
		goto END;
	}
	// Begin
	do
	{
		if(NULL == p_node->dynamic.data)
		{
			goto END;
		}
		if(p_list == p_node->next)
		{
			// Circular linked list
			break;
		}
		p_node = p_node->next;
	} while(NULL != p_node);
	// End
END:
	return p_node;
}

bool si_linked_list_append_3(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const bool deep_copy)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_linked_list_t* p_node = si_linked_list_next_available(p_list);
	if(NULL == p_node)
	{
		// List is full.
		p_node = si_linked_list_last_node(p_list);
		result = si_linked_list_insert_next_3(p_node, p_data, deep_copy);
	}
	else
	{
		// List is not full.
		result = si_linked_list_assign_4(p_node, p_data, deep_copy, false);
	}
	// End
END:
	return result;
}
inline bool si_linked_list_append(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data)
{
	// Default value of deep_copy = SI_LL_DFLT_DEEP_CPY_DYNAMICS(false)
	return si_linked_list_append_3(p_list, p_data,
		SI_LINKED_LIST_DEFAULT_DEEP_COPY);
}

bool si_linked_list_append_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_linked_list_t* p_node = si_linked_list_next_available(p_list);
	if(NULL == p_node)
	{
		// List is full.
		p_node = si_linked_list_last_node(p_list);
		result = si_linked_list_insert_next_raw(p_node, p_data, data_size);
	}
	else
	{
		// List is not full. No need to do_free as data is NULL.
		result = si_linked_list_assign_raw_4(p_node, p_data, data_size, false);
	}
	// End
END:
	return result;
}

bool si_linked_list_remove_next_2(si_linked_list_t* const p_list,
	const bool do_free)
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
	si_linked_list_t* next_node = p_list->next;
	if(do_free)
	{
		si_dynamic_free(&(next_node->dynamic));
	}
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
inline bool si_linked_list_remove_next(si_linked_list_t* const p_list)
{
	// Default value of do_free = DI_LL_DEF_DO_FREE(true)
	return si_linked_list_remove_next_2(p_list,
		SI_LINKED_LIST_DEFAULT_DO_FREE);
}

bool si_linked_list_remove_2(si_linked_list_t* p_list, const bool do_free)
{
	bool result = false;
	// Validate
	if(NULL == p_list)
	{
		// Already Removed
		goto END;
	}
	// Begin Self-Remove
	if(do_free)
	{
		si_dynamic_free(&(p_list->dynamic));
	}
	p_list->dynamic.data = NULL;
	si_linked_list_t* p_next = p_list->next;
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
		// Shallow copy dynamic
		memcpy(&(p_list->dynamic), &(p_next->dynamic), sizeof(si_dynamic_t));
		p_next->dynamic.data = NULL;
		p_list->next = p_next->next;
		p_next->next = NULL;
		// Free old next.
		free(p_next);
	}
	result = true;
	// End
END:
	return result;
}
inline bool si_linked_list_remove(si_linked_list_t* p_list)
{
	// Default value of do_free = DI_LL_DEF_DO_FREE(true)
	return si_linked_list_remove_2(p_list,
		SI_LINKED_LIST_DEFAULT_DO_FREE);
}

bool si_linked_list_remove_at_3(si_linked_list_t* p_list,
	const size_t index, const bool do_free)
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
		result = si_linked_list_remove_2(p_list, do_free);
		goto END;
	}
	// Next-Remove
	si_linked_list_t* const p_parent = si_linked_list_node_at(p_list, index - 1);
	result = si_linked_list_remove_next(p_parent);
	// End
END:
	return result;
}
bool si_linked_list_remove_at(si_linked_list_t* p_list,
	const size_t index)
{
	// Default value of do_free = DI_LL_DEF_DO_FREE(true)
	return si_linked_list_remove_at_3(p_list, index,
		SI_LINKED_LIST_DEFAULT_DO_FREE);
}

void si_linked_list_free(si_linked_list_t* const p_list)
{
	// Validate
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	si_dynamic_free(&(p_list->dynamic));
	if(NULL != p_list->next)
	{
		si_linked_list_free(p_list->next);
		free(p_list->next);
	}
	// End
END:
	return;
}

void si_linked_list_fprint(const si_linked_list_t* const p_list, FILE* const p_file)
{
	// TODO
	// Validate parameters
	if((NULL == p_list) || (NULL == p_file))
	{
		goto END;
	}
	// Begin
	const si_linked_list_t* iterator = p_list;
	const size_t list_count = si_linked_list_count(iterator);
	const size_t list_capacity = si_linked_list_capacity(iterator);
	fprintf(p_file, "{");
	for(size_t i = 0u; i < list_capacity; i++)
	{
		// Ignores Byte order for simplicity.
		if(NULL == iterator->dynamic.data)
		{
			continue;
		}
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
	fprintf(p_file, "}:%lu/%lu", list_count, list_capacity);
	// End
END:
	return;
}
