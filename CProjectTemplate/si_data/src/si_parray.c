//si_parray.c
#include "si_parray.h"

void si_parray_init_2(si_parray_t* const p_array,
	const size_t initial_capacity)
{
	if(NULL == p_array)
	{
		goto END;
	}
	p_array->p_free_value = NULL;
	p_array->p_settings = NULL;
	si_array_init_3(&(p_array->array), sizeof(void*), initial_capacity);
END:
	return;
}
inline void si_parray_init(si_parray_t* const p_array)
{
	// Default value of initial_capacity is 0
	si_parray_init_2(p_array, 0u);
}

si_parray_t* si_parray_new_1(const size_t initial_capacity)
{
	si_parray_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_parray_t));
	if(NULL == p_result)
	{
		goto END;
	}
	si_parray_init_2(p_result, initial_capacity);
END:
	return p_result;
}
inline si_parray_t* si_parray_new()
{
	// Default value of initial capacity is 0
	return si_parray_new_1(0u);
}

size_t si_parray_size(const si_parray_t* const p_array)
{
	size_t result = SIZE_MAX;
	if(NULL == p_array)
	{
		goto END;
	}
	result = si_array_size(&(p_array->array));
END:
	return result;
}

size_t si_parray_count(const si_parray_t* const p_array)
{
	size_t result = SIZE_MAX;
	if(NULL == p_array)
	{
		goto END;
	}
	result++;
	for(size_t iii = 0u; iii < p_array->array.capacity; iii++)
	{
		const bool is_set = si_parray_is_set(p_array, iii);
		if(!is_set)
		{
			// Values are contigious
			break;
		}
		result++;
	}
END:
	return result;
}

bool si_parray_fit(si_parray_t* const p_array)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	const size_t count = si_parray_count(p_array);
	result = si_array_resize(&(p_array->array), count);
END:
	return result;
}

bool si_parray_is_pointer_within(si_parray_t* const p_array,
	const void* const p_pointer)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	result = si_array_is_pointer_within(&(p_array->array), p_pointer);
END:
	return result;
}

bool si_parray_is_pointer_valid(si_parray_t* const p_array,
	const void** const pp_pointer)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	result = si_array_is_pointer_element(&(p_array->array), pp_pointer);
END:
	return result;
}

size_t si_parray_pointer_to_index(si_parray_t* const p_array,
	const void** const pp_pointer)
{
	size_t result = SIZE_MAX;
	if(NULL == p_array)
	{
		goto END;
	}
	result = si_array_find_pointer_index(&(p_array->array), pp_pointer);
END:
	return result;
}

size_t si_parray_find(const si_parray_t* const p_array,
	const void* const p_value, int (*p_cmp_f)(const void*, const void*))
{
	size_t index = SIZE_MAX;
	if(NULL == p_array)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < p_array->array.capacity; iii++)
	{
		const void** const pp_next = si_array_at(&(p_array->array), iii);
		if(NULL == pp_next)
		{
			goto END;
		}
		if(p_value == *pp_next)
		{
			index = iii;
			goto END;
		}
		if(NULL != p_cmp_f)
		{
			const int cmp_result = p_cmp_f(p_value, *pp_next);
			if(0 == cmp_result)
			{
				index = iii;
				goto END;
			}
		}
	}
END:
	return index;
}

bool si_parray_contains(const si_parray_t* const p_array,
	const void* const p_value, int (*p_cmp_f)(const void*, const void*))
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	const size_t index = si_parray_find(p_array, p_value, p_cmp_f);
	result = (index != SIZE_MAX);
END:
	return result;
}

void* si_parray_at(const si_parray_t* const p_array,
	const size_t index)
{
	void** pp_result = NULL;
	if(NULL == p_array)
	{
		goto END;
	}
	pp_result = si_array_at(&(p_array->array), index);
	if(NULL == pp_result)
	{
		goto END;
	}
	pp_result = *pp_result;
END:
	return (void*)pp_result;
}

bool si_parray_is_set(const si_parray_t* const p_array, const size_t index)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	void** pp_value = si_array_at(&(p_array->array), index);
	if(NULL == pp_value)
	{
		goto END;
	}
	result = (NULL != *pp_value);
END:
	return result;
}

void** si_parray_first(const si_parray_t* const p_array)
{
	void** pp_result = NULL;
	if(NULL == p_array)
	{
		goto END;
	}
	pp_result = si_array_at(&(p_array->array), 0u);
END:
	return pp_result;
}

