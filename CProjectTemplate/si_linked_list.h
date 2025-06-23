/* si_linked_list.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define struct with functions 4 managing dynamic single-linked lists
 * Created: 20250618
 * Updated: 20250623
//*/

#include <stdio.h>

#include "si_dynamic.h"

#ifndef SI_LINKED_LIST_H
#define SI_LINKED_LIST_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_linked_list
{
	si_dynamic dynamic;
	struct si_linked_list* next;
} si_linked_list;

/** Doxygen
 * @brief Initializes list struct at pointer.
 *
 * @param p_list Pointer to linked list to be initialized.
 * @param initial_value = Pointer to si_dynamic initial value for root node.
 */
void si_linked_list_new(si_linked_list* const p_list,
	const si_dynamic* const initial_value);

/** Doxygen
 * @brief Returns the next node in the list on success. NULL otherwise.
 *
 * @param p_list Pointer to linked list node to get next node from.
 *
 * @return Returns next node or NULL from p_list.
 */
si_linked_list* si_linked_list_next(const si_linked_list* const p_list);

/** Doxygen
 * @brief Walks the pointed at linked list counting elements.
 *
 * @param p_list Pointer to linked list to counter elements of.
 *
 * @return Returns the number of elements in the linked list.
 */
size_t si_linked_list_count(const si_linked_list* const p_list);

/** Doxygen
 * @brief Returns pointer to dynamic at index offset into list.
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns pointer to si_dynamic struct at index.
 */
si_dynamic* si_linked_list_at(const si_linked_list* const p_list,
	const size_t index);

/** Doxygen
 * @brief Returns pointer to last dynamic stored in the list.
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns pointer to last si_dynamic of in list.
 */
si_dynamic* si_linked_list_last(const si_linked_list* const p_list);

/** Doxygen
 * @brief Finds first index of data in linked list.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to value to search for of element_size.
 *
 * @return Returns linked list node index of first data match.
 */
size_t si_linked_list_find_next_3(const si_linked_list* const p_list,
	const si_dynamic* const p_data, const size_t current_index);
size_t si_linked_list_find_next(const si_linked_list* const p_list,
	const si_dynamic* const p_data);

/** Doxygen
 * @brief Inserts new node with p_data of element size into next.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert_next(si_linked_list* const p_list,
	const si_dynamic* const p_data);

/** Doxygen
 * @brief Adds new node into list at index with value from p_data.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to si_dynamic of element_size to add to list.
 * @param index is the location within the linked list to insert.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_insert(si_linked_list* const p_list, const si_dynamic* const p_data,
	const size_t index);

/** Doxygen
 * @brief Inserts new linked list node with data value to the end.
 *
 * @param p_list Pointer to a si_linked_list struct.
 * @param p_data Pointer to data of element_size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_linked_list_append(si_linked_list* const p_list, const si_dynamic* const p_data);

/** Doxygen
 * @brief
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns
 */
bool si_linked_list_remove_next(si_linked_list* const p_list);

/** Doxygen
 * @brief
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns
 */
bool si_linked_list_remove_at(si_linked_list* const p_list, const size_t index);

/** Doxygen
 * @brief
 *
 * @param p_list Pointer to a si_linked_list struct.
 *
 * @return Returns
 */
void si_linked_list_free(si_linked_list* const p_list);

/** Doxygen
 * @brief Prints contents of list to file stream.
 *
 * @param p_list Pointer to list to be printed to file stream.
 * @param p_file Pointer to file stream to be printed to.
 */
void si_linked_list_fprint(const si_linked_list* const p_list, FILE* const p_file);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_LINKED_LIST
