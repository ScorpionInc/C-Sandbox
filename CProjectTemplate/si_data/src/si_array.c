//si_array.c

#include "si_array.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void si_array_new_3(si_array_t* p_dynamic, const size_t element_size,
	const size_t capacity)
{
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	p_dynamic->p_data = calloc(capacity, element_size);
	if (NULL == p_dynamic->p_data)
	{
		goto END;
	}
	p_dynamic->element_size = element_size;
	p_dynamic->capacity = capacity;
	// End
	END:
		return;
}
inline void si_array_new(si_array_t* p_dynamic, const size_t element_size)
{
	// Default value of capacity is 0u
	si_array_new_3(p_dynamic, element_size, 0u);
}

size_t si_array_size(const si_array_t* p_dynamic)
{
	size_t result = 0u;
	// Validate Parameters
	if (NULL == p_dynamic)
	{
		goto END;
	}
	if ((NULL == p_dynamic->p_data) || (0u == p_dynamic->element_size))
	{
		goto END;
	}
	// Begin
	// Prevent Overflows
	if ((p_dynamic->capacity * p_dynamic->element_size) /
		p_dynamic->element_size != p_dynamic->capacity)
	{
		// Overflow detected.
		goto END;
	}
	result = p_dynamic->element_size * p_dynamic->capacity;
	// End
END:
	return result;
}

bool si_array_resize(si_array_t* p_dynamic,
    const size_t new_capacity)
{
	bool result = false;
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	const size_t old_capacity = p_dynamic->capacity;
	const size_t old_size = (p_dynamic->element_size * old_capacity);
	const size_t new_size =	(p_dynamic->element_size * new_capacity);
	// Check for overflows
	if(0u < p_dynamic->element_size)
	{
		if((old_size / p_dynamic->element_size) != old_capacity)
		{
			goto END;
		}
		if((new_size / p_dynamic->element_size) != new_capacity)
		{
			goto END;
		}
	}
	void* tmp = realloc(p_dynamic->p_data, new_size);
	if(NULL == tmp)
	{
		goto END;
	}
	p_dynamic->p_data = tmp;
	p_dynamic->capacity = new_capacity;
	// If grown, initialize new memory to 0x00(NULL).
	if(new_size > old_size)
	{
		char* p_end = &((char*)p_dynamic->p_data)[old_size];
		memset(p_end, 0x00, new_size - old_size);
	}
	result = true;
	// End
END:
	return result;
}

bool si_array_is_pointer_within(const si_array_t* p_dynamic,
	const void* p_test)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_dynamic) || (NULL == p_test))
	{
		goto END;
	}
	if(NULL == p_dynamic->p_data)
	{
		goto END;
	}
	// Begin
	result = (p_test >= p_dynamic->p_data);
	if(true == result)
	{
		const size_t size = si_array_size(p_dynamic);
		result = (p_test < (p_dynamic->p_data + size));
	}
	// End
END:
	return result;
}

size_t si_array_find_pointer_offset(const si_array_t* p_dynamic,
	const void* p_test)
{
	size_t offset = SIZE_MAX;
	// Validate parameters
	if( ! si_array_is_pointer_within(p_dynamic, p_test) )
	{
		// is_pointer_within verifies values are not NULL.
		goto END;
	}
	// Begin
	const size_t size = si_array_size(p_dynamic);
	offset = (p_dynamic->p_data + size) - p_test;
	// End
END:
	return offset;
}

bool si_array_is_pointer_element(const si_array_t* p_dynamic,
	const void* p_test)
{
	bool result = false;
	// Validate parameters
	if( ! si_array_is_pointer_within(p_dynamic, p_test) )
	{
		// is_pointer_within verifies values are not NULL.
		goto END;
	}
	if(p_dynamic->element_size == 0u)
	{
		goto END;
	}
	// Begin
	const size_t offset = si_array_find_pointer_offset(p_dynamic, p_test);
	result = ((offset % p_dynamic->element_size) == 0u);
	// End
END:
	return result;
}

size_t si_array_find_pointer_index(const si_array_t* p_dynamic,
	const void* p_test)
{
	size_t index = SIZE_MAX;
	// Validate parameters
	// Begin
	if( ! si_array_is_pointer_element(p_dynamic, p_test))
	{
		// is_pointer_element validates values are not NULL.
		// is_pointer_element validates element_size > 0
		goto END;
	}
	const size_t offset = si_array_find_pointer_offset(p_dynamic, p_test);
	index = offset / p_dynamic->element_size;
	// End
END:
	return index;
}