void** si_parray_last(const si_parray_t* const p_array)
{
	void** pp_result = NULL;
	if(NULL == p_array)
	{
		goto END;
	}
	if(0u >= p_array->array.capacity)
	{
		goto END;
	}
	pp_result = si_array_at(&(p_array->array), p_array->array.capacity - 1u);
END:
	return pp_result;
}

void** si_parray_tail(const si_parray_t* const p_array)
{
	void** pp_result = NULL;
	if(NULL == p_array)
	{
		goto END;
	}
	const size_t count = si_parray_count(p_array);
	if(0u >= count)
	{
		goto END;
	}
	pp_result = si_array_at(&(p_array->array), count - 1u);
END:
	return pp_result;
}

/** Doxygen
 * @brief Apply realloc settings to shrink if needed & defined or fits() array.
 * 
 * @param p_array Pointer to the pointer array to handle the shrinking there of
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool handle_shrink(si_parray_t* const p_array)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	if(NULL == p_array->p_settings)
	{
		// Not specified how to handle shrink.
		// We will fit the array capacity to its count.
		result = si_parray_fit(p_array);
	}
	else
	{
		const size_t count = si_parray_count(p_array);
		const size_t next_capacity = si_realloc_settings_next_shrink_capacity(
			p_array->p_settings,
			p_array->array.capacity
		);
		if(next_capacity < count)
		{
			goto END;
		}
		result = si_realloc_settings_shrink(
			p_array->p_settings,
			&(p_array->array)
		);
	}
END:
	return result;
}

bool si_parray_remove_at(si_parray_t* const p_array, const size_t index)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	size_t counter = index;
	void** pp_target = si_array_at(&(p_array->array), index);
	if(NULL == pp_target)
	{
		// index or array is invalid
		goto END;
	}
	if(NULL != p_array->p_free_value)
	{
		p_array->p_free_value(*pp_target);
		*pp_target = NULL;
	}
	void** const pp_tail = si_parray_tail(p_array);
	if(NULL == pp_tail)
	{
		// Array is empty
		goto END;
	}
	void** pp_next = NULL;
	while(pp_target != pp_tail)
	{
		// Sanity check
		if(counter >= p_array->array.capacity)
		{
			goto END;
		}
		// Shift left
		pp_next = &(pp_target[1]);
		*pp_target = *pp_next;
		pp_target = pp_next;
		if(NULL == *pp_target)
		{
			goto END;
		}
		counter++;
	}
	// Remove tail
	if(NULL != p_array->p_free_value)
	{
		p_array->p_free_value(*pp_tail);
	}
	*pp_tail = NULL;
	handle_shrink(p_array);
	result = true;
END:
	return result;
}

void si_parray_clear(si_parray_t* const p_array)
{
	if(NULL == p_array)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < p_array->array.capacity; iii++)
	{
		void** pp_value = si_array_at(&(p_array->array), iii);
		if(NULL == pp_value)
		{
			break;
		}
		if(NULL == *pp_value)
		{
			continue;
		}
		if(NULL != p_array->p_free_value)
		{
			p_array->p_free_value(*pp_value);
		}
		*pp_value = NULL;
	}
END:
	return;
}

/** Doxygen
 * @brief Called by set_from() / append_from() funcs to take on pointer freeing
 *
 * @param p_array Pointer to the parray_t who's to handle freeing their values.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 * @details Defines the p_free_value function, inside the si_parray_t struct,
 *        to free() if it hasn't already been defined. This is in order to
 *        release the self-allocated pointer values allocated on clone when
 *        parray is freed. Also converts any already existing direct pointer
 *        values into indirect heap pointer values so free() can safely be
 *        called on them.
 */
static bool claim_value_ownership(si_parray_t* const p_array)
{
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	if(NULL != p_array->p_free_value)
	{
		// Pointer array already own's it's value freeing responsibility
		goto END;
	}
	// Convert direct pointers to indirect heap pointers
	for(size_t iii = 0u; iii < p_array->array.capacity; iii++)
	{
		const bool is_set = si_parray_is_set(p_array, iii);
		if(!is_set)
		{
			break;
		}
		void** pp_direct = si_array_at(&(p_array->array), iii);
		void** pp_indirect = calloc(1u, sizeof(void*));
		if(NULL == pp_indirect)
		{
			// Failed to convert to indirect!
			// Undo changes and return false
			for(size_t jjj = 0u; (jjj < (iii - 1u)) && (0u < iii); jjj++)
			{
				pp_indirect = si_array_at(&(p_array->array), jjj);
				pp_direct = *pp_indirect;
				free(*pp_indirect);
				*pp_indirect = NULL;
				si_parray_set(p_array, jjj, *pp_direct);
			}
			goto END;
		}
		*pp_indirect = *pp_direct;
		si_parray_set(p_array, iii, pp_indirect);
	}
	p_array->p_free_value = free;
	result = true;
END:
	return result;
}

