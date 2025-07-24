/* si_double_list.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define structs with functions for managing data in
 *          dynamicaly allocated buffered double-linked lists.
 * Created: 20250711
 * Updated: 20250715
//*/

#include <stdbool.h> // true, false
#include <stddef.h> // size_t
#include <stdint.h> // int64_t
#include <stdlib.h> // calloc, free

#ifndef SI_DOUBLE_LIST_H
#define SI_DOUBLE_LIST_H

// Declare constant default values
#define SI_DOUBLE_LIST_DEFAULT_CAPACITY 0
#define SI_DOUBLE_LIST_IS_CIRCULAR true

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_double_node_t si_double_node_t;

typedef struct si_double_list_t
{
	bool is_circular;
	size_t count;
	size_t capacity;
	struct si_double_node_t* p_head;
	struct si_double_node_t* p_tail;
	int (*p_cmp_f)(const void* const, const void* const);
} si_double_list_t;

/** Doxygen
 * @brief Initializes list struct at pointer.
 *
 * @param p_list Pointer to linked list to be initialized.
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 * @param p_cmp_f Pointer to comparison function for data by pointer(uses int).
 */
void si_double_list_init_4(si_double_list_t* const p_list,
	const bool is_circular, const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const));
void si_double_list_init_3(si_double_list_t* const p_list,
	const bool is_circular, const size_t initial_capacity);
void si_double_list_init_2(si_double_list_t* const p_list,
	const bool is_circular);
void si_double_list_init(si_double_list_t* const p_list);

/** Doxygen
 * @brief Allocates, Initializes and returns a new si_double_list struct.
 *
 * @param is_circular Should this list be initialized as a circular list?
 * @param initial_capacity Number of list nodes to initialize.
 * @param p_cmp_f Pointer to comparison function for data by pointer(uses int).
 *
 * @return Returns a new pointer into the heap at newly initialized
 *         si_double_list struct.
 */
si_double_list_t* si_double_list_new_3(const bool is_circular,
	const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const));
si_double_list_t* si_double_list_new_2(const bool is_circular,
	const size_t initial_capacity);
si_double_list_t* si_double_list_new_1(const bool is_circular);
si_double_list_t* si_double_list_new();

/** Doxygen
 * @brief Returns pointer to data at index offset into list.
 *
 * @param p_list Pointer to a si_double_list struct.
 *
 * @return Returns pointer to si_dynamic struct at index.
 */
