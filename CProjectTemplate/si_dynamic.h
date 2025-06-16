/* si_dynamic.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for managing dynamically allocated buffers.
 * Created: 20150501
 * Updated: 20250611
//*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
} si_dynamic;

/** Doxygen
 * @brief Initializes fields within si_dynamic struct being pointed at.
 *
 * @param p_dynamic Pointer addressing struct to be initialized.
 * @param element_size Size of a single item in the dynamic buffer.
 * @param capacity Number of said item to be stored in the buffer.
 * @param settings si_realloc_settings determining how to reallocate.
 */
void si_dynamic_new_3(si_dynamic* p_dynamic, const size_t element_size,
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
 * @brief Determines if pointer lies with allocated data/buffer address space.
 *
 * @param p_dynamic Pointer to the struct to use for bounds check.
 * @param p_test Pointer to check if it is within allocated memory.
 *
 * @return Returns bool true if true. Return false otherwise.
 */
bool si_dynamic_is_pointer_within(const si_dynamic* p_dynamic,
	const void* p_test);

/** Doxygen
 * @brief Calculates the byte offset into the buffer of the pointer.
 *
 * @param p_dynamic Pointer to si_dynamic struct to do checks with.
 * @param p_test Pointer to allocated memory to do checks on.
 *
 * @return On good returns offset of pointer in heap. SIZE_MAX on failure/max.
 */
size_t si_dynamic_find_pointer_offset(const si_dynamic* p_dynamic,
	const void* p_test);

/** Doxygen
 * @brief Determines if pointer lies at an element starting address.
 *
 * @param p_dynamic Pointer to the struct to use for bounds check.
 * @param p_test Pointer to check if it is pointing at an element.
 *
 * @return Returns bool true if true. Return false otherwise.
 */
bool si_dynamic_is_pointer_element(const si_dynamic* p_dynamic,
	const void* p_test);

/** Doxygen
 * @brief Returns the index of pointer or SIZE_MAX if invalid.
 *
 * @param p_dynamic Pointer to si_dynamic struct to do checks with.
 * @param p_test Pointer to allocated memory to do checks on.
 *
 * @return On good returns index of element in heap. SIZE_MAX on failure/max.
 */
size_t si_dynamic_find_pointer_index(const si_dynamic* p_dynamic,
	const void* p_test);

/** Doxygen
 * @brief Determines where in allocated memory element index starts.
 *
 * @param p_dynamic Pointer to si_dynamic struct
 * @param index Index of the element in the buffer to find address of.
 *
 * @return Success returns pointer into heap memory of element_size. Else NULL.
 */
void* si_dynamic_at(const si_dynamic* p_dynamic,
	const size_t index);

/** Doxygen
 * @brief Gets and returns first elements address of allocated memory.
 *
 * @param p_dynamic Pointer to si_dynamic struct to read from.
 *
 * @return Returns pointer into heap memory of element size on success. Or NUll
 */
void* si_dynamic_first(const si_dynamic* p_dynamic);

/** Doxygen
 * @brief Gets and returns last elements address of allocated memory.
 *
 * @param p_dynamic Pointer to si_dynamic struct to read from.
 *
 * @return Returns pointer into heap memory of element size on success. Or NUll
 */
void* si_dynamic_last(const si_dynamic* p_dynamic);

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
