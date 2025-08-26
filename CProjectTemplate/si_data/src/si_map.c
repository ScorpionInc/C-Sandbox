//si_map.c

#include "si_map.h"

si_map_pair_t* si_map_pair_new(const void* const p_key,
	const void* const p_value)
{
	si_map_pair_t* p_new = NULL;
	if(NULL == p_key)
	{
		goto END;
	}
	p_new = calloc(1u, sizeof(si_map_pair_t));
	if(NULL == p_new)
	{
		goto END;
	}
	p_new->p_key = (void*)p_key;
	p_new->p_value = (void*)p_value;
END:
	return p_new;
}


static inline int si_map_default_compare(const void* const p_left,
	const void* const p_right)
{
	// Simple pointer compare.
	return (p_left == p_right);
}

void si_map_init(si_map_t* const p_map)
{
	if(NULL == p_map)
	{
		goto END;
	}
	si_array_init_3(&p_map->entries, sizeof(void*), 0u);
	p_map->p_settings = NULL;
	p_map->p_cmp_key_f = si_map_default_compare;
	p_map->p_cmp_value_f = si_map_default_compare;
	p_map->p_free_key_f = NULL;
	p_map->p_free_value_f = NULL;
END:
	return;
}

si_map_t* si_map_new()
{
	si_map_t* p_new = calloc(1u, sizeof(si_map_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_map_init(p_new);
END:
	return p_new;
}

size_t si_map_count(const si_map_t* const p_map)
{
	size_t result = SIZE_MAX;
	// Validation
	if(NULL == p_map)
	{
		goto END;
	}
	if(NULL == p_map->entries.p_data)
	{
		goto END;
	}
	// Begin
	result++;
	for(size_t i = 0u; i < p_map->entries.capacity; i++)
	{
		si_map_pair_t** pp_next = si_array_at(&p_map->entries, i);
		if(NULL == pp_next)
		{
			continue;
		}
		if(NULL != *pp_next)
		{
			result++;
		}
	}
	// End
END:
	return result;
}

size_t si_map_index_of(const si_map_t* const p_map, const void* const p_key)
{
	size_t result = SIZE_MAX;
	// Validation
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	if(NULL == p_map->entries.p_data)
	{
		goto END;
	}
	if(NULL == p_map->p_cmp_key_f)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < p_map->entries.capacity; i++)
	{
		si_map_pair_t** pp_pair = si_array_at(&p_map->entries, i);
		if(NULL == pp_pair)
		{
			continue;
		}
		if(NULL == *pp_pair)
		{
			continue;
		}
		if(0 == p_map->p_cmp_key_f((*pp_pair)->p_key, p_key))
		{
			// Key Match Found!
			result = i;
			break;
		}
	}
	// End
END:
	return result;
}

size_t si_map_find(const si_map_t* const p_map, const void* const p_value)
{
	size_t result = SIZE_MAX;
	// Validation
	if((NULL == p_map) || (NULL == p_value))
	{
		goto END;
	}
	if(NULL == p_map->entries.p_data)
	{
		goto END;
	}
	if(NULL == p_map->p_cmp_value_f)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < p_map->entries.capacity; i++)
	{
		si_map_pair_t** pp_pair = si_array_at(&p_map->entries, i);
		if(NULL == pp_pair)
		{
			continue;
		}
		if(NULL == *pp_pair)
		{
			continue;
		}
		if(0 == p_map->p_cmp_value_f((*pp_pair)->p_value, p_value))
		{
			// Value Match Found!
			result = i;
			break;
		}
	}
	// End
END:
	return result;
}

void* si_map_at(si_map_t* const p_map, const void* const p_key)
{
	void* p_result = NULL;
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if(index >= p_map->entries.capacity)
	{
		goto END;
	}
	const si_map_pair_t** const pp_pair = si_array_at(&p_map->entries, index);
	if(NULL == pp_pair)
	{
		goto END;
	}
	if(NULL == *pp_pair)
	{
		goto END;
	}
	p_result = (*pp_pair)->p_value;
END:
	return p_result;
}

bool si_map_has(si_map_t* const p_map, const void* const p_key)
{
	bool result = false;
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	result = (si_map_index_of(p_map, p_key) < p_map->entries.capacity);
END:
	return result;
}

