//si_dynamic.c

#include "si_dynamic.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void si_dynamic_new_4(si_dynamic* p_dynamic, const size_t element_size,
	const size_t capacity, const si_realloc_settings* p_settings)
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
	p_dynamic->settings = (si_realloc_settings){};
	if (NULL == p_settings)
	{
		si_realloc_settings_new(&(p_dynamic->settings));
	}
	else
	{
		memcpy(&(p_dynamic->settings), p_settings,
			sizeof(si_realloc_settings));
	}
	// End
	END:
		return;
}
void si_dynamic_new_3(si_dynamic* p_dynamic, const size_t element_size,
	const size_t capacity)
{
	// Default value of settings is determined by si_realloc_settings_new()
	si_realloc_settings settings = (si_realloc_settings){};
	si_realloc_settings_new(&settings);
	si_dynamic_new_4(p_dynamic, element_size, capacity, &settings);
}
inline void si_dynamic_new(si_dynamic* p_dynamic, const size_t element_size)
{
	// Default value of capacity is 0u
	si_dynamic_new_3(p_dynamic, element_size, 0u);
}

size_t si_dynamic_size(const si_dynamic* p_dynamic)
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

bool si_dynamic_resize(si_dynamic* p_dynamic,
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
	if((new_size / p_dynamic->element_size) != new_capacity)
	{
		goto END;
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

bool si_dynamic_grow(si_dynamic* p_dynamic)
{
	bool result = false;
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	const size_t new_capacity = si_realloc_next_grow_capacity(
		&p_dynamic->settings, p_dynamic->capacity
	);
	result = si_dynamic_resize(p_dynamic, new_capacity);
	// End
END:
	return result;
}

bool si_dynamic_shrink(si_dynamic* p_dynamic)
{
	bool result = false;
	// Validate parameter
	if (NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	const size_t new_capacity = si_realloc_next_shrink_capacity(
		&p_dynamic->settings, p_dynamic->capacity
	);
	result = si_dynamic_resize(p_dynamic, new_capacity);
	// End
END:
		return result;
}

bool si_dynamic_is_safe_to_shrink(si_dynamic* p_dynamic,
    const size_t current_count)
{
	bool result = false;
	// Validate parameter
	if(NULL == p_dynamic)
	{
		goto END;
	}
	// Begin
	size_t next_shrink_capacity = si_realloc_next_shrink_capacity(
		&(p_dynamic->settings), p_dynamic->capacity);
	result = (current_count <= next_shrink_capacity);
	// End
END:
	return result;
}

void si_dynamic_set(si_dynamic* p_dynamic,
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

void si_dynamic_get(const si_dynamic* p_dynamic,
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

void si_dynamic_free(si_dynamic* p_dynamic)
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
