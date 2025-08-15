//si_task.c

#include "si_task.h"

void si_task_new_5(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments, const size_t task_id)
{
	// Validate
	if((NULL == p_task) || (NULL == p_function))
	{
		goto END;
	}
	// Begin
	p_task->field = field;
	p_task->return_value = (si_array_t){};
	p_task->return_value.p_data = NULL;
	(p_task->p_arguments) = (si_singular_list_t*)p_arguments;
	p_task->p_function = p_function;
	p_task->task_id = task_id;
	// End
END:
	return;
}
inline void si_task_new_4(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field,
	const si_singular_list_t* const p_arguments)
{
	// Default task_id = SI_TASK_DEFAULT_ID(0u)
	si_task_new_5(p_task, p_function, field, p_arguments, SI_TASK_DEFAULT_ID);
}
inline void si_task_new_3(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field)
{
	// Default p_arguments = NULL(No arguments are passed.)
	si_task_new_4(p_task, p_function, field, NULL);
}
inline void si_task_new(si_task* const p_task,
	const si_task_t const p_function)
{
	// Default field = 0x00(One-Shot task, with no args and no return.)
	si_task_new_3(p_task, p_function, 0x00);
}

bool si_task_is_looping(const si_task* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_LOOPS_MASK & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_is_complete(const si_task* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_COMPLETE_MASK & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_is_running(const si_task* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_RUNNING_MASK & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

bool si_task_returns_value(const si_task* const p_task)
{
	bool result = false;
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	if(SI_TASK_RETURNS_MASK & p_task->field)
	{
		result = true;
	}
	// End
END:
	return result;
}

uint8_t si_task_priority(const si_task* const p_task)
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

void si_task_start(si_task* const p_task)
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

void si_task_stop(si_task* const p_task)
{
	//! TODO
	if(NULL == p_task)
	{
		goto END;
	}
END:
	return;
}

void si_task_kill(si_task* const p_task)
{
	//! TODO
	if(NULL == p_task)
	{
		goto END;
	}
END:
	return;
}

void si_task_free(si_task* const p_task)
{
	// Validate
	if(NULL == p_task)
	{
		goto END;
	}
	// Begin
	si_array_free(&(p_task->return_value));
	if(NULL != p_task->p_arguments)
	{
		si_singular_list_free(p_task->p_arguments);
		p_task->p_arguments = NULL;
	}
	// End
END:
	return;
}