bool si_map_remove_at(si_map_t* const p_map, const size_t index)
{
	bool result = false;
	if(NULL == p_map)
	{
		goto END;
	}
	if(index >= p_map->entries.capacity)
	{
		goto END;
	}
	si_map_pair_t** pp_pair = si_array_at(&p_map->entries, index);
	if(NULL == pp_pair)
	{
		goto END;
	}
	if(NULL != *pp_pair)
	{
		if((NULL != p_map->p_free_key_f) &&
			(NULL != (*pp_pair)->p_key))
		{
			p_map->p_free_key_f((*pp_pair)->p_key);
		}
		(*pp_pair)->p_key = NULL;
		if((NULL != p_map->p_free_value_f) &&
			(NULL != (*pp_pair)->p_value))
		{
			p_map->p_free_value_f((*pp_pair)->p_value);
		}
		(*pp_pair)->p_value = NULL;
		free(*pp_pair);
		*pp_pair = NULL;
		result = true;
	}
END:
	return result;
}

bool si_map_remove(si_map_t* const p_map, const void* const p_key)
{
	bool result = false;
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if(index >= p_map->entries.capacity)
	{
		goto END;
	}
	result = si_map_remove_at(p_map, index);
END:
	return result;
}

bool si_map_insert_pair(si_map_t* const p_map,
	const si_map_pair_t* const p_pair)
{
	bool result = false;
	if((NULL == p_map) || (NULL == p_pair))
	{
		goto END;
	}
	if(NULL == p_map->entries.p_data)
	{
		goto END;
	}
	// Ensure key is unique
	if(SIZE_MAX != si_map_index_of(p_map, p_pair->p_key))
	{
		goto END;
	}
	// Insert pair at first open slot
	for(size_t i = 0u; i < p_map->entries.capacity; i++)
	{
		si_map_pair_t** pp_next = si_array_at(&p_map->entries, i);
		if(NULL == pp_next)
		{
			continue;
		}
		if(NULL == *pp_next)
		{
			// Assign
			si_array_set(&p_map->entries, i, &p_pair);
			result = true;
			goto END;
		}
	}
	// Didn't have an index to assign a new pair in map.
	if(si_realloc_settings_grow(p_map->p_settings, &p_map->entries))
	{
		result = si_map_insert_pair(p_map, p_pair);
	}
END:
	return result;
}

bool si_map_insert(si_map_t* const p_map, const void* const p_key, const void* const p_value)
{
	bool result = false;
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	si_map_pair_t* p_pair = si_map_pair_new(p_key, p_value);
	if(NULL == p_pair)
	{
		goto END;
	}
	result = si_map_insert_pair(p_map, p_pair);
	if(true != result)
	{
		free(p_pair);
		p_pair = NULL;
	}
END:
	return result;
}

bool si_map_assign(si_map_t* const p_map, const void* const p_key,
	const void* const p_value)
{
	bool result = false;
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if(SIZE_MAX <= index)
	{
		goto END;
	}
	si_map_pair_t** pp_pair = si_array_at(&p_map->entries, index);
	if(NULL == pp_pair)
	{
		goto END;
	}
	if(NULL == *pp_pair)
	{
		goto END;
	}
	(*pp_pair)->p_value = (void*)p_value;
	result = true;
END:
	return result;
}

void si_map_free(si_map_t* const p_map)
{
	if(NULL == p_map)
	{
		goto END;
	}
	p_map->p_cmp_key_f = NULL;
	p_map->p_cmp_value_f = NULL;

	for(size_t i = 0u; i < p_map->entries.capacity; i++)
	{
		si_map_remove_at(p_map, i);
	}
	p_map->p_free_key_f = NULL;
	p_map->p_free_value_f = NULL;
	si_array_free(&p_map->entries);
END:
	return;
}

void si_map_free_at(si_map_t** const pp_map)
{
	if(NULL == pp_map)
	{
		goto END;
	}
	if(NULL == *pp_map)
	{
		// Already Freed
		goto END;
	}
	si_map_free(*pp_map);
	free(*pp_map);
	*pp_map = NULL;
END:
	return;
}
