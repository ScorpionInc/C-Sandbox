/* si_dynamic.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for managing dynamically allocated buffers.
 * Created: 20150501
 * Updated: 20250611
//*/

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "si_realloc_settings.h"

#ifndef SI_DYNAMIC_H
#define SI_DYNAMIC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_dynamic
{
	void* data;
	size_t element_size;
	size_t capacity;
	si_realloc_settings settings;
} si_dynamic;

/** Doxygen
 * @brief Initializes fields within si_dynamic struct being pointed at.
 *
 * @param p_dynamic Pointer addressing struct to be initialized.
 * @param element_size Size of a single item in the dynamic buffer.
 * @param capacity Number of said item to be stored in the buffer.
 * @param settings si_realloc_settings determining how to reallocate.
 */
void si_dynamic_new4(si_dynamic* p_dynamic, const size_t element_size,
	const size_t capacity, const si_realloc_settings* settings);
void si_dynamic_new3(si_dynamic* p_dynamic, const size_t element_size,
	const size_t capacity);
void si_dynamic_new (si_dynamic* p_dynamic, const size_t element_size);

/** Doxygen
 * @brief Calculates the size in bytes allocated for struct at p_dynamic
 *
 * @param p_dynamic Pointer to si_dynamic struct to size.
 * @return Returns the allocated size of the buffer in bytes.
 */
size_t si_dynamic_size(const si_dynamic* p_dynamic);

/* Doxygen
 * @brief Sets capacity of buffer of items to new_capacity. Ignores settings.
 *
 * @param p_dynamic Pointer to si_dynamic struct to have a resized capacity.
 * @param new_capacity Number of items of element_size to be stored in buffer.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_dynamic_resize(si_dynamic* p_dynamic,
	const size_t new_capacity);

/** Doxygen
 * @brief Increases the capacity of p_dynamic by grow_value using grow_method.
 *
 * @param p_dynamic Pointer to struct of allocated dynamic memory to be grown.
 * @param dynamics Pointer to si_realloc_settings struct holding grow settings.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_dynamic_grow(si_dynamic* p_dynamic);

/** Doxygen
 * @brief Lowers the capacity of p_dynamic by shrink_value using shrink_method.
 *
 * @param p_dynamic Pointer to struct of allocated dynamic memory to be shrunk.
 * @param dynamics Pointer to si_dynamics struct holding shrink settings.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_dynamic_shrink(si_dynamic* p_dynamic);

/** Doxygen
 * @brief Determines if count of element_size items could fit after next shrink
 *
 * @param p_dynamic Pointer to the struct to be checked if safe to shrink.
 * @param current_count Count of currently used items in buffer.
 *
 * @return Returns true if buffer can be shrunk. False is returned otherwise.
 */
bool si_dynamic_is_safe_to_shrink(si_dynamic* p_dynamic,
	const size_t current_count);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_dynamic Pointer to si_dynamic struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_dynamic_set(si_dynamic* p_dynamic,
	const size_t index, const void* p_item);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_dynamic Pointer to si_dynamic struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_dynamic_get(const si_dynamic* p_dynamic,
	const size_t index, void* p_item);

/** Doxygen
 * @brief Frees buffer if allocated.
 */
void si_dynamic_free(si_dynamic* p_dynamic);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_DYNAMIC_H
