/* si_linked_list.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define struct with functions 4 managing dynamic single-linked lists
 * Created: 20250618
 * Updated: 20250623
//*/

#include <stdbool.h>
#include <stdio.h>

#include "si_dynamic.h"


// Should linked list free old values (ownership) when being replaced by default.
#define SI_LINKED_LIST_DEFAULT_DO_FREE true

// Should assignments / insertions copy dynamics shallow or deep by default.
#define SI_LINKED_LIST_DEFAULT_DEEP_COPY false


#ifndef SI_LINKED_LIST_H
#define SI_LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_linked_list_t
{
	si_dynamic_t dynamic;
	struct si_linked_list_t* next;
} si_linked_list_t;

/** Doxygen
 * @brief Allocates new empty si_linked list nodes to increase list capacity.
 *
 * @param p_list Pointer to list to grow capacity from.
 * @param amount Number of nodes after this to allocate and add.
 *
 * @return Returns stdbool true on success. False otherwise.
 */
bool si_linked_list_grow_by(si_linked_list_t* const p_list,
	const size_t amount);

/** Doxygen
 * @brief Allocates new empty si_linked list nodes to increase list capacity.
 *
 * @param p_list Pointer to list to grow capacity from.
 * @param capacity Number of nodes after and including current node are desired
 *
 * @return Returns stdbool true on success. False otherwise.
 */
bool si_linked_list_grow_to(si_linked_list_t* const p_list,
	const size_t capacity);

/** Doxygen
 * @brief Initializes list struct at pointer.
 *
 * @param p_list Pointer to linked list to be initialized.
 * @param initial_capacity Number of list nodes to initialize.
 */
void si_linked_list_new_2(si_linked_list_t* const p_list,
	const size_t initial_capacity);
void si_linked_list_new(si_linked_list_t* const p_list);

/** Doxygen
 * @brief Walks the pointed at linked list counting elements.
 *
 * @param p_list Pointer to linked list to counter elements of.
 *
 * @return Returns the number of elements in the linked list.
 */
