//si_task.c

#include "si_task.h"

void si_task_init_5(si_task_t* const p_task,
	const si_task_f const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments, const size_t task_id)
{
	// Validate
	if((NULL == p_task) || (NULL == p_function))
	{
		goto END;
	}
	// Begin
	p_task->field = field;
	p_task->task_id = task_id;
	p_task->return_value = (si_array_t){0};
	p_task->return_value.p_data = NULL;
	(p_task->p_arguments) = (si_singular_list_t*)p_arguments;
	p_task->p_function = p_function;
	// End
END:
	return;
}
inline void si_task_init_4(si_task_t* const p_task,
	const si_task_f const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments)
{
	// Default task_id = SI_TASK_DEFAULT_ID(0u)
	si_task_init_5(p_task, p_function, field, p_arguments, SI_TASK_DEFAULT_ID);
}
inline void si_task_init_3(si_task_t* const p_task,
	const si_task_f const p_function, const uint8_t field)
{
	// Default p_arguments = NULL(No arguments are passed.)
	si_task_init_4(p_task, p_function, field, NULL);
}
inline void si_task_init(si_task_t* const p_task,
	const si_task_f const p_function)
{
	// Default field = 0x00(One-Shot task, with no args and no return.)
	si_task_init_3(p_task, p_function, 0x00);
}


si_task_t* si_task_new_4(const si_task_f const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments, const size_t task_id)
{
	si_task_t* p_new = NULL;
	if(NULL == p_function)
	{
		goto END;
	}
	p_new = calloc(1u, sizeof(si_task_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_task_init_5(p_new, p_function, field, p_arguments, task_id);
END:
	return p_new;
}
inline si_task_t* si_task_new_3(const si_task_f const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments)
{
	// Default task_id = SI_TASK_DEFAULT_ID(0u)
	return si_task_new_4(p_function, field, p_arguments, SI_TASK_DEFAULT_ID);
}
inline si_task_t* si_task_new_2(const si_task_f const p_function, const uint8_t field)
{
	// Default p_arguments = NULL(No arguments are passed.)
	return si_task_new_3(p_function, field, NULL);
}
inline si_task_t* si_task_new(const si_task_f const p_function)
{
	// Default field = 0x00(One-Shot task, with no args and no return.)
	return si_task_new_2(p_function, 0x00);
}

bool si_task_is_looping(const si_task_t* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_LOOPS & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_is_complete(const si_task_t* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_COMPLETE & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_is_running(const si_task_t* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_RUNNING & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_returns_value(const si_task_t* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_RETURNS & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

uint8_t si_task_priority(const si_task_t* const p_task)
{
	uint8_t result = 0u;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	result = (SI_TASK_PRIORITY_MAX & p_task->field);
	// End
END:
	return result;
}

void si_task_start(si_task_t* const p_task)
{
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	if(NULL == p_task->p_function)
	{
		goto END;
	}
	// Begin
	p_task->p_function(p_task->p_arguments, &p_task->return_value);
	// End
END:
	return;
}

void si_task_free(si_task_t* const p_task)
{
	if(NULL == p_task)
	{
		goto END;
	}
	si_array_free(&(p_task->return_value));
	if(NULL != p_task->p_arguments)
	{
		si_singular_list_free(p_task->p_arguments);
		p_task->p_arguments = NULL;
	}
END:
	return;
}

void si_task_free_at(si_task_t** pp_task)
{
	if(NULL == pp_task)
	{
		goto END;
	}
	if(NULL == *pp_task)
	{
		// Already freed
		goto END;
	}
	si_task_free(*pp_task);
	free(*pp_task);
	*pp_task = NULL;
END:
	return;
}
