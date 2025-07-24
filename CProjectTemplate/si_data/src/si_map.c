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


void si_map_init(si_map_t* const p_map)
{
	//!TODO
	if(NULL == p_map)
	{
		goto END;
	}
	p_map->p_cmp_key_f = NULL;
	p_map->p_cmp_value_f = NULL;
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
		if(NULL != si_dynamic_at(&p_map->entries, i))
		{
			result++;
		}
	}
	// End
END:
	return result;
}

size_t si_map_index_of_raw(const si_map_t* const p_map, const void* const p_key)
{
	//!TODO
	size_t result = SIZE_MAX;
	// Validation
	if((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	// Begin
	const size_t count = si_map_count(p_map);
	for(size_t i = 0u; i < count; i++)
	{
		if(0)//TODO
		{
		}
	}
	// End
END:
	return result;
}

size_t si_map_index_of(const si_map_t* const p_map, const si_dynamic_t* const p_key)
{
	//!TODO
}

si_dynamic_t* si_map_at(si_map_t* const p_map, const void* const p_key)
{
	//!TODO
}

bool si_map_has(si_map_t* const p_map, const void* const p_key)
{
	//!TODO
}

bool si_map_remove(si_map_t* const p_map, const void* const p_key)
{
	//!TODO
}

bool si_map_insert_raw(si_map_t* const p_map, const void* const p_key, const void* const p_value)
{
	//!TODO
}

void si_map_free(si_map_t* const p_map)
{
	if(NULL == p_map)
	{
		goto END;
	}
	p_map->p_cmp_key_f = NULL;
	p_map->p_cmp_value_f = NULL;
	si_realloc_settings_free(&p_map->settings);

	if(NULL != p_map->p_free_key_f)
	{
		for(size_t i = 0u; i < p_map->keys.capacity; i++)
		{
			p_map->p_free_key_f(si_dynamic_at(p_map->keys, i));
		}
	}
	p_map->p_free_key_f = NULL;
	si_dynamic_free(&p_map->keys);

	if(NULL != p_map->p_free_value_f)
	{
		for(size_t i = 0u; i < p_map->values.capacity; i++)
		{
			p_map->p_free_value_f(si_dynamic_at(p_map->values, i));
		}
	}
	p_map->p_free_value_f = NULL;
	si_dynamic_free(&p_map->values);
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
	}
	si_map_free(*pp_map);
	*pp_map = NULL;
END:
	return;
}