size_t si_linked_list_count(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Walks the pointed at linked list counting list nodes.
 *
 * @param p_list Pointer to the linked list struct to be walked.
 *
 * @return Returns the number of nodes in the linked list.
 */
size_t si_linked_list_capacity(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Determines if the list contains no set values. Or is empty.
 *
 * @param p_list Pointer to si_linked_list struct.
 *
 * @return Returns stdbool true if empty. Returns false otherwise.
 */
bool si_linked_list_is_empty(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Determines if the list values are at capacity. Or is full.
 *
 * @param p_list Pointer to si_linked_list struct.
 *
 * @return Returns stdbool true if full. Returns false otherwise.
 */
bool si_linked_list_is_full(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Returns pointer to si_linked_list node index away from current node.
 *
 * @param p_list Pointer to current linked_list node to walk index times.
 * @param index Number of nodes away from the current node to walk.
 *
 * @return Returns pointer to linked list node by index. Or NULL on error.
 */
si_linked_list_t* si_linked_list_node_at(const si_linked_list_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Returns pointer to dynamic at index offset into list.
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns pointer to si_dynamic struct at index.
 */
si_dynamic_t* si_linked_list_at(const si_linked_list_t* const p_list,
	const size_t index);

/** Doxygen
 *
 *
 * @param p_list Pointer to si_linked_list struct.
 *
 * @return Returns pointer to raw heap value from list at index.
 */
void* si_linked_list_raw_at(const si_linked_list_t* const p_list,
	const size_t index);

/** Doxygen
 * @brief Finds first index of a si_dynamic in linked list.
 *
 * @param p_list Pointer to a si_linked_list struct to be searched.
 * @param p_data Pointer to si_dynamic struct to search for of element_size.
 * @param start_index Offset into p_list to start searching for the needle.
 *
 * @return Returns index from p_list of first match. SIZE_MAX on error.
 */
size_t si_linked_list_find_3(const si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t start_index);
size_t si_linked_list_find(const si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data);

/** Doxygen
 * @brief Finds the first index of raw data in linked list.
 *
 * @param p_list Pointer to the si_linked_list struct to be searched.
 * @param p_data Pointer to the raw data to be searched for. (The needle)
 * @param data_size Number of bytes of raw data in the needle.
 * @param start_index Offset into p_list to start searching. Default: 0u
 *
 * @return Returns index from p_list of first match. SIZE_MAX on error.
 */
size_t si_linked_list_find_raw_4(const si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size, const size_t start_index);
size_t si_linked_list_find_raw(const si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size);

/** Doxygen
 * @brief Sets the dynamic's value at index in list to new value.
 *
 * @param p_list Pointer to si_linked_list struct to set value in.
 * @param p_dynamic Pointer to the dynamic struct to set the new value to.
 * @param deep_copy Bool should assign value as a deep copy? Default: true
 * @param do_free Bool Should value be freed before reassigned. Default: true
 * @param index Index of the node from current node to be set. Default: 0u
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_linked_list_assign_5(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_dynamic, const bool deep_copy,
	const bool do_free, const size_t index);
bool si_linked_list_assign_4(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_dynamic, const bool deep_copy,
	const bool do_free);
bool si_linked_list_assign_3(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_dynamic, const bool deep_copy);
bool si_linked_list_assign(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_dynamic);

/** Doxygen
 * @brief Sets the value of the dynamic at p_list to new raw value.
 *
 * @param p_list Pointer to si_linked_list struct.
 * @param p_data Pointer to raw data buffer.
 * @param data_size Number of bytes in the raw data buffer.
 * @param do_free Should the old value be freed before assigned.
 *
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_linked_list_assign_raw_4(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size, const bool do_free);
bool si_linked_list_assign_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size);

/** Doxygen
 * @brief Inserts new node with p_data of element size into next.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 * @param deep_copy Bool should assign value as a deep copy? Default: false
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert_next_3(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const bool deep_copy);
bool si_linked_list_insert_next(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data);

/** Doxygen
 * @brief Inserts new node with p_data of data_size into next.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 * @param data_size Number of bytes in the data element.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert_next_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size);

/** Doxygen
 * @brief Adds new node into list at index with value from p_data.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 * @param index is the location within the linked list to insert.
 * @param deep_copy Bool should assign value as a deep copy? Default: false
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert_4(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t index, const bool deep_copy);
bool si_linked_list_insert(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const size_t index);

/** Doxygen
 * @brief Adds new node into list at index with value from raw p_data.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to data of data_size to add to list.
 * @param data_size Number of bytes in p_data to be assigned.
 * @param index is the location within the linked list to insert.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size, const size_t index);

/** Doxygen
 * @brief Returns pointer to last linked list node stored in the list.
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns pointer to last si_linked_list in p_list.
 */
si_linked_list_t* si_linked_list_last_node(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Gets pointer to first unassigned node in p_list. NULL if full.
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns pointer to next node without assigned value in p_list.
 */
si_linked_list_t* si_linked_list_next_available(const si_linked_list_t* const p_list);

/** Doxygen
 * @brief Inserts new linked list node with data value to the end.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to data of element_size to add to list.
 * @param deep_copy Bool should assign value as a deep copy? Default: false
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_append_3(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data, const bool deep_copy);
bool si_linked_list_append(si_linked_list_t* const p_list,
	const si_dynamic_t* const p_data);

/** Doxygen
 * @brief Inserts new linked list node with raw data value to the end.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to data of data_size to add to list.
 * @param data_size Number of bytes in the data's value buffer.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_append_raw(si_linked_list_t* const p_list,
	const void* const p_data, const size_t data_size);

/** Doxygen
 * @brief Removes linked list node after the node specified by p_list.
 *
 * @param p_list Pointer to a si_linked_list struct to remove it's next.
 * @param do_free Should value in next node pointed at be freed? Default: true
 *
 * @return Returns True on success. Returns false otherwise.
 */
bool si_linked_list_remove_next_2(si_linked_list_t* const p_list,
	const bool do_free);
bool si_linked_list_remove_next(si_linked_list_t* const p_list);

/** Doxygen
 * @brief Removes linked list node specified by p_list.
 *
 * @param p_list Pointer to the si_linked_list struct to remove.
 * @param do_free Should value in next node pointed at be freed? Default: true
 *
 * @return Returns True on success. Returns false otherwise.
 */
bool si_linked_list_remove_2(si_linked_list_t* p_list, const bool do_free);
bool si_linked_list_remove(si_linked_list_t* p_list);

/** Doxygen
 * @brief
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param index Node offset into the list to remove at.
 * @param do_free Should value be freed before node removal? Default: true
 *
 * @return Returns True on success, False otherwise.
 */
bool si_linked_list_remove_at_3(si_linked_list_t* p_list,
	const size_t index, const bool do_free);
bool si_linked_list_remove_at(si_linked_list_t* p_list,
	const size_t index);

/** Doxygen
 * @brief
 *
 * @param p_list Pointer to a si_linked_list struct.
 */
void si_linked_list_free(si_linked_list_t* const p_list);

/** Doxygen
 * @brief Prints contents of list to file stream.
 *
 * @param p_list Pointer to list to be printed to file stream.
 * @param p_file Pointer to file stream to be printed to.
 */
void si_linked_list_fprint(const si_linked_list_t* const p_list, FILE* const p_file);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_LINKED_LIST
