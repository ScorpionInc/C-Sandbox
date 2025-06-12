//si_dynamic.c

#include <stdlib.h>

#include "si_dynamic.h"

#include <stdint.h>
#include <string.h>
#include <tgmath.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void init_si_realloc_settings(si_realloc_settings* p_settings)
{
	// Validate parameter
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	p_settings->grow_mode = SI_DEFAULT_RESIZE_MODE;
	p_settings->grow_value = SI_DEFAULT_RESIZE_VALUE;
	p_settings->shrink_mode = SI_DEFAULT_RESIZE_MODE;
	p_settings->shrink_value = SI_DEFAULT_RESIZE_VALUE;
	// End
END:
	return;
}

size_t si_realloc_next_grow_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity)
{
	size_t new_capacity = current_capacity;
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	switch (p_settings->grow_mode)
	{
		case NEVER:
			break;
		case LINEAR:
			// Prevent Overflows
			if (SIZE_MAX - p_settings->grow_value > new_capacity)
			{
				// Overflow detected
				new_capacity = SIZE_MAX;
				break;
			}
			new_capacity += p_settings->grow_value;
			break;
		case SCALAR:
			// Prevent Overflows
			if (p_settings->grow_value != 0.0f)
			{
				// Don't divide by zero.
				if ((new_capacity * p_settings->grow_value) / p_settings->grow_value != new_capacity )
				{
					// Overflow detected
					new_capacity = SIZE_MAX;
					break;
				}
			}
			new_capacity *= p_settings->grow_value;
			break;
		case EXPONENTIAL:
			// Prevent Overflows
			size_t calculated_result = pow(new_capacity, p_settings->grow_value);
			if (calculated_result < new_capacity)
			{
				// Wrap-around detected Overflow
				calculated_result = SIZE_MAX;
			}
			new_capacity = calculated_result;
			break;
		default:
			// Unknown grow mode
			//goto END;
			break;
	}
	// End
END:
	return new_capacity;
}

size_t si_realloc_next_shrink_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity)
{
	size_t new_capacity = current_capacity;
	if (NULL == p_settings)
	{
		goto END;
	}
	// Begin
	switch (p_settings->shrink_mode)
	{
		case NEVER:
			//goto END;
			break;
		case LINEAR:
			// Prevent Underflow
			if (p_settings->shrink_value > new_capacity)
			{
				// Underflow detected
				new_capacity = 0u;
				break;
			}
			new_capacity -= p_settings->shrink_value;
			break;
		case SCALAR:
			// Prevent Underflow
			if (p_settings->shrink_value != 0.0f)
			{
				// Don't divide by zero.
				if ((new_capacity / p_settings->shrink_value) * p_settings->shrink_value != new_capacity)
				{
					// Underflow detected
					new_capacity = 0u;
					break;
				}
			}
			new_capacity /= p_settings->shrink_value;
			break;
		case EXPONENTIAL:
			// Prevent Underflow
			size_t calculated_result = new_capacity;
			if (p_settings->shrink_value != 0.0f)
			{
				// Don't divide by zero.
				calculated_result = pow(new_capacity, 1.0f / p_settings->shrink_value);
				if (calculated_result > new_capacity)
				{
					// Wrap-around detected Underflow
					calculated_result = 0u;
				}
			}
			new_capacity = calculated_result;
			break;
		default:
			// Unknown grow mode
			//goto END;
			break;
	}
	// End
END:
	return new_capacity;
}

void init_si_dynamic_4(si_dynamic* p_dynamic, const size_t element_size, const size_t capacity, const si_realloc_settings* settings)
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
	if (NULL == settings)
	{
		init_si_realloc_settings(&(p_dynamic->settings));
	}
	else
	{
		memcpy(&(p_dynamic->settings), &settings, sizeof(si_realloc_settings));
	}
	// End
	END:
		return;
}
void init_si_dynamic_3(si_dynamic* p_dynamic, const size_t element_size, const size_t capacity)
{
	// Default value of settings is determined by init_si_realloc_settings()
	si_realloc_settings settings = (si_realloc_settings){};
	init_si_realloc_settings(&settings);
	init_si_dynamic_4(p_dynamic, element_size, capacity, &settings);
}
inline void init_si_dynamic_2(si_dynamic* p_dynamic, const size_t element_size)
{
	// Default value of capacity is 0u
	init_si_dynamic_3(p_dynamic, element_size, 0u);
}
inline void init_si_dynamic(si_dynamic* p_dynamic)
{
	// Default value of element_size is SI_DYNAMIC_DEFAULT_ELEMENT_SIZE (1u)
	init_si_dynamic_2(p_dynamic, SI_DYNAMIC_DEFAULT_ELEMENT_SIZE);
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

si_dynamic* si_dynamic_grow_by(si_dynamic* p_dynamic, const size_t count)
{
	// Validate parameters
	if ((NULL == p_dynamic) || (0u == count))
	{
		goto END;
	}
	// Begin
	const size_t new_capacity = ((SIZE_MAX - count) < p_dynamic->capacity) ?
									SIZE_MAX :
									(p_dynamic->capacity + count);
	const size_t new_size = new_capacity *	p_dynamic->element_size;
	void* tmp = realloc(p_dynamic->data, new_size);
	if (NULL != tmp)
	{
		p_dynamic->data = tmp;
	}
	// End
END:
	return p_dynamic;
}

si_dynamic* si_dynamic_grow(si_dynamic* p_dynamic)
{
	if (NULL == p_dynamic)
	{
		goto END;
	}
	const size_t new_capacity = si_realloc_next_grow_capacity(
		&p_dynamic->settings, p_dynamic->capacity
	);
	p_dynamic = si_dynamic_grow_by(p_dynamic, new_capacity - p_dynamic->capacity);
END:
	return p_dynamic;
}

si_dynamic* si_dynamic_shrink_by(si_dynamic* p_dynamic, const size_t count)
{
	if ((NULL == p_dynamic) || (0u == count))
	{
		goto END;
	}
	const size_t new_capacity = (count > p_dynamic->capacity) ?
		0u : p_dynamic->capacity - count;
	const size_t new_size = new_capacity * p_dynamic->element_size;
	void* tmp = realloc(p_dynamic->data, new_size);
	if (NULL != tmp)
	{
		p_dynamic->data = tmp;
	}
END:
	return p_dynamic;
}

si_dynamic* si_dynamic_shrink(si_dynamic* p_dynamic)
{
	if (NULL == p_dynamic)
	{
		goto END;
	}
	const size_t new_capacity = si_realloc_next_shrink_capacity(
		&p_dynamic->settings, p_dynamic->capacity
	);
	p_dynamic = si_dynamic_shrink_by(
		p_dynamic, p_dynamic->capacity - new_capacity
	);
	END:
		return p_dynamic;
}

void si_dynamic_set(si_dynamic* p_dynamic, const size_t index, const void* p_item)
{
	// TODO BOUNDS CHECKING HERE
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	memcpy(p_dynamic->data + (p_dynamic->element_size * index), p_item,
		p_dynamic->element_size);
END:
}

void si_dynamic_get(const si_dynamic* p_dynamic, const size_t index, void* p_item)
{
	// TODO BOUNDS CHECKING HERE
	if ((NULL == p_dynamic) || (NULL == p_item))
	{
		goto END;
	}
	memcpy(p_item,p_dynamic->data + (p_dynamic->element_size * index),
		p_dynamic->element_size);
	END:
}

void free_si_dynamic(si_dynamic* p_dynamic)
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
