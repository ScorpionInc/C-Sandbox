/* si_singular_list.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define struct with functions for managing fixed-size data in
 *          dynamicaly allocated single-linked lists.
 * Created: 20250710
 * Updated: 20250711
//*/

#include <stdbool.h>// bool true false
#include <stdint.h> // size_t
#include <stdio.h>  // fprintf
#include <stdlib.h> // calloc free
#include <string.h> // memcpy

// Should lists be circular by default?
#define SI_SINGULAR_LIST_IS_CIRCULAR true
#define SI_SINGULAR_LIST_DEFAULT_CAPACITY 1

#ifndef SI_SINGULAR_LIST_H
#define SI_SINGULAR_LIST_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_singular_list_t
{
	void*  p_data;
	struct si_singular_list_t* p_next;
} si_singular_list_t;

/** Doxygen
 * @brief Initializes list struct at pointer.
 *
 * @param p_list Pointer to linked list to be initialized.
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 */
void si_singular_list_init_3(si_singular_list_t* const p_list,
	const bool is_circular, const size_t initial_capacity);
void si_singular_list_init_2(si_singular_list_t* const p_list,
	const bool is_circular);
void si_singular_list_init(si_singular_list_t* const p_list);

/** Doxygen
 * @brief Allocates, Initializes and returns a new si_singular_list struct.
 *
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 *
 * @return Returns a new pointer into the heap at newly initialized
 *         si_singular_list struct.
 */
si_singular_list_t* si_singular_list_new_2(const bool is_circular,
	const size_t initial_capacity);
si_singular_list_t* si_singular_list_new_1(const bool is_circular);
si_singular_list_t* si_singular_list_new();

/** Doxygen
 * @brief Walks the pointed at linked list counting elements.
 *
 * @param p_list Pointer to linked list to counter elements of.
 *
 * @return Returns the number of non-NULL elements in the linked list.
 */
size_t si_singular_list_count(const si_singular_list_t* const p_list);

/** Doxygen
 * @brief Walks the pointer at si_singular_list and returns number of allocated
 *        list nodes.
 *
 * @param p_list Pointer to the si_singular_list struct to be walked.
 *
 * @return Returns capacity of the list being pointed at.
 */
size_t si_singular_list_capacity(const si_singular_list_t* const p_list);

/** Doxygen
 * @brief Returns pointer to si_singular_list node index away from current node
 *
 * @param p_list Pointer to current linked_list node to walk index times.
 * @param index Number of nodes away from the current node to walk. Index
 *        values larger than capacity wrap around if list is circular.
 *
 * @return Returns pointer to linked list node by index. Or NULL on error.
 */
