/* si_queue.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for managing a dynamically allocated queue.
 * Created: 20150601
 * Updated: 20250707
//*/

#include <stdbool.h>
#include <stddef.h>

#include "si_dynamic.h"
#include "si_realloc_settings.h"

#ifndef SI_QUEUE_H
#define SI_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_queue_t
{
	size_t front;
	size_t back;
	si_realloc_settings settings;
	si_dynamic_t dynamic;
} si_queue_t;

/** Doxygen
 * @brief Initializes a queue struct
 *
 * @param p_queue Pointer to struct to be initialized.
 * @param element_size Size in bytes of the items to be stored.
 * @param initial_capacity Count of items to be stored in the queue. (0)
 * @param p_settings Pointer to si_realloc_settings to read from.
 */
void si_queue_init_4(si_queue_t* p_queue, const size_t element_size,
	const size_t initial_capacity, const si_realloc_settings* p_settings);
void si_queue_init_3(si_queue_t* p_queue, const size_t element_size,
	const size_t initial_capacity);
void si_queue_init(si_queue_t* p_queue, const size_t element_size);

/** Doxygen
 * @brief Allocates and Initializes a new queue struct
 *
 * @param element_size Size in bytes of the items to be stored.
 * @param initial_capacity Count of items to be stored in the queue. (0)
 * @param p_settings Pointer to si_realloc_settings to read from.
 */
si_queue_t* si_queue_new_3(const size_t element_size, const size_t initial_capacity,
	const si_realloc_settings* p_settings);
si_queue_t* si_queue_new_2(const size_t element_size, const size_t initial_capacity);
si_queue_t* si_queue_new(const size_t element_size);

/** Doxygen
 * @brief Counts the elements in the queue
 *
 * @param p_queue The pointer to the struct to count from.
 *
 * @return Returns the number of elements in the queue as size_t.
 */
size_t si_queue_count(const si_queue_t* p_queue);

/** Doxygen
 * @brief Determines if the queue is empty
 *
 * @param p_queue Pointer to the queue to check.
 *
 * @return Returns stdbool true if queue is empty.
 */
bool si_queue_is_empty(const si_queue_t* p_queue);

/** Doxygen
 * @brief Determines if the queue is full
 *
 * @param p_queue Pointer to the queue to check.
 *
 * @return Returns stdbool true if queue is full.
 */
bool si_queue_is_full(const si_queue_t* p_queue);

/** Doxygen
 * @brief Adds an item of element_size to the queue
 *
 * @param p_queue Pointer to the struct to add item to.
 * @param p_item Item whose data is to be added into the queue.
 *
 * @return Returns new count of the queue.
 */
size_t si_queue_enqueue(si_queue_t* p_queue, const void* p_item);



/** Doxygen
 * @brief Sets value at p_item from queue and removes the item(pop).
 *
 * @param p_queue Pointer to the queue struct to get item from.
 * @param p_item Pointer to where to save the results to.
 *
 * @return Returns new count of the queue.
 */
size_t si_queue_dequeue(si_queue_t* p_queue, void* p_item);

/** Doxygen
 * @brief Frees the dynamically allocated values of a queue.
 *
 * @param p_queue Pointer to the queue struct to be free'd.
 */
void si_queue_free(si_queue_t* p_queue);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_QUEUE_H
