// si_map.c
#include "si_map.h"

si_map_pair_t* si_map_pair_new(const void* const p_key,
	const void* const p_value)
{
	si_map_pair_t* p_new = NULL;
	if (NULL == p_key)
	{
		goto END;
	}
	p_new = calloc(1u, sizeof(si_map_pair_t));
	if (NULL == p_new)
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
	if (NULL == p_map)
	{
		goto END;
	}
	si_parray_init_2(&(p_map->entries), 0u);
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
	if (NULL == p_new)
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
	if (NULL == p_map)
	{
		goto END;
	}
	result = si_parray_count(&(p_map->entries));
END:
	return result;
}

size_t si_map_index_of(const si_map_t* const p_map, const void* const p_key)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	if (NULL == p_map->p_cmp_key_f)
	{
		goto END;
	}

	const size_t count = si_map_count(p_map);
	if (SIZE_MAX <= count)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < count; iii++)
	{
		si_map_pair_t* p_pair = si_parray_at(&p_map->entries, iii);
		if (NULL == p_pair)
		{
			break;
		}
		const int cmp_result = p_map->p_cmp_key_f(p_pair->p_key, p_key);
		if (0 == cmp_result)
		{
			// Key Match Found!
			result = iii;
			break;
		}
	}
END:
	return result;
}

size_t si_map_find(const si_map_t* const p_map, const void* const p_value)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_map) || (NULL == p_value))
	{
		goto END;
	}
	if (NULL == p_map->p_cmp_value_f)
	{
		goto END;
	}

	const size_t count = si_map_count(p_map);
	if (SIZE_MAX <= count)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < count; iii++)
	{
		si_map_pair_t* p_pair = si_parray_at(&(p_map->entries), iii);
		if (NULL == p_pair)
		{
			break;
		}
		const int cmp_result = p_map->p_cmp_value_f(p_pair->p_value, p_value);
		if (0 == cmp_result)
		{
			// Value Match Found!
			result = iii;
			break;
		}
	}
END:
	return result;
}

void* si_map_at(si_map_t* const p_map, const void* const p_key)
{
	void* p_result = NULL;
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if (SIZE_MAX <= index)
	{
		goto END;
	}
	const si_map_pair_t* const p_pair = si_parray_at(&(p_map->entries), index);
	if (NULL == p_pair)
	{
		goto END;
	}
	p_result = p_pair->p_value;
END:
	return p_result;
}

bool si_map_has(si_map_t* const p_map, const void* const p_key)
{
	bool result = false;
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	result = (SIZE_MAX != si_map_index_of(p_map, p_key));
END:
	return result;
}

bool si_map_remove_at(si_map_t* const p_map, const size_t index)
{
	bool result = false;
	if (NULL == p_map)
	{
		goto END;
	}

	// NULL/Free pair key/value.
	si_map_pair_t* p_pair = si_parray_at(&(p_map->entries), index);
	if (NULL == p_pair)
	{
		goto END;
	}
	if ((NULL != p_map->p_free_key_f) &&
		(NULL != p_pair->p_key))
	{
		p_map->p_free_key_f(p_pair->p_key);
	}
	p_pair->p_key = NULL;
	if ((NULL != p_map->p_free_value_f) &&
		(NULL != p_pair->p_value))
	{
		p_map->p_free_value_f(p_pair->p_value);
	}
	p_pair->p_value = NULL;
	free(p_pair);
	p_pair = NULL;

	// Remove from parray
	result = si_parray_remove_at(&(p_map->entries), index);
END:
	return result;
}

bool si_map_remove(si_map_t* const p_map, const void* const p_key)
{
	bool result = false;
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if (SIZE_MAX <= index)
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
	if ((NULL == p_map) || (NULL == p_pair))
	{
		goto END;
	}

	// Ensure key is unique
	const size_t map_index = si_map_index_of(p_map, p_pair->p_key);
	if (SIZE_MAX != map_index)
	{
		fprintf(stderr, "si_map_insert_pair() Failed due to key collision.\n");
		goto END;
	}

	// Insert pair value
	const size_t append_index = si_parray_append(&(p_map->entries), p_pair);
	result = (SIZE_MAX != append_index);
END:
	return result;
}

bool si_map_insert(si_map_t* const p_map, const void* const p_key,
	const void* const p_value)
{
	bool result = false;
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	si_map_pair_t* p_pair = si_map_pair_new(p_key, p_value);
	if (NULL == p_pair)
	{
		goto END;
	}
	result = si_map_insert_pair(p_map, p_pair);
	if (true != result)
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
	if ((NULL == p_map) || (NULL == p_key))
	{
		goto END;
	}
	const size_t index = si_map_index_of(p_map, p_key);
	if (SIZE_MAX <= index)
	{
		goto END;
	}
	si_map_pair_t* p_pair = si_parray_at(&(p_map->entries), index);
	if (NULL == p_pair)
	{
		goto END;
	}
	p_pair->p_value = (void*)p_value;
	result = true;
END:
	return result;
}

void si_map_free(si_map_t* const p_map)
{
	if (NULL == p_map)
	{
		goto END;
	}
	p_map->p_cmp_key_f = NULL;
	p_map->p_cmp_value_f = NULL;

	const size_t count = si_map_count(&(p_map->entries));
	if (SIZE_MAX <= count)
	{
		goto CLEAN;
	}
	for (size_t iii = 0u; iii < count; iii++)
	{
		si_map_remove_at(p_map, iii);
	}
CLEAN:
	p_map->p_free_key_f = NULL;
	p_map->p_free_value_f = NULL;
	si_parray_free(&(p_map->entries));
END:
	return;
}

void si_map_destroy(si_map_t** const pp_map)
{
	if (NULL == pp_map)
	{
		goto END;
	}
	if (NULL == *pp_map)
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

void si_map_fprint(FILE* const p_file, const si_map_t* const p_map)
{
	if (NULL == p_file)
	{
		goto END;
	}
	if (NULL == p_map)
	{
		fprintf(p_file, "NULL");
		goto END;
	}
	const size_t count = si_map_count(p_map);
	fprintf(p_file, "{");
	fprintf(p_file, "key_comp_func: %p; ", (void*)p_map->p_cmp_key_f);
	fprintf(p_file, "value_comp_func: %p; ", (void*)p_map->p_cmp_value_f);
	fprintf(p_file, "key_free_func: %p; ", (void*)p_map->p_free_key_f);
	fprintf(p_file, "value_free_func: %p; ", (void*)p_map->p_free_value_f);
	fprintf(p_file, "}:%lu/%lu@%p",
		count, p_map->entries.array.capacity, (void*)p_map
	);
END:
	return;
}