//si_parray.c
#include "si_parray.h"

void si_parray_init_2(si_parray_t* const p_array,
	const size_t initial_capacity)
{
	if(NULL == p_array)
	{
		goto END;
	}
	p_array->p_free_value = free;
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
	if(NULL == p_array->array.p_data)
	{
		goto END;
	}
	result++;
	for(size_t iii = 0u; iii < p_array->array.capacity; iii++)
	{
		const bool is_set = si_parray_has_set(p_array, iii);
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

void* si_parray_at(const si_parray_t* const p_array,
	const size_t index)
{
	void* p_result = NULL;
	if(NULL == p_array)
	{
		goto END;
	}
	void** pp_value = si_array_at(&(p_array->array), index);
	if(NULL == pp_value)
	{
		goto END;
	}
	p_result = (void*)(*pp_value);
END:
	return p_result;
}

bool si_parray_has_set(const si_parray_t* const p_array, const size_t index)
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

bool si_parray_remove_at(si_parray_t* const p_array, const size_t index)
{
	// TODO
	bool result = false;
	if(NULL == p_array)
	{
		goto END;
	}
	size_t counter = index;
	void** pp_target = si_array_at(&(p_array->array), index);
	void** pp_next = NULL;
	if(NULL == pp_target)
	{
		goto END;
	}
	void** const pp_last = si_parray_last(p_array);
	do
	{
		// Sanity check
		if(counter >= p_array->array.capacity)
		{
			goto END;
		}
		if(NULL == *pp_target)
		{
			goto END;
		}
		// Remove and shift(keep contigious)
		if((counter == index) && (NULL != p_array->p_free_value))
		{
			p_array->p_free_value(*pp_target);
		}
		*pp_target = NULL;
		pp_next = &(pp_target[1]);
		if(pp_next != pp_last)
		{
			*pp_target = *pp_next;
		}
		pp_target = pp_next;
		counter++;
	} while(pp_target != pp_last);
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
	const bool is_cleared = si_parray_remove_at(p_array, index);
	if(!is_cleared)
	{
		goto END;
	}
	si_array_set(&(p_array->array), index, p_value);
END:
	return;
}

void si_parray_set_from(si_parray_t* const p_array, const size_t index,
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
			did_grow = si_realloc_settings_grow(p_array->p_settings, &(p_array->array));
		}
		if(!did_grow)
		{
			goto END;
		}
	}
	si_parray_set(p_array, count, p_value);
END:
	return result;
}

size_t si_parray_append_from(si_parray_t* const p_array,
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
	if(NULL != p_array->p_free_value)
	{
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
			p_array->p_free_value(*pp_value);
			*pp_value = NULL;
		}
	}
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