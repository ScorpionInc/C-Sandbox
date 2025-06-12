#include "si_stack.h"

void si_stack_new_3(si_stack* p_stack, const size_t element_size,
    const size_t initial_capacity)
{
	p_stack->count = 0u;
	init_si_dynamic_3(&(p_stack->dynamic), element_size, initial_capacity);
}
inline void si_stack_new(si_stack* p_stack, const size_t element_size)
{
	// Default capacity is 0u
	si_stack_new_3(p_stack, element_size, 0u);
}

bool si_stack_is_full(const si_stack* p_stack)
{
	bool is_full = true;
	// Validate parameter
	if(NULL == p_stack)
	{
		goto END;
	}
	// Begin
	is_full = (p_stack->count >= p_stack->dynamic.capacity);
	// End
END:
	return;
}

bool si_stack_is_empty(const si_stack* p_stack)
{
	bool is_empty = true;
	// Validate parameter
	if(NULL == p_stack)
	{
		goto END;
	}
	// Begin
	is_empty = (p_stack->count == 0u);
	// End
END:
	return;
}

void si_stack_push(si_stack* p_stack, const void* p_item)
{
	// Validate parameters
	if((NULL == p_stack) || (NULL == p_item))
	{
		goto END;
	}
	// Begin
	if(si_stack_is_full(p_stack))
	{
		si_dynamic_grow(&(p_stack->dynamic));
		if(si_stack_is_full(p_stack))
		{
			// Failed to grow
			goto END;
		}
	}
	si_dynamic_set(&(p_stack->dynamic), p_stack->count, p_item);
	p_stack->count++;
	// End
END:
	return;
}

void si_stack_pop(si_stack* p_stack, void* p_item)
{
	// Validate parameter
	if(NULL == p_stack)
	{
		goto END;
	}
	// Begin
	if(si_stack_is_empty(p_stack))
	{
		goto END;
	}
	si_dynamic_get(&(p_stack->dynamic), p_stack->count - 1u, p_item);
	size_t shrink_capacity = si_realloc_next_shrink_capacity(
		&(p_stack->dynamic.settings), p_stack->dynamic.capacity);
	if(p_stack->count < shrink_capacity)
	{
		si_dynamic_shrink(&(p_stack->dynamic));
	}
	p_stack->count--;
	// End
END:
	return;
}

void si_stack_free(si_stack* p_stack)
{
	// Validate parameter
	if(p_stack == NULL)
	{
		goto END;
	}
	// Begin
	free_si_dynamic(&(p_stack->dynamic));
	// End
END:
	return;
}
