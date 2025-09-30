/* si_priority_queue.h
 * Language: C
 * Created : 20250904
 * Purpose : Thread-safe FIFO data structure with priority value ordering.
 */

#include "si_mutex.h" // si_mutex_new(), si_mutex_lock(), si_mutex_unlock()
#include "si_queue.h" // si_queue_t
#include "si_parray.h" // si_parray_t
#include "si_realloc_settings.h" // si_realloc_settings_t

#include <stdbool.h> // bool, false, true
#include <stdlib.h> // calloc(), free()

#ifndef SI_PRIORITY_QUEUE_H
#define SI_PRIORITY_QUEUE_H

typedef struct si_priority_queue_t
{
	si_realloc_settings_t* p_settings;
	void (*p_free_value)(void*);
	si_parray_t locks;
	si_parray_t queues;
} si_priority_queue_t;

/** Doxygen
 * @brief Initializes an existing si_priority_queue_t struct values.
 * 
 * @param p_pqueue Pointer to the priority_queue struct to be initialized.
 * @param priority_count Number of priority levels to support.
 */
void si_priority_queue_init(si_priority_queue_t* const p_pqueue,
	const size_t priority_count);

/** Doxygen
 * @brief Allocates and initializes a new priority queue on the heap.
 * 
 * @param p_pqueue Pointer to the priority_queue struct to be initialized.
 * @param priority_count Number of priority levels to support.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_priority_queue_t* si_priority_queue_new(const size_t priority_count);

/** Doxygen
 * @brief Gets the priority count(number of priorites) from a priority queue.
 * 
 * @param p_pqueue Pointer to a priority queue to read from.
 * 
 * @return Returns smallest valid priority count size. (0u on error).
 */
size_t si_priority_queue_priority_count(
	const si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Determine the entries in the priority queue
 * 
 * @param p_pqueue Pointer to the priority_queue struct to read from.
 * 
 * @return Returns size_t count on success. Returns SIZE_MAX otherwise.
 */
size_t si_priority_queue_count(const si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Determines if the queue is empty.
 * 
 * @param p_pqueue Pointer to the priority_queue struct to read from.
 * 
 * @return Returns stdbool true if empty. Returns false otherwise.
 */
bool si_priority_queue_is_empty(const si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Prevents item starvation by moving items up the available priority
 *        levels by size_t amount.
 * 
 * @param p_pqueue Pointer to the priority_queue struct to be feed.
 * @param amount Increase in priority level amount.
 * 
 * @return Returns number of moved items as size_t.
 */
size_t si_priority_queue_feed(si_priority_queue_t* const p_pqueue,
	const size_t amount);

/** Doxygen
 * @brief Adds data to queue by priority.
 * 
 * @param Pointer to the priority_queue struct to add to.
 * @param p_data Pointer value to be enqueued at priority.
 * @param priority Priority of the value as size_t.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_priority_queue_enqueue(si_priority_queue_t* const p_pqueue,
	void* const p_data, const size_t priority);

/** Doxygen
 * @brief Determines and pops highest priority item from a priority queue.
 * 
 * @param Pointer to the priority_queue struct to remove item from.
 * 
 * @return Returns data pointer on success. Returns NULL otherwise.
 */
void* si_priority_queue_dequeue(si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Updates the settings of exisiting internal data structures. Call only
 *        when reassigning settings pointer after priority queue has been used.
 * 
 * @param Pointer to the priority_queue struct to update settings within.
 */
void si_priority_queue_update_settings(si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Frees contents in an already existing priority queue struct.
 * 
 * @param Pointer to the priority_queue struct to free data from within.
 */
void si_priority_queue_free(si_priority_queue_t* const p_pqueue);

/** Doxygen
 * @brief Frees a priority queue as well as all internal data elements.
 * 
 * @param Pointer to the priority_queue struct's heap pointer to free.
 */
void si_priority_queue_destroy(si_priority_queue_t** pp_pqueue);

#endif//SI_PRIORITY_QUEUE_H