si_singular_list_t* si_singular_list_node_at(const si_singular_list_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Returns pointer to data at index offset into list.
 *
 * @param p_list Pointer to a si_singular_list struct.
 *
 * @return Returns pointer to si_dynamic struct at index.
 */
void* si_singular_list_at(const si_singular_list_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Finds the first index of raw data in linked list.
 *
 * @param p_list Pointer to the si_singular_list struct to be searched.
 * @param p_data Pointer to the raw data to be searched for. (The needle)
 * @param cmp_f Pointer to comparison function.
 * @param start_index Offset into p_list to start searching. Default: 0u
 *
 * @return Returns index from p_list of first match. SIZE_MAX on error.
 */
size_t si_singular_list_find_4(const si_singular_list_t* const p_list,
	const void* const p_data, int (*p_cmp_f)(const void*, const void*),
	const size_t start_index);
size_t si_singular_list_find(const si_singular_list_t* const p_list,
	const void* const p_data, int (*p_cmp_f)(const void*, const void*));

/** Doxygen
 * @brief Sorts the nodes of the list by provided comparison function.
 *
 * @param p_list Pointer to si_singular_list struct to be sorted.
 * @param p_cmp_f Pointer to comparison function.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_singular_list_sort(si_singular_list_t* const p_list,
	int (*p_cmp_f)(const void*, const void*));

/** Doxygen
 * @brief Inserts new node with p_data of data_size into next.
 *
 * @param p_list Pointer to a si_singular_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 * @param data_size Number of bytes in the data element.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_singular_list_insert_next(si_singular_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Adds new node into list at index with value from raw p_data.
 *
 * @param p_list Pointer to a si_singular_list struct.
 * @param p_data Pointer to data of data_size to add to list.
 * @param data_size Number of bytes in p_data to be assigned.
 * @param index is the location within the linked list to insert.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_singular_list_insert(si_singular_list_t* const p_list,
	const void* const p_data, const size_t index);

/** Doxygen
 * @brief Returns pointer to last linked list node stored in the list.
 *
 * @param p_list Pointer to a si_singular_list struct.
 *
 * @return Returns pointer to last si_singular_list in p_list.
 */
si_singular_list_t* si_singular_list_last_node(const si_singular_list_t* const p_list);

/** Doxygen
 * @brief Links the tail node to the head node to create a circle.
 *
 * @param p_list Pointer to head node of list to make circular.
 */
void si_singular_list_set_circular_2(si_singular_list_t* const p_list,
	const bool is_circular);
void si_singular_list_set_circular(si_singular_list_t* const p_list);

/** Doxygen
 * @brief Determines is the list pointed at is circular or not.
 *
 * @param p_list Pointer to head node of list to test is it's circular.
 *
 * @return Returns a stdbool true if list is circularly linked. False otherwise
 */
bool si_singular_list_is_circular(si_singular_list_t* const p_list);

/** Doxygen
 * @brief Increases capacity of si_singular_list by amount.
 *
 * @param p_list Pointer to si_singular_list structure to be resized.
 * @param amount How many times to allocate a new node on the list p_list.
 *
 * @return Returns the amount of capacity the list increased by after run.
 */
size_t si_singular_list_grow_by(si_singular_list_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Increases capacity of si_singular_list to capacity.
 *
 * @param p_list Pointer to si_singular_list structure to be resized.
 * @param capacity Desired capacity to grow to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns the new capacity of the list after completed.
 */
size_t si_singular_list_grow_to(si_singular_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Decreases capacity of si_singular_list by amount. Removes from tail.
 *
 * @param p_list Pointer to si_singular_list structure to be resized.
 * @param amount How many times to deallocate a node on the list p_list.
 *
 * @return Returns the amount of nodes removed.
 */
size_t si_singular_list_shrink_by(si_singular_list_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Decreases capacity of si_singular_list to capacity. Removes from tail
 *
 * @param p_list Pointer to si_singular_list structure to be resized.
 * @param capacity Desired capacity to shrink to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns new capacity after command has run.
 */
size_t si_singular_list_shrink_to(si_singular_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Changes capacity of si_singular_list to capacity. Removes from tail.
 *
 * @param p_list Pointer to si_singular_list structure to be resized.
 * @param capacity Desired capacity to change to. Values less than current
 *        p_list's capacity are ignored.
 */
void si_singular_list_resize(si_singular_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Inserts new linked list node with raw data value to the end.
 *
 * @param p_list Pointer to a si_singular_list struct.
 * @param p_data Pointer to data of data_size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_singular_list_append(si_singular_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes linked list node after the node specified by p_list.
 *
 * @param p_list Pointer to a si_singular_list struct to remove it's next.
 *
 * @return Returns True on success. Returns false otherwise.
 */
bool si_singular_list_remove_next(si_singular_list_t* const p_list);

/** Doxygen
 * @brief Removes linked list node specified by p_list.
 *
 * @param p_list Pointer to the si_singular_list struct to remove.
 *
 * @return Returns True on success. Returns false otherwise.
 */
bool si_singular_list_remove(si_singular_list_t* p_list);

/** Doxygen
 * @brief Removes node from list at index.
 *
 * @param p_list Pointer to a si_singular_list struct.
 * @param index Node offset into the list to remove at.
 *
 * @return Returns True on success, False otherwise.
 */
bool si_singular_list_remove_at(si_singular_list_t* p_list,
	const size_t index);

/** Doxygen
 * @brief Adds specified data to the list. Either assigning to allocated node
 *        or inserting a new node as needed.
 *
 * @param p_list Pointer to the si_singular_list to have data pushed on/into.
 *
 * @return Returns index of node data was inserted to on success. Returns
 * SIZE_MAX otherwise.
 */
size_t si_singular_list_push(si_singular_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes first non-null data from list. Doesn't deallocate nodes.
 *
 * @param p_list Pointer to the si_singular_list to have data popped of off.
 *
 * @return Returns void* of data that was removed from list on success. Returns
 *         a NULL value pointer otherwise.
 */
void* si_singular_list_pop(si_singular_list_t* const p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at struct.
 *
 * @param p_list Pointer to a si_singular_list struct.
 */
void si_singular_list_free(si_singular_list_t* p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at address.
 *
 * @param pp_list Pointer to Pointer of si_singular_list struct to be freed.
 */
void si_singular_list_free_at(si_singular_list_t** const pp_list);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_SINGULAR_LIST_H
