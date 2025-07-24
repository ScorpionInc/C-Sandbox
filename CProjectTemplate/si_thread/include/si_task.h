//si_task.h

#include <stdbool.h>

#include "si_dynamic.h"
#include "si_linked_list.h"

#define SI_TASK_DEFAULT_ID 0u

// 8-Bit Field
// 1000 pppp = Loop Task (task gets requeued on completion)
// 0100 pppp = Completed Flag (Marks a task as ready to read/pop)
// 0010 pppp = Running Flag (Marks task as being started)
// 0001 pppp = Returns Flag (Is not safe to remove on completion.)
// LSB-Nibble= priority (0-15)

#define SI_TASK_LOOPS_MASK    0x80
#define SI_TASK_COMPLETE_MASK 0x40
#define SI_TASK_RUNNING_MASK  0x20
#define SI_TASK_RETURNS_MASK  0x10
#define SI_TASK_PRIORITY_MAX  0x0F
#define SI_TASK_PRIORITY_MIN  0x00

#ifndef SI_TASK_H
#define SI_TASK_H

typedef void (*si_task_t)(si_linked_list_t* optional_arguments, si_dynamic_t* stores_results);

typedef struct si_task
{
	uint8_t field;
	si_dynamic_t return_value;
	si_linked_list_t* p_arguments;
	si_task_t p_function;
	size_t task_id;
} si_task;

void si_task_new_5(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field,
	const si_linked_list_t* const p_arguments, const size_t task_id);
void si_task_new_4(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field,
	const si_linked_list_t* const p_arguments);
void si_task_new_3(si_task* const p_task,
	const si_task_t const p_function, const uint8_t field);
void si_task_new(si_task* const p_task,
	const si_task_t const p_function);

bool si_task_is_looping(const si_task* const p_task);
bool si_task_is_complete(const si_task* const p_task);
bool si_task_is_running(const si_task* const p_task);
bool si_task_returns_value(const si_task* const p_task);
uint8_t si_task_priority(const si_task* const p_task);

void si_task_start(si_task* const p_task);

void si_task_stop(si_task* const p_task);

void si_task_kill(si_task* const p_task);

void si_task_free(si_task* const p_task);

#endif//SI_TASK_H
