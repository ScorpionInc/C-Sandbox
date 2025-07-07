//si_map.c

#include "si_map.h"

void si_map_pair_new(si_map_pair* const p_pair, const void* const p_key,
	const size_t key_size, const void* const p_value, const size_t value_size)
{
	// Validation
	if((NULL == p_pair) || (NULL == p_value) || (NULL == p_value))
	{
		goto END;
	}
	// Begin
	si_dynamic_new_3(&p_pair->key, key_size, 1u);
	si_dynamic_new_3(&p_pair->value, value_size, 1u);
	si_dynamic_set(&p_pair->key, 0u, p_key);
	si_dynamic_set(&p_pair->value, 0u, p_value);
	// End
END:
	return;
}

void si_map_new(si_map* const p_map)
{
	//!TODO
}

size_t si_map_count(const si_map* const p_map)
{
	size_t result = SIZE_MAX;
	// Validation
	if(NULL == p_map)
	{
		goto END;
	}
	// Begin
	result = si_linked_list_count(&p_map->entries);
	// End
END:
	return result;
}

size_t si_map_index_of_raw(const si_map* const p_map, const void* const p_key,
	const size_t key_size)
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

size_t si_map_index_of(const si_map* const p_map, const si_dynamic* const p_key)
{
	//!TODO
}

si_dynamic* si_map_at_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size)
{
	//!TODO
}

si_dynamic* si_map_at(si_map* const p_map, const si_dynamic* const p_key)
{
	//!TODO
}

bool si_map_has_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size)
{
	//!TODO
}

bool si_map_has(si_map* const p_map, const si_dynamic* const p_key)
{
	//!TODO
}

bool si_map_remove_raw(si_map* const p_map, const void* const p_key, const size_t key_size)
{
	//!TODO
}

bool si_map_remove(si_map* const p_map, const si_dynamic* const p_key)
{
	//!TODO
}

bool si_map_insert_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size, const void* p_value, const size_t value_size)
{
	//!TODO
}

bool si_map_insert(si_map* const p_map, const si_map_pair* const p_pair)
{
	//!TODO
}

void si_map_free(si_map* const p_map)
{
	//!TODO
}
