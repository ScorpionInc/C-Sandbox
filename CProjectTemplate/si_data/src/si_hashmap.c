//si_hashmap.c

#include "si_hashmap.h"

static size_t si_hashmap_default_hash(const void* const p_key,
	const size_t key_size)
{
	size_t result = 0u;
	if (NULL == p_key)
	{
		goto END;
	}
	si_adler_t hash = (si_adler_t){0};
	si_adler_new(&hash, sizeof(size_t));
	si_adler_update(&hash, p_key, key_size);
	result = *((size_t*)hash.p_hash);
	si_adler_free(&hash);
END:
	return result;
}

static int si_hashmap_compare_hash(const void* const p_left,
	const void* const p_right)
{
	int result = 0;
	if (p_left == p_right)
	{
		goto END;
	}
	const size_t* const p_lhash = (const size_t*)p_left;
	const size_t* const p_rhash = (const size_t*)p_right;
	if (*p_lhash == *p_rhash)
	{
		goto END;
	}
	result = -1;
	if (*p_lhash < *p_rhash)
	{
		goto END;
	}
	result = 1;
END:
	return result;
}

void si_hashmap_init(si_hashmap_t* const p_hashmap, const size_t capacity)
{
	if (NULL == p_hashmap)
	{
		goto END;
	}
	if (0u >= capacity)
	{
		goto END;
	}
	si_array_init_3(&(p_hashmap->maps), sizeof(void*), capacity);
	if (NULL == p_hashmap->p_hash_f)
	{
		p_hashmap->p_hash_f = si_hashmap_default_hash;
	}
END:
	return;
}

si_hashmap_t* si_hashmap_new(const size_t capacity)
{
	si_hashmap_t* p_new = NULL;
	if (0u >= capacity)
	{
		goto END;
	}
	p_new = calloc(1u, sizeof(si_hashmap_t));
	if (NULL != p_new)
	{
		si_hashmap_init(p_new, capacity);
	}
END:
	return p_new;
}

void si_hashmap_update_settings(si_hashmap_t* const p_hashmap)
{
	if (NULL == p_hashmap)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_hashmap->maps.capacity; iii++)
	{
		si_map_t** const pp_map = si_array_at(&(p_hashmap->maps), iii);
		if (NULL == pp_map)
		{
			break;
		}
		if (NULL == *pp_map)
		{
			continue;
		}
		(*pp_map)->p_settings = p_hashmap->p_settings;
	}
END:
	return;
}

size_t si_hashmap_count(const si_hashmap_t* const p_hashmap)
{
	size_t result = SIZE_MAX;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	if (NULL == p_hashmap->maps.p_data)
	{
		goto END;
	}
	result++;
	for (size_t iii = 0u; iii < p_hashmap->maps.capacity; iii++)
	{
		si_map_t** pp_map = si_array_at(&(p_hashmap->maps), iii);
		if (NULL == pp_map)
		{
			continue;
		}
		if (NULL == *pp_map)
		{
			continue;
		}
		result += si_map_count(*pp_map);
	}
END:
	return result;
}

bool si_hashmap_is_empty(const si_hashmap_t* const p_hashmap)
{
	bool result = true;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	result = (0u == si_hashmap_count(p_hashmap));
END:
	return result;
}

size_t si_hashmap_hash(const si_hashmap_t* const p_hashmap,
	const void* const p_key, const size_t key_size)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_hashmap) || (NULL == p_key))
	{
		goto END;
	}
	if (NULL == p_hashmap->p_hash_f)
	{
		// The dutchman must have a captain.
		result = si_hashmap_default_hash(p_key, key_size);
	}
	else
	{
		result = p_hashmap->p_hash_f(p_key, key_size);
	}
END:
	return result;
}

void* si_hashmap_at(const si_hashmap_t* p_hashmap, const void* const p_key,
	const size_t key_size)
{
	void* p_result = NULL;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	const size_t hash = si_hashmap_hash(p_hashmap, p_key, key_size);
	p_result = si_hashmap_at_hash(p_hashmap, hash);
END:
	return p_result;
}

void* si_hashmap_at_hash(const si_hashmap_t* p_hashmap, const size_t hash)
{
	void* p_result = NULL;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	const size_t index = (hash % p_hashmap->maps.capacity);
	si_map_t** pp_map = si_array_at(&(p_hashmap->maps), index);
	if (NULL == pp_map)
	{
		goto END;
	}
	if (NULL == *pp_map)
	{
		goto END;
	}
	p_result = si_map_at(*pp_map, &hash);
END:
	return p_result;
}

bool si_hashmap_has(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size)
{
	bool result = false;
	if ((NULL == p_hashmap) || (NULL == p_key))
	{
		goto END;
	}
	result = (NULL != si_hashmap_at(p_hashmap, p_key, key_size));
END:
	return result;
}

bool si_hashmap_has_hash(si_hashmap_t* const p_hashmap, const size_t hash)
{
	bool result = false;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	result = (NULL != si_hashmap_at_hash(p_hashmap, hash));
END:
	return result;
}

