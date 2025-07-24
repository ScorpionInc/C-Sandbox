//si_dynamic.c

#include "si_dynamic.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void si_dynamic_new_3(si_dynamic_t* p_dynamic, const size_t element_size,
	const size_t capacity)
{
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	p_dynamic->data = calloc(capacity, element_size);
	if (NULL == p_dynamic->data)
	{
		goto END;
	}
	p_dynamic->element_size = element_size;
	p_dynamic->capacity = capacity;
	// End
	END:
		return;
}
inline void si_dynamic_new(si_dynamic_t* p_dynamic, const size_t element_size)
{
	// Default value of capacity is 0u
	si_dynamic_new_3(p_dynamic, element_size, 0u);
}

size_t si_dynamic_size(const si_dynamic_t* p_dynamic)
{
	size_t result = 0u;
	// Validate Parameters
	if (NULL == p_dynamic)
	{
		goto END;
	}
	if ((NULL == p_dynamic->data) || (0u == p_dynamic->element_size))
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

bool si_dynamic_resize(si_dynamic_t* p_dynamic,
    const size_t new_capacity)
{
	bool result = false;
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	const size_t new_size =	(p_dynamic->element_size * new_capacity);
	// Check for overflows
	if(0u < p_dynamic->element_size)
	{
		if((new_size / p_dynamic->element_size) != new_capacity)
		{
			goto END;
		}
	}
	void* tmp = realloc(p_dynamic->data, new_size);
	if(NULL == tmp)
	{
		goto END;
	}
	p_dynamic->data = tmp;
	p_dynamic->capacity = new_capacity;
	result = true;
	// End
END:
	return result;
}

bool si_dynamic_is_pointer_within(const si_dynamic_t* p_dynamic,
	const void* p_test)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_dynamic) || (NULL == p_test))
	{
		goto END;
	}
	if(NULL == p_dynamic->data)
	{
		goto END;
	}
	// Begin
	result = (p_test >= p_dynamic->data);
	if(result)
	{
		const size_t size = si_dynamic_size(p_dynamic);
		result = (p_test < (p_dynamic->data + size));
	}
	// End
END:
	return result;
}

size_t si_dynamic_find_pointer_offset(const si_dynamic_t* p_dynamic,
	const void* p_test)
{
	size_t offset = SIZE_MAX;
	// Validate parameters
	if( ! si_dynamic_is_pointer_within(p_dynamic, p_test) )
	{
		// is_pointer_within verifies values are not NULL.
		goto END;
	}
	// Begin
	const size_t size = si_dynamic_size(p_dynamic);
	offset = (p_dynamic->data + size) - p_test;
	// End
END:
	return offset;
}

bool si_dynamic_is_pointer_element(const si_dynamic_t* p_dynamic,
	const void* p_test)
{
	bool result = false;
	// Validate parameters
	if( ! si_dynamic_is_pointer_within(p_dynamic, p_test) )
	{
		// is_pointer_within verifies values are not NULL.
		goto END;
	}
	if(p_dynamic->element_size == 0u)
	{
		goto END;
	}
	// Begin
	const size_t offset = si_dynamic_find_pointer_offset(p_dynamic, p_test);
	result = ((offset % p_dynamic->element_size) == 0u);
	// End
END:
	return result;
}

size_t si_dynamic_find_pointer_index(const si_dynamic_t* p_dynamic,
	const void* p_test)
{
	size_t index = SIZE_MAX;
	// Validate parameters
	// Begin
	if( ! si_dynamic_is_pointer_element(p_dynamic, p_test))
	{
		// is_pointer_element validates values are not NULL.
		// is_pointer_element validates element_size > 0
		goto END;
	}
	const size_t offset = si_dynamic_find_pointer_offset(p_dynamic, p_test);
	index = offset / p_dynamic->element_size;
	// End
END:
	return index;
}

void* si_dynamic_at(const si_dynamic_t* p_dynamic,
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
	p_item = (p_dynamic->data + (p_dynamic->element_size * index));
	// End
END:
	return p_item;
}
inline void* si_dynamic_first(const si_dynamic_t* p_dynamic)
{
	return si_dynamic_at(p_dynamic, 0u);
}
inline void* si_dynamic_last(const si_dynamic_t* p_dynamic)
{
	return si_dynamic_at(p_dynamic, p_dynamic->capacity - 1u);
}

void si_dynamic_set(si_dynamic_t* p_dynamic,
	const size_t index, const void* p_item)
{
	// Validate Parameters
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	const size_t offset = (p_dynamic->element_size * index);
	const size_t allocated_size = si_dynamic_size(p_dynamic);
	// Bounds Check
	if(offset >= allocated_size)
	{
		goto END;
	}
	// Begin
	memcpy(p_dynamic->data + offset, p_item,
		p_dynamic->element_size);
	// End
END:
}

void si_dynamic_get(const si_dynamic_t* p_dynamic,
	const size_t index, void* p_item)
{
	// Validate parameters
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	const size_t offset = (p_dynamic->element_size * index);
	const size_t allocated_size = si_dynamic_size(p_dynamic);
	// Bounds Check
	if(offset >= allocated_size)
	{
		goto END;
	}
	// Begin
	memcpy(p_item, p_dynamic->data + offset,
		p_dynamic->element_size);
	// End
	END:
}

int si_dynamic_cmp(const si_dynamic_t* const p_dynamic_a,
	const si_dynamic_t* const p_dynamic_b)
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
	const size_t a_size = si_dynamic_size(p_dynamic_a);
	const size_t b_size = si_dynamic_size(p_dynamic_b);
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
		next_a = ((uint8_t*)p_dynamic_a->data)[i];
		next_b = ((uint8_t*)p_dynamic_b->data)[i];
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

void fprint_si_dynamic(FILE* const p_file, const si_dynamic_t* const p_dynamic)
{
	// Validate parameters
	if((NULL == p_file) || (NULL == p_dynamic))
	{
		goto END;
	}
	// Begin
	fprintf(p_file, "{data address: ");
	if(NULL == p_dynamic->data)
	{
		fprintf(p_file, "NULL, ");
	}
	else
	{
		fprintf(p_file, "%p, ", p_dynamic->data);
	}
	fprintf(p_file, "element size: %lu, ", p_dynamic->element_size);
	fprintf(p_file, "capacity: %lu", p_dynamic->capacity);
	fprintf(p_file, "}");
	// End
END:
	return;
}

void si_dynamic_free(si_dynamic_t* const p_dynamic)
{
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	if (NULL != p_dynamic->data)
	{
		free(p_dynamic->data);
		p_dynamic->data = NULL;
	}
	// End
END:
	return;
}

#ifdef __cplusplus
}
#endif //__cplusplus
