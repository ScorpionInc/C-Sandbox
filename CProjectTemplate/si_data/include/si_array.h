/* si_array.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct functions for managing an allocated memory buffer.
 * Created: 20150501
 * Updated: 20250813
//*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SI_ARRAY_H
#define SI_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_array_t
{
	void*  p_data;
	size_t element_size;
	size_t capacity;
} si_array_t;

/** Doxygen
 * @brief Initializes fields within si_array_t struct being pointed at.
 *
 * @param p_array Pointer addressing struct to be initialized.
 * @param element_size Size of a single item in the dynamic buffer.
 * @param capacity Number of said item to be stored in the buffer.
 * @param settings si_realloc_settings determining how to reallocate.
 */
void si_array_init_3(si_array_t* p_array, const size_t element_size,
	const size_t capacity);
void si_array_init(si_array_t* p_array, const size_t element_size);

/** Doxygen
 * @brief Allocates & initializes a new si_array_t struct.
 * 
 * @param element_size Size of a single item in the dynamic buffer.
 * @param capacity Number of items in the buffer.
 * 
 * @return Returns pointer to newly allocated si_array_t struct in heap.
 */
si_array_t* si_array_new_2(const size_t element_size, const size_t capacity);
si_array_t* si_array_new(const size_t element_size);

/** Doxygen
 * @brief Calculates the size in bytes allocated for struct at p_array
 *
 * @param p_array Pointer to si_array_t struct to size.
 * @return Returns the allocated size of the buffer in bytes.
 */
size_t si_array_size(const si_array_t* p_array);

/* Doxygen
 * @brief Sets capacity of buffer of items to new_capacity.
 *
 * @param p_array Pointer to si_array_t struct to have a resized capacity.
 * @param new_capacity Number of items of element_size to be stored in buffer.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_array_resize(si_array_t* p_array,
	const size_t new_capacity);

/** Doxygen
 * @brief Determines if pointer lies with allocated data/buffer address space.
 *
 * @param p_array Pointer to the struct to use for bounds check.
 * @param p_test Pointer to check if it is within allocated memory.
 *
 * @return Returns bool true if true. Return false otherwise.
 */
bool si_array_is_pointer_within(const si_array_t* p_array,
	const void* p_test);

/** Doxygen
 * @brief Calculates the byte offset into the buffer of the pointer.
 *
 * @param p_array Pointer to si_array_t struct to do checks with.
 * @param p_test Pointer to allocated memory to do checks on.
 *
 * @return On good returns offset of pointer in heap. SIZE_MAX on failure/max.
 */
size_t si_array_find_pointer_offset(const si_array_t* p_array,
	const void* p_test);

/** Doxygen
 * @brief Determines if pointer lies at an element starting address.
 *
 * @param p_array Pointer to the struct to use for bounds check.
 * @param p_test Pointer to check if it is pointing at an element.
 *
 * @return Returns bool true if true. Return false otherwise.
 */
bool si_array_is_pointer_element(const si_array_t* p_array,
	const void* p_test);

/** Doxygen
 * @brief Returns the index of pointer or SIZE_MAX if invalid.
 *
 * @param p_array Pointer to si_array_t struct to do checks with.
 * @param p_test Pointer to allocated memory to do checks on.
 *
 * @return On good returns index of element in heap. SIZE_MAX on failure/max.
 */
size_t si_array_find_pointer_index(const si_array_t* p_array,
	const void* p_test);

/** Doxygen
 * @brief Determines where in allocated memory element index starts.
 *
 * @param p_array Pointer to si_array_t struct
 * @param index Index of the element in the buffer to find address of.
 *
 * @return Success returns pointer into heap memory of element_size. Else NULL.
 */
void* si_array_at(const si_array_t* p_array,
	const size_t index);

/** Doxygen
 * @brief Gets and returns first elements address of allocated memory.
 *
 * @param p_array Pointer to si_array_t struct to read from.
 *
 * @return Returns pointer into heap memory of element size on success. Or NUll
 */
void* si_array_first(const si_array_t* p_array);

/** Doxygen
 * @brief Gets and returns last elements address of allocated memory.
 *
 * @param p_array Pointer to si_array_t struct to read from.
 *
 * @return Returns pointer into heap memory of element size on success. Or NUll
 */
void* si_array_last(const si_array_t* p_array);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_array Pointer to si_array_t struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_array_set(si_array_t* p_array,
	const size_t index, const void* p_item);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_array Pointer to si_array_t struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_array_get(const si_array_t* p_array,
	const size_t index, void* p_item);

/**Doxygen
 * @brief Swaps the values of two indexs inside the array.
 * 
 * @param p_array Pointer to si_array_t struct whose buffer is to be changed.
 * @param left An index of type size_t to be swapped.
 * @param right An index of type size_t to be swapped.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_array_swp(si_array_t* const p_array,
	const size_t left, const size_t right);

/** Doxygen
 * @brief Compares the raw byte values of two si_array_t structs data.
 *
 * @param p_array_a Pointer to first si_array_t struct to be compared.
 * @param p_array_b Pointer to the second si_array_t struct to be compared.
 *
 * @return Returns int value of 0 if equal. 1 or -1 otherwise simular to memcmp
 */
int si_array_cmp(const si_array_t* const p_array_a,
	const si_array_t* const p_array_b);

/** Doxygen
 * @brief Writes values of si_array_t struct to file for debugging.
 *
 * @param p_file Pointer to the FILE to be printed to.
 * @param p_array Pointer to si_array_t struct that is to be printed to file.
 */
void fprint_si_array(FILE* const p_file, const si_array_t* const p_array);

/** Doxygen
 * @brief Frees buffer if allocated.
 *
 * @param p_array Pointer to si_array_t struct that's to have it's data freed
 */
void si_array_free(si_array_t* const p_array);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_ARRAY_H