bool si_hashmap_find(si_hashmap_t* const p_hashmap, const void* const p_value,
	const size_t** pp_hash)
{
	bool result = false;
	if ((NULL == p_hashmap) || (NULL == p_value))
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_hashmap->maps.capacity; iii++)
	{
		const si_map_t** pp_next = si_array_at(&(p_hashmap->maps), iii);
		if (NULL == pp_next)
		{
			continue;
		}
		if (NULL == *pp_next)
		{
			continue;
		}
		const size_t index = si_map_find(*pp_next, p_value);
		if (SIZE_MAX != index)
		{
			si_map_pair_t** pp_pair = si_array_at(
				&((*pp_next)->entries), index
			);
			if (NULL == pp_pair)
			{
				continue;
			}
			if (NULL == *pp_pair)
			{
				continue;
			}
			*pp_hash = (*pp_pair)->p_key;
			result = true;
			break;
		}
	}
END:
	return result;
}

bool si_hashmap_insert(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value)
{
	bool result = false;
	if ((NULL == p_hashmap) || (NULL == p_key))
	{
		goto END;
	}
	size_t hash = si_hashmap_hash(p_hashmap, p_key, key_size);
	result = si_hashmap_insert_hash(p_hashmap, hash, p_value);
END:
	return result;
}

bool si_hashmap_insert_hash(si_hashmap_t* const p_hashmap, const size_t hash,
	const void* const p_value)
{
	bool result = false;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	const size_t index = hash % p_hashmap->maps.capacity;
	si_map_t** pp_map = si_array_at(&(p_hashmap->maps), index);
	if (NULL == pp_map)
	{
		goto END;
	}
	if (NULL == *pp_map)
	{
		// Initialize map at index
		*pp_map = si_map_new();
		if (NULL == *pp_map)
		{
			// Failed to initialize new map.
			goto END;
		}
		(*pp_map)->p_settings = p_hashmap->p_settings;
		(*pp_map)->p_cmp_key_f = si_hashmap_compare_hash;
		(*pp_map)->p_free_key_f = free;
	}
	size_t* p_heap_key = calloc(1u, sizeof(size_t));
	if (NULL == p_heap_key)
	{
		goto END;
	}
	*p_heap_key = hash;
	result = si_map_insert(*pp_map, p_heap_key, p_value);
END:
	return result;
}

bool si_hashmap_assign(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value)
{
	bool result = false;
	if ((NULL == p_hashmap) || (NULL == p_key))
	{
		goto END;
	}
	const size_t hash = si_hashmap_hash(p_hashmap, p_key, key_size);
	result = si_hashmap_assign_hash(p_hashmap, hash, p_value);
END:
	return result;
}

bool si_hashmap_assign_hash(si_hashmap_t* const p_hashmap,
	const size_t hash, const void* const p_value)
{
	bool result = false;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	const size_t index = (hash % p_hashmap->maps.capacity);
	si_map_t** pp_map = si_array_at(&(p_hashmap->maps), index);
	if (NULL == pp_map)
	{
		goto END;
	}
	if (NULL == *pp_map)
	{
		goto END;
	}
	result = si_map_assign(*pp_map, &hash, p_value);
END:
	return result;
}

bool si_hashmap_remove(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size)
{
	bool result = false;
	if ((NULL == p_hashmap) || (NULL == p_key))
	{
		goto END;
	}
	const size_t hash = si_hashmap_hash(p_hashmap, p_key, key_size);
	result = si_hashmap_remove_hash(p_hashmap, hash);
END:
	return result;
}

bool si_hashmap_remove_hash(si_hashmap_t* const p_hashmap, const size_t hash)
{
	bool result = false;
	if (NULL == p_hashmap)
	{
		goto END;
	}
	const size_t index = (hash % p_hashmap->maps.capacity);
	si_map_t** pp_map = si_array_at(&(p_hashmap->maps), index);
	if (NULL == pp_map)
	{
		goto END;
	}
	if (NULL == *pp_map)
	{
		goto END;
	}
	result = si_map_remove(*pp_map, &hash);
END:
	return result;
}

void si_hashmap_free(si_hashmap_t* p_hashmap)
{
	if (NULL == p_hashmap)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_hashmap->maps.capacity; iii++)
	{
		si_map_t** pp_map = si_array_at(&(p_hashmap->maps), iii);
		if (NULL == pp_map)
		{
			break;
		}
		if (NULL == *pp_map)
		{
			continue;
		}
		si_map_free_at(pp_map);
	}
	si_array_free(&(p_hashmap->maps));
	p_hashmap->p_hash_f = NULL;
END:
	return;
}

void si_hashmap_free_at(si_hashmap_t** pp_hashmap)
{
	if (NULL == pp_hashmap)
	{
		goto END;
	}
	if (NULL == *pp_hashmap)
	{
		// Already freed
		goto END;
	}
	si_hashmap_free(*pp_hashmap);
	free(*pp_hashmap);
	*pp_hashmap = NULL;
END:
	return;
}
