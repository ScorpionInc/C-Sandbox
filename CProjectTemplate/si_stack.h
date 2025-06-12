//si_stack.h

#include <stdbool.h>

#include "si_dynamic.h"

#ifndef SI_STACK_H
#define SI_STACK_H

typedef struct si_stack
{
	size_t count;
	si_dynamic dynamic;
} si_stack;

void si_stack_new_3(si_stack* p_stack, const size_t element_size,
	const size_t initial_capacity);
void si_stack_new(si_stack* p_stack, const size_t element_size);

bool si_stack_is_full(const si_stack* p_stack);

bool si_stack_is_empty(const si_stack* p_stack);

void si_stack_push(si_stack* p_stack, const void* p_item);

void si_stack_pop(si_stack* p_stack, void* p_item);

void si_stack_free(si_stack* p_stack);

#endif//SI_STACK_H
