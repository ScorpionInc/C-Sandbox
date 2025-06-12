// si_queue.h

#include <stdbool.h>
#include <stddef.h>

#include "si_dynamic.h"

#ifndef SI_QUEUE_H
#define SI_QUEUE_H

typedef struct si_queue {
    size_t front;
    size_t back;
    si_dynamic dynamic;
} si_queue;

/** Doxygen
 * @brief Initializes a queue struct
 *
 * @param p_queue Pointer to struct to be initialized.
 */
void init_si_queue(si_queue* p_queue);

/** Doxygen
 * @brief Counts the elements in the queue
 *
 * @param p_queue The pointer to the struct to count from.
 */
size_t si_queue_count(const si_queue* p_queue);

/** Doxygen
 * @brief Determines if the queue is empty
 *
 * @param p_queue Pointer to the queue to check.
 */
bool si_queue_is_empty(const si_queue* p_queue);

/** Doxygen
 * @brief Determines if the queue is full
 *
 * @param p_queue Pointer to the queue to check.
 */
bool si_queue_is_full(const si_queue* p_queue);

/** Doxygen
 * @brief Adds an item of element_size to the queue
 *
 * @param p_queue Pointer to the struct to add item to.
 * @param p_item Item whose data is to be added into the queue.
 */
size_t si_queue_enqueue(si_queue* p_queue, const void* p_item);

/** Doxygen
 * @brief Sets value at p_item from queue and removes the item(pop).
 *
 * @param p_queue Pointer to the queue struct to get item from.
 * @param p_item Pointer to where to save the results to.
 */
size_t si_queue_dequeue(si_queue* p_queue, void* p_item);

/** Doxygen
 * @brief Frees the dynamically allocated values of a queue.
 *
 * @param p_queue Pointer to the queue struct to be free'd.
 */
void free_si_queue(si_queue* p_queue);

#endif //SI_QUEUE_H
