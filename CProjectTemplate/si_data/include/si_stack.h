//si_stack.h

#include <stdbool.h>

#include "si_realloc_settings.h"
#include "si_dynamic.h"

#ifndef SI_STACK_H
#define SI_STACK_H

typedef struct si_stack
{
	size_t count;
	si_realloc_settings settings;
	si_dynamic dynamic;
} si_stack;

/** Doxygen
 * @brief Initializes the values of a si_stack struct.
 *
 * @param Pointer to the struct to be initialized.
 * @param element_size Size in bytes of the items to be stacked.
 * @param initial_capacity Initial amount of items to allot for.
 * @param p_settings Pointer to si_realloc_settings to read from.
 */
void si_stack_new_4(si_stack* p_stack, const size_t element_size,
	const size_t initial_capacity, const si_realloc_settings* p_settings);
void si_stack_new_3(si_stack* p_stack, const size_t element_size,
	const size_t initial_capacity);
void si_stack_new(si_stack* p_stack, const size_t element_size);

/** Doxygen
 * @brief Determines if the allocated memory is currently full or not.
 *
 * @param p_stack Pointer to the si_stack struct to check.
 *
 * @return Returns true is the stack can't add item. False otherwise.
 */
bool si_stack_is_full(const si_stack* p_stack);

/** Doxygen
 * @brief Determines if the allocated memory is currently empty or not.
 *
 * @param p_stack Pointer to the si_stack struct to check.
 *
 * @return Returns true is the stack can't remove item. False otherwise.
 */
bool si_stack_is_empty(const si_stack* p_stack);

/** Doxygen
 * @brief Adds p_item onto top of a stack.
 *
 * @param p_stack Pointer to the stack struct to push the item onto.
 * @param p_item Data/element/item to be pushed onto the stack.
 */
void si_stack_push(si_stack* p_stack, const void* p_item);

/** Doxygen
 * @brief Removes p_item from on top of the stack.
 *
 * @param p_stack Pointer to the stack struct to pop the item off of.
 * @param p_item Pointer/item to hold the value coming off of the stack.
 */
void si_stack_pop(si_stack* p_stack, void* p_item);

/** Doxygen
 * @brief Frees the contents of a si_stack struct.
 *
 * @param p_stack Pointer to the stack struct to be freed.
 */
void si_stack_free(si_stack* p_stack);

#endif//SI_STACK_H
