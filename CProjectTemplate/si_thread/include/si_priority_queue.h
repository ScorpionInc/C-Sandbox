/* si_priority_queue.h - 20250904 */

#ifdef __linux__

#include <pthread.h> // pthread_mutex_init()

#ifndef PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_NORMAL (PTHREAD_MUTEX_TIMED_NP)
#endif//PTHREAD_MUTEX_NORMAL

#else
#error Unsupported Operating System
#endif

#include <stdbool.h> // bool, false, true
#include <stdlib.h> // calloc(), free()

#include "si_queue.h" // si_queue_t
#include "si_parray.h" // si_parray_t
#include "si_realloc_settings.h" // si_realloc_settings_t

#ifndef SI_PRIORITY_QUEUE_H
#define SI_PRIORITY_QUEUE_H

typedef struct si_priority_queue_t
{
	si_realloc_settings_t* p_settings;
	void (*p_free_value)(void*);
	si_parray_t locks;
	si_parray_t queues;
} si_priority_queue_t;

void si_priority_queue_init(si_priority_queue_t* const p_pqueue,
	const size_t priority_count);

si_priority_queue_t* si_priority_queue_new(const size_t priority_count);

size_t si_priority_queue_count(const si_priority_queue_t* const p_pqueue);

bool si_priority_queue_is_empty(const si_priority_queue_t* const p_pqueue);

size_t si_priority_queue_feed(si_priority_queue_t* const p_pqueue,
	const size_t amount);

bool si_priority_queue_enqueue(si_priority_queue_t* const p_pqueue,
	void* const p_data, const size_t priority);

void* si_priority_queue_dequeue(si_priority_queue_t* const p_pqueue);

void si_priority_queue_update_settings(si_priority_queue_t* const p_pqueue);

void si_priority_queue_free(si_priority_queue_t* const p_pqueue);

void si_priority_queue_destroy(si_priority_queue_t** pp_pqueue);

#endif//SI_PRIORITY_QUEUE_H