void* si_array_at(const si_array_t* p_dynamic,
	const size_t index)
{
	void* p_item = NULL;
	// Validate Parameters
	if(NULL == p_dynamic)
	{
		goto END;
	}
	if(index >= p_dynamic->capacity)
	{
		goto END;
	}
	// Begin
	p_item = (p_dynamic->p_data + (p_dynamic->element_size * index));
	// End
END:
	return p_item;
}
inline void* si_array_first(const si_array_t* p_dynamic)
{
	return si_array_at(p_dynamic, 0u);
}
inline void* si_array_last(const si_array_t* p_dynamic)
{
	return si_array_at(p_dynamic, p_dynamic->capacity - 1u);
}

void si_array_set(si_array_t* p_dynamic,
	const size_t index, const void* p_item)
{
	// Validate Parameters
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	const size_t offset = (p_dynamic->element_size * index);
	const size_t allocated_size = si_array_size(p_dynamic);
	// Bounds Check
	if(offset >= allocated_size)
	{
		goto END;
	}
	// Begin
	memcpy(p_dynamic->p_data + offset, p_item,
		p_dynamic->element_size);
	// End
END:
}

void si_array_get(const si_array_t* p_dynamic,
	const size_t index, void* p_item)
{
	// Validate parameters
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	const size_t offset = (p_dynamic->element_size * index);
	const size_t allocated_size = si_array_size(p_dynamic);
	// Bounds Check
	if(offset >= allocated_size)
	{
		goto END;
	}
	// Begin
	memcpy(p_item, p_dynamic->p_data + offset,
		p_dynamic->element_size);
	// End
	END:
}

int si_array_cmp(const si_array_t* const p_dynamic_a,
	const si_array_t* const p_dynamic_b)
{
	int result = 0;
	if(p_dynamic_a == p_dynamic_b)
	{
		// Pointing to the same struct or same memory.
		goto END;
	}
	// We will treat NULL as the smallest possible value.
	if(NULL == p_dynamic_a)
	{
		// a(0) < b(?)
		result = -1;
		goto END;
	}
	if(NULL == p_dynamic_b)
	{
		// a(?) > b(0)
		result = 1;
		goto END;
	}
	// Different non-NULL structs in memory.
	// Determine bounds of data buffer sizes.
	const size_t a_size = si_array_size(p_dynamic_a);
	const size_t b_size = si_array_size(p_dynamic_b);
	size_t min_size = 0u;
	size_t max_size = 0u;
	if(a_size >= b_size)
	{
		max_size = a_size;
		min_size = b_size;
	}
	else
	{
		max_size = b_size;
		min_size = a_size;
	}
	// Compare safe bounds values.
	uint8_t next_a = 0u;
	uint8_t next_b = 0u;
	for(size_t i = 0u; i < min_size; i++)
	{
		next_a = ((uint8_t*)p_dynamic_a->p_data)[i];
		next_b = ((uint8_t*)p_dynamic_b->p_data)[i];
		if(next_a == next_b)
		{
			continue;
		}
		result = -1;
		if(next_a > next_b)
		{
			result = 1;
		}
		goto END;
	}
	if(min_size == max_size)
	{
		// Equal :)
		goto END;
	}
	/* Unsafe bounds (Undefined behavior in C memcmp.)
	 * For our implementation, even if values are the same and the dynamic with
	 * the larger element size only containing 0x00 in the extra space, we will
	 * return a result as being not equal.
	 * E.G.
	 * 0xDEADBEEF != 0x00000000DEADBEEF
	 * Instead we will return the larger allocated size as the largest.
	 */
	result = -1;
	if(a_size > b_size)
	{
		result = 1;
	}
END:
	return result;
}

void fprint_si_dynamic(FILE* const p_file, const si_array_t* const p_dynamic)
{
	// Validate parameters
	if((NULL == p_file) || (NULL == p_dynamic))
	{
		goto END;
	}
	// Begin
	fprintf(p_file, "{data address: ");
	if(NULL == p_dynamic->p_data)
	{
		fprintf(p_file, "NULL, ");
	}
	else
	{
		fprintf(p_file, "%p, ", p_dynamic->p_data);
	}
	fprintf(p_file, "element size: %lu, ", p_dynamic->element_size);
	fprintf(p_file, "capacity: %lu", p_dynamic->capacity);
	fprintf(p_file, "}");
	// End
END:
	return;
}

void si_array_free(si_array_t* const p_dynamic)
{
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	if (NULL != p_dynamic->p_data)
	{
		free(p_dynamic->p_data);
		p_dynamic->p_data = NULL;
	}
	p_dynamic->capacity = 0u;
	// End
END:
	return;
}

#ifdef __cplusplus
}
#endif //__cplusplus
