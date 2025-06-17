#include "si_linked_list.h"

void si_linked_list_new(si_linked_list* p_list, const size_t element_size,
	const void* initial_value)
{
	si_dynamic_new_3(&(p_list->dynamic), element_size, 1u);
	si_dynamic_set(&(p_list->dynamic), 0u, initial_value);
	p_list->next = NULL;
}

size_t si_linked_list_count(const si_linked_list* p_list)
{
	size_t counter = 0u;
	// Validate Parameter
	if(NULL == p_list)
	{
		goto END;
	}
	counter = 1u;
	const si_linked_list* p_tmp = p_list;
	while(NULL != p_tmp->next)
	{
		p_tmp = p_tmp->next;
		counter++;
	}
END:
	return counter;
}

si_linked_list* si_linked_list_next(const si_linked_list* p_list)
{
	si_linked_list* p_tmp = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	p_tmp = p_list->next;
END:
	return p_tmp;
}

void* si_linked_list_at(const si_linked_list* p_list, const size_t index)
{
	void* p_data = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	if(0u == index)
	{
		p_data = p_list->dynamic.data;
		goto END;
	}
	size_t counter = 1u;
	const si_linked_list* p_tmp = p_list;
	while(NULL != p_tmp->next)
	{
		p_tmp = p_tmp->next;
		if(index == counter)
		{
			break;
		}
		counter++;
	}
	p_data = p_tmp->dynamic.data;
END:
	return p_data;
}

void* si_linked_list_last(const si_linked_list* p_list)
{
	const size_t current_size = si_linked_list_count(p_list);
	void* p_data = NULL;
	if(0u == current_size)
	{
		goto END;
	}
	p_data = si_linked_list_at(p_list, current_size - 1u);
END:
	return p_data;
}

size_t si_linked_list_find_next_3(const si_linked_list* p_list,
	const void* p_data, const size_t current_index)
{
	// Recursion implementation may be slower.
	size_t index = current_index;
	if((NULL == p_list) || (p_data == NULL))
	{
		goto END;
	}
	if(0u == memcmp(p_list->dynamic.data, p_data, p_list->dynamic.element_size))
	{
		goto END;
	}
	index = si_linked_list_find_next_3(p_list, p_data, current_index + 1u);
END:
	return index;
}
inline size_t si_linked_list_find_next(const si_linked_list* p_list,
	const void* p_data)
{
	// Default parameter current_index=0 (Assumes this is the root node)
	return si_linked_list_find_next_3(p_list, p_data, 0u);
}

bool si_linked_list_insert_next(si_linked_list* p_list, const void* p_data)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	si_linked_list* next_node = calloc(1u, sizeof(si_linked_list));
	if(NULL == next_node)
	{
		goto END;
	}
	si_linked_list_new(next_node, p_list->dynamic.element_size, p_data);
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
END:
	return result;
}

bool si_linked_list_insert(si_linked_list* p_list, const void* p_data,
	const size_t index)
{
	bool result = false;
	if((NULL == p_list) || (NULL == p_data))
	{
		goto END;
	}
	if(0u == index)
	{
		// Self-Insert
		si_linked_list* next_node = calloc(1u, sizeof(si_linked_list));
		if(NULL == next_node)
		{
			goto END;
		}
		si_linked_list_new(next_node, p_list->dynamic.element_size, p_data);
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
	while(p_node != NULL)
	{
		if(current_index >= (index - 1u))
		{
			break;
		}
		p_node = p_node->next;
		current_index += 1u;
	}
	result = si_linked_list_insert_next(p_node, p_data);
END:
	return result;
}

bool si_linked_list_append(si_linked_list* p_list, const void* p_data)
{
	//TODO
	bool result = false;
	// Validate parameters
	if((NULL == p_list) || (NULL == p_data))
	{
		goto END;
	}
	// Begin
	// End
END:
	return result;
}

bool si_linked_list_remove_next(si_linked_list* p_list)
{
	//TODO
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	// End
END:
	return result;
}

bool si_linked_list_remove_at(si_linked_list* p_list, const size_t index)
{
	//TODO
	bool result = false;
	// Validate parameters
	if(NULL == p_list)
	{
		goto END;
	}
	// Begin
	// End
END:
	return result;
}

void si_linked_list_free(si_linked_list* p_list)
{
	if(NULL == p_list)
	{
		goto END;
	}
	si_dynamic_free(&(p_list->dynamic));
	if(NULL != p_list->next)
	{
		si_linked_list_free(p_next);
		free(p_list->next);
	}
END:
	return;
}