void* si_double_list_at(const si_double_list_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Determines is the list pointed at is empty or not.
 *
 * @param p_list Pointer to list to test is it's empty.
 *
 * @return Returns a stdbool true if list is empty. False otherwise
 */
bool si_double_list_is_empty(si_double_list_t* const p_list);

/** Doxygen
 * @brief Determines is the list pointed at is at capacity.
 *
 * @param p_list Pointer to list to test is it's full.
 *
 * @return Returns a stdbool true if list is at capacity. False otherwise
 */
bool si_double_list_is_full(si_double_list_t* const p_list);

/** Doxygen
 * @brief Finds the first index of raw data in linked list.
 *
 * @param p_list Pointer to the si_double_list struct to be searched.
 * @param p_data Pointer to the raw data to be searched for. (The needle)
 * @param start_index Offset into p_list to start searching. Default: 0u
 *
 * @return Returns index from p_list of first match. SIZE_MAX on error.
 */
size_t si_double_list_find_3(const si_double_list_t* const p_list,
	const void* const p_data, const size_t start_index);
size_t si_double_list_find(const si_double_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Sorts the nodes of the list by provided comparison function.
 *
 * @param p_list Pointer to si_double_list struct to be sorted.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_double_list_sort(si_double_list_t* const p_list);

/** Doxygen
 * @brief Adds new node into list at index with value from raw p_data.
 *
 * @param p_list Pointer to a si_double_list struct.
 * @param p_data Pointer to data to add to list.
 * @param index is the location within the linked list to insert.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_double_list_insert(si_double_list_t* const p_list,
	const void* const p_data, const size_t index);

/** Doxygen
 * @brief Links the tail node to the head node to create a circle.
 *
 * @param p_list Pointer to head node of list to make circular.
 */
void si_double_list_set_circular_2(si_double_list_t* const p_list,
	const bool is_circular);
void si_double_list_set_circular(si_double_list_t* const p_list);

/** Doxygen
 * @brief Determines is the list pointed at is circular or not.
 *
 * @param p_list Pointer to head node of list to test is it's circular.
 *
 * @return Returns a stdbool true if list is circularly linked. False otherwise
 */
bool si_double_list_is_circular(si_double_list_t* const p_list);

/** Doxygen
 * @brief Increases capacity of si_double_list by amount.
 *
 * @param p_list Pointer to si_double_list structure to be resized.
 * @param amount How many times to allocate a new node on the list p_list.
 *
 * @return Returns number of nodes that were added to the list.
 */
size_t si_double_list_grow_by(si_double_list_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Increases capacity of si_double_list to capacity.
 *
 * @param p_list Pointer to si_double_list structure to be resized.
 * @param capacity Desired capacity to grow to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns new capacity after list has/if grown.
 */
size_t si_double_list_grow_to(si_double_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Decreases capacity of si_double_list by amount. Removes from tail.
 *
 * @param p_list Pointer to si_double_list structure to be resized.
 * @param amount How many times to deallocate a node on the list p_list.
 *
 * @return Returns number of nodes that were removed from the list.
 */
size_t si_double_list_shrink_by(si_double_list_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Decreases capacity of si_double_list to capacity. Removes from tail
 *
 * @param p_list Pointer to si_double_list structure to be resized.
 * @param capacity Desired capacity to shrink to. Values less than current
 *        p_list's capacity are ignored.
 *
 * @return Returns new capacity after list has/if shrunk.
 */
size_t si_double_list_shrink_to(si_double_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Changes capacity of si_double_list to capacity. Removes from tail.
 *
 * @param p_list Pointer to si_double_list structure to be resized.
 * @param capacity Desired capacity to change to. Values less than current
 *        p_list's capacity are ignored.
 * 
 * @return Returns true on success. Returns false otherwise.
 */
bool si_double_list_resize(si_double_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Inserts new linked list node with raw data value to the end.
 *
 * @param p_list Pointer to a si_double_list struct.
 * @param p_data Pointer to data of data_size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_double_list_append(si_double_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes node from list at index.
 *
 * @param p_list Pointer to a si_double_list struct.
 * @param index Node offset into the list to remove at.
 *
 * @return Returns True on success, False otherwise.
 */
bool si_double_list_remove_at(si_double_list_t* p_list,
	const size_t index);

/** Doxygen
 * @brief Adds specified data to the list. Either assigning to allocated node
 *        or inserting a new node as needed. Goes forward from the head (LIFO).
 *
 * @param p_list Pointer to the si_double_list to have data pushed on/into.
 * @param p_data Pointer value to be added into double list.
 *
 * @return Returns index of node data was inserted to on success. Returns
 * SIZE_MAX otherwise.
 */
size_t si_double_list_push(si_double_list_t* const p_list,
	const void* const p_data);

/** Doxygen
 * @brief Removes first found non-null data from list. Doesn't deallocate
 *        nodes. Goes forward from the head (LIFO).
 *
 * @param p_list Pointer to the si_double_list to have data popped of off.
 *
 * @return Returns void* of data that was removed from list on success. Returns
 *         a NULL value pointer otherwise.
 */
void* si_double_list_pop(si_double_list_t* const p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at struct.
 *
 * @param p_list Pointer to a si_double_list struct.
 */
void si_double_list_free(si_double_list_t* p_list);

/** Doxygen
 * @brief Frees all allocated memory in/by the list at pointed at address.
 *
 * @param pp_list Pointer to Pointer of si_double_list struct to be freed.
 */
void si_double_list_free_at(si_double_list_t** const pp_list);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//si_double_list_H
