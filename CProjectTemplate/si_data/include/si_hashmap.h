//si_hashmap.h

#include <stdbool.h>
#include <stdlib.h>

#include "si_map.h"
#include "si_adler.h"

#ifndef SI_HASHMAP_H
#define SI_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_hashmap_t
{
	si_array_t maps;
	size_t (*p_hash_f)(const void* const, const size_t);
} si_hashmap_t;

void si_hashmap_init(si_hashmap_t* const p_hashmap);

si_hashmap_t* si_hashmap_new();

size_t si_hashmap_count(si_hashmap_t* const p_hashmap);

bool si_hashmap_is_empty(si_hashmap_t* const p_hashmap);

bool si_hashmap_has(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size);

size_t si_hashmap_find(si_hashmap_t* const p_hashmap, const void* const p_value,
	void* const p_key, size_t key_size);

bool si_hashmap_insert(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value);

bool si_hashmap_remove(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size);

bool si_hashmap_assign(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value);

void si_hashmap_free(si_hashmap_t* p_hashmap);

void si_hashmap_free_at(si_hashmap_t** pp_hashmap);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_HASHMAP_H
