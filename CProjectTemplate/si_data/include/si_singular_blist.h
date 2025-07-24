/* si_singular_blist.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define struct and functions for managing si_linked_list data using
 *          buffered info for a faster but less memory efficient implimentation
 * Created: 20250711
 * Updated: 20250711
//*/

#include "si_singular_list.h"

#ifndef SI_SINGULAR_BLIST_H
#define SI_SINGULAR_BLIST_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_singular_blist_t
{
	size_t count;
	size_t capacity;
	struct si_singular_list_t* p_head;
	struct si_singular_list_t* p_tail;
	int (*p_cmp_f)(const void* const, const void* const);
} si_singular_blist_t;

/** Doxygen
 * @brief Initializes list struct at pointer.
 *
 * @param p_list Pointer to linked list to be initialized.
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 * @param p_cmp_f Pointer to a function that compares 2 values by pointers(int)
 */
void si_singular_blist_init_4(si_singular_blist_t* const p_list,
	const bool is_circular, const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const));
void si_singular_blist_init_3(si_singular_blist_t* const p_list,
	const bool is_circular, const size_t initial_capacity);
void si_singular_blist_init_2(si_singular_blist_t* const p_list,
	const bool is_circular);
void si_singular_blist_init(si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Allocates, Initializes and returns a new si_singular_blist struct.
 *
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 *
 * @return Returns a new pointer into the heap at newly initialized
 *         si_singular_blist struct.
 */
si_singular_blist_t* si_singular_blist_new_3(const bool is_circular,
	const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const));
si_singular_blist_t* si_singular_blist_new_2(const bool is_circular,
	const size_t initial_capacity);
si_singular_blist_t* si_singular_blist_new_1(const bool is_circular);
si_singular_blist_t* si_singular_blist_new();

/** Doxygen
 * @brief Returns pointer to si_singular_blist node index away from current node
 *
 * @param p_si_singular_blist* si_singular_blist_node_at(const si_singular_blist* const p_list,
    const size_t index);list Pointer to current linked_list node to walk index times.
 * @param index Number of nodes away from the current node to walk. Index
 *        values larger than capacity wrap around if list is circular.
 *
 * @return Returns pointer to linked list node by index. Or NULL on error.
 */
si_singular_list_t* si_singular_blist_node_at(const si_singular_blist_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Returns pointer to data at index offset into list.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 *
 * @return Returns pointer to si_dynamic struct at index.
 */
void* si_singular_blist_at(const si_singular_blist_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Finds the first index of raw data in linked list.
 *
 * @param p_list Pointer to the si_singular_blist struct to be searched.
 * @param p_data Pointer to the raw data to be searched for. (The needle)
 * @param start_index Offset into p_list to start searching. Default: 0u
 *
 * @return Returns index from p_list of first match. SIZE_MAX on error.
 */
size_t si_singular_blist_find_3(const si_singular_blist_t* const p_list,
	const void* const p_data, const size_t start_index);
size_t si_singular_blist_find(const si_singular_blist_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Sorts the nodes of the list by provided comparison function.
 *
 * @param p_list Pointer to si_singular_blist struct to be sorted.
 * @param p_cmp_f Pointer to comparison function.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_singular_blist_sort(si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Adds new node into list at index with value from raw p_data.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 * @param p_data Pointer to data of data_size to add to list.
 * @param data_size Number of bytes in p_data to be assigned.
 * @param index is the location within the linked list to insert.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_singular_blist_insert(si_singular_blist_t* const p_list,
	const void* const p_data, const size_t index);

/** Doxygen
 * @brief Returns pointer to last linked list node stored in the list.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 *
 * @return Returns pointer to last si_singular_blist in p_list.
 */
si_singular_blist_t* si_singular_blist_last_node(const si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Links the tail node to the head node to create a circle.
 *
 * @param p_list Pointer to head node of list to make circular.
 */
void si_singular_blist_set_circular_2(si_singular_blist_t* const p_list,
	const bool is_circular);
void si_singular_blist_set_circular(si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Determines is the list pointed at is circular or not.
 *
 * @param p_list Pointer to head node of list to test is it's circular.
 *
 * @return Returns a stdbool true if list is circularly linked. False otherwise
 */
bool si_singular_blist_is_circular(si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Increases capacity of si_singular_blist by amount.
 *
 * @param p_list Pointer to si_singular_blist structure to be resized.
 * @param amount How many times to allocate a new node on the list p_list.
 *
 * @return Returns capacity growth after function completes.
 */
size_t si_singular_blist_grow_by(si_singular_blist_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Increases capacity of si_singular_blist to capacity.
 *
 * @param p_list Pointer to si_singular_blist structure to be resized.
 * @param capacity Desired capacity to grow to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns new capacity after function completes.
 */
size_t si_singular_blist_grow_to(si_singular_blist_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Decreases capacity of si_singular_blist by amount. Removes from tail.
 *
 * @param p_list Pointer to si_singular_blist structure to be resized.
 * @param amount How many times to deallocate a node on the list p_list.
 *
 * @return Returns the amount of nodes removed.
 */
size_t si_singular_blist_shrink_by(si_singular_blist_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Decreases capacity of si_singular_blist to capacity. Removes from tail
 *
 * @param p_list Pointer to si_singular_blist structure to be resized.
 * @param capacity Desired capacity to shrink to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns new capacity after command has run.
 */
size_t si_singular_blist_shrink_to(si_singular_blist_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Changes capacity of si_singular_blist to capacity. Removes from tail.
 *
 * @param p_list Pointer to si_singular_blist structure to be resized.
 * @param capacity Desired capacity to change to. Values less than current
 *        p_list's capacity are ignored.
 */
void si_singular_blist_resize(si_singular_blist_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Inserts new linked list node with raw data value to the end.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 * @param p_data Pointer to data of data_size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_singular_blist_append(si_singular_blist_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes node from list at index.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 * @param index Node offset into the list to remove at.
 *
 * @return Returns True on success, False otherwise.
 */
bool si_singular_blist_remove_at(si_singular_blist_t* p_list,
	const size_t index);

/** Doxygen
 * @brief Adds specified data to the list. Either assigning to allocated node
 *        or inserting a new node as needed.
 *
 * @param p_list Pointer to the si_singular_blist to have data pushed on/into.
 *
 * @return Returns index of node data was inserted to on success. Returns
 * SIZE_MAX otherwise.
 */
size_t si_singular_blist_push(si_singular_blist_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes first non-null data from list. Doesn't deallocate nodes.
 *
 * @param p_list Pointer to the si_singular_blist to have data popped of off.
 *
 * @return Returns void* of data that was removed from list on success. Returns
 *         a NULL value pointer otherwise.
 */
void* si_singular_blist_pop(si_singular_blist_t* const p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at struct.
 *
 * @param p_list Pointer to a si_singular_blist struct.
 */
void si_singular_blist_free(si_singular_blist_t* p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at address.
 *
 * @param pp_list Pointer to Pointer of si_singular_blist struct to be freed.
 */
void si_singular_blist_free_at(si_singular_blist_t** const pp_list);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//si_singular_blist_H
