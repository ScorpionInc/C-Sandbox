//si_hashmap.c

#include "si_hashmap.h"

static size_t si_hashmap_default_hash(const void* const p_key, const size_t key_size)
{
	size_t result = 0u;
	if(NULL == p_key)
	{
		goto END;
	}
	si_adler_t hash = (si_adler_t){};
	si_adler_new(&hash, sizeof(size_t));
	si_adler_update(&hash, p_key, key_size);
	result = *((size_t*)hash.hash);
	si_adler_free(&hash);
END:
	return result;
}

void si_hashmap_init(si_hashmap_t* const p_hashmap)
{
	if(NULL == p_hashmap)
	{
		goto END;
	}
	si_array_new_3(&(p_hashmap->maps), sizeof(void*), 0u);
	if(NULL == p_hashmap->p_hash_f)
	{
		p_hashmap->p_hash_f = si_hashmap_default_hash;
	}
END:
	return;
}

si_hashmap_t* si_hashmap_new()
{
	si_hashmap_t* p_new = calloc(1u, sizeof(si_hashmap_t));
	if(NULL != p_new)
	{
		si_hashmap_init(p_new);
	}
	return p_new;
}

size_t si_hashmap_count(si_hashmap_t* const p_hashmap)
{
	size_t result = 0u;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_hashmap->map.entries.p_data)
	{
		goto END;
	}
	for(size_t i = 0u; i < p_hashmap->maps.capacity; i++)
	{
		si_map_t** pp_map = si_array_at(&(p_hashmap->maps), i);
		if(NULL == pp_map)
		{
			continue;
		}
		if(NULL == *pp_map)
		{
			continue;
		}
		result += si_map_count(*pp_map);
	}
END:
	return result;
}

bool si_hashmap_is_empty(si_hashmap_t* const p_hashmap)
{
	bool result = true;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	result = (0u == si_hashmap_count(p_hashmap));
END:
	return result;
}

bool si_hashmap_has(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size)
{
	//! TODO
	bool result = false;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_key)
	{
		goto END;
	}
END:
	return result;
}

size_t si_hashmap_find(si_hashmap_t* const p_hashmap, const void* const p_value,
	void* const p_key, const size_t key_size)
{
	//! TODO
	size_t result = 0u;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_value)
	{
		goto END;
	}
	if(NULL == p_key)
	{
		goto END;
	}
END:
	return result;
}

bool si_hashmap_insert(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value)
{
	//! TODO
	bool result = false;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_key)
	{
		goto END;
	}
END:
	return result;
}

bool si_hashmap_remove(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size)
{
	//! TODO
	bool result = false;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_key)
	{
		goto END;
	}
END:
	return result;
}

bool si_hashmap_assign(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value)
{
	//! TODO
	bool result = false;
	if(NULL == p_hashmap)
	{
		goto END;
	}
	if(NULL == p_key)
	{
		goto END;
	}
END:
	return result;
}

void si_hashmap_free(si_hashmap_t* p_hashmap)
{
	if(NULL == p_hashmap)
	{
		goto END;
	}
	si_array_free(&(p_hashmap->maps));
	p_hashmap->p_hash_f = NULL;
END:
	return;
}

void si_hashmap_free_at(si_hashmap_t** pp_hashmap)
{
	if(NULL == pp_hashmap)
	{
		goto END;
	}
	if(NULL == *pp_hashmap)
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
