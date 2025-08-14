/* si_parray.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for interacting with a contigious,
 *          and dynamicly resizing, heap pointer array.
 * Created: 20250812
 * Updated: 20250812
//*/

#include <stdbool.h> // bool, true, false
#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include <stdio.h> // FILE, fprintf()
#include <stdlib.h> // calloc(), free()
#include <string.h> // memcpy()

#include "si_array.h" // si_array_t
#include "si_realloc_settings.h" // si_realloc_settings_t

#ifndef SI_PARRAY_H
#define SI_PARRAY_H

typedef struct si_parray_t
{
	void (*p_free_value)(void*);
	si_realloc_settings_t* p_settings;
	si_array_t array;
} si_parray_t;

void si_parray_init_2(si_parray_t* const p_array,
	const size_t initial_capacity);
void si_parray_init(si_parray_t* const p_array);

si_parray_t* si_parray_new_1(const size_t initial_capacity);
si_parray_t* si_parray_new();

size_t si_parray_size(const si_parray_t* const p_array);

size_t si_parray_count(const si_parray_t* const p_array);

bool si_parray_fit(si_parray_t* const p_array);

bool si_parray_is_pointer_within(si_parray_t* const p_array,
	const void* const p_pointer);

bool si_parray_is_pointer_valid(si_parray_t* const p_array,
	const void** const pp_pointer);

size_t si_parray_pointer_to_index(si_parray_t* const p_array,
	const void** const pp_pointer);

void* si_parray_at(const si_parray_t* const p_array,
	const size_t index);

bool si_parray_has_set(const si_parray_t* const p_array, const size_t index);

void** si_parray_first(const si_parray_t* const p_array);

void** si_parray_last(const si_parray_t* const p_array);

void** si_parray_tail(const si_parray_t* const p_array);

bool si_parray_remove_at(si_parray_t* const p_array, const size_t index);

void si_parray_set(si_parray_t* const p_array, const size_t index,
	const void* p_value);

void si_parray_set_from(si_parray_t* const p_array, const size_t index,
	const void* p_source, const size_t source_size);

size_t si_parray_append(si_parray_t* const p_array, const void* const p_value);

size_t si_parray_append_from(si_parray_t* const p_array,
	const void* const p_source, const size_t source_size);

void si_parray_fprint(FILE* p_file, const si_parray_t* const p_array,
	void (*p_print_value)(FILE*, void*));

void si_parray_free(si_parray_t* const p_array);

void si_parray_destroy(si_parray_t** pp_array);

#endif//SI_PARRAY_H