void si_parray_set(si_parray_t* const p_array, const size_t index,
	const void* p_value)
{
	if((NULL == p_array) || (NULL == p_value))
	{
		goto END;
	}
	if(index >= p_array->array.capacity)
	{
		goto END;
	}
	void* p_old_value = si_parray_at(p_array, index);
	if(NULL != p_old_value)
	{
		if(NULL != p_array->p_free_value)
		{
			p_array->p_free_value(p_old_value);
		}
		p_old_value = NULL;
	}
	si_array_set(&(p_array->array), index, &p_value);
END:
	return;
}

void si_parray_set_clone(si_parray_t* const p_array, const size_t index,
	const void* p_source, const size_t source_size)
{
	if((NULL == p_array) || (NULL == p_source))
	{
		goto END;
	}
	if((index >= p_array->array.capacity) || (0u >= source_size))
	{
		goto END;
	}
	void* p_value = calloc(1u, source_size);
	if(NULL == p_value)
	{
		goto END;
	}
	if(NULL == p_array->p_free_value)
	{
		const bool is_claimed = claim_value_ownership(p_array);
		if(!is_claimed)
		{
			free(p_value);
			p_value = NULL;
			goto END;
		}
	}
	memcpy(p_value, p_source, source_size);
	si_parray_set(p_array, index, p_value);
END:
	return;
}

size_t si_parray_append(si_parray_t* const p_array, const void* const p_value)
{
	size_t result = SIZE_MAX;
	if((NULL == p_array) || (NULL == p_value))
	{
		goto END;
	}
	const size_t count = si_parray_count(p_array);
	if(count >= p_array->array.capacity)
	{
		bool did_grow = false;
		if(NULL == p_array->p_settings)
		{
			did_grow = si_array_resize(&(p_array->array), count + 1u);
		}
		else
		{
			did_grow = si_realloc_settings_grow(
				p_array->p_settings,
				&(p_array->array)
			);
		}
		if(!did_grow)
		{
			goto END;
		}
	}
	si_parray_set(p_array, count, p_value);
	result = count;
END:
	return result;
}

size_t si_parray_append_clone(si_parray_t* const p_array,
	const void* const p_source, const size_t source_size)
{
	size_t result = SIZE_MAX;
	if((NULL == p_array) || (NULL == p_source))
	{
		goto END;
	}
	if(0u >= source_size)
	{
		goto END;
	}
	void* p_value = calloc(1u, source_size);
	if(NULL == p_value)
	{
		goto END;
	}
	if(NULL == p_array->p_free_value)
	{
		const bool is_claimed = claim_value_ownership(p_array);
		if(!is_claimed)
		{
			free(p_value);
			p_value = NULL;
			goto END;
		}
	}
	memcpy(p_value, p_source, source_size);
	result = si_parray_append(p_array, p_value);
END:
	return result;
}

void si_parray_fprint(FILE* p_file, const si_parray_t* const p_array,
	void (*p_print_value)(FILE*, void*))
{
	if((NULL == p_file) || (NULL == p_array))
	{
		goto END;
	}
	const size_t count = si_parray_count(p_array);
	fprintf(p_file, "%s", "{");
	for(size_t iii = 0u; iii < count; iii++)
	{
		void* p_value = si_parray_at(p_array, iii);
		if(NULL == p_value)
		{
			break;
		}
		if(NULL == p_print_value)
		{
			fprintf(p_file, "%p", p_value);
		}
		else
		{
			p_print_value(p_file, p_value);
		}
		if(iii < (count - 1u))
		{
			fprintf(p_file, ", ");
		}
	}
	fprintf(p_file, "}[%lu]", count);
END:
	return;
}

void si_parray_free(si_parray_t* const p_array)
{
	if(NULL == p_array)
	{
		goto END;
	}
	// Free values
	si_parray_clear(p_array);
	si_array_free(&(p_array->array));
END:
	return;
}

void si_parray_destroy(si_parray_t** pp_array)
{
	if(NULL == pp_array)
	{
		goto END;
	}
	if(NULL == *pp_array)
	{
		// Already freed
		goto END;
	}
	si_parray_free(*pp_array);
	free(*pp_array);
	*pp_array = NULL;
END:
	return;
}