/* si_parray.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for interacting with a contigious,
 *          and dynamicly resizing, heap pointer array.
 * Created: 20250812
 * Updated: 20250812
//*/

#include <stdbool.h> // bool, true, false
#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include <stdio.h> // FILE, fprintf()
#include <stdlib.h> // calloc(), free()
#include <string.h> // memcpy()

#include "si_array.h" // si_array_t
#include "si_realloc_settings.h" // si_realloc_settings_t

#ifndef SI_PARRAY_H
#define SI_PARRAY_H

typedef struct si_parray_t
{
	void (*p_free_value)(void*);
	si_realloc_settings_t* p_settings;
	si_array_t array;
} si_parray_t;

/** Doxygen
 * @brief Initializes an already existing si_parray_t at pointer.
 * 
 * @param p_array Pointer to the dynamic pointer array struct.
 * @param initial_capacity size_t of initial pointer capacity.
 */
void si_parray_init_2(si_parray_t* const p_array,
	const size_t initial_capacity);
void si_parray_init(si_parray_t* const p_array);

/** Doxygen
 * @brief Allocates and returns an initialized si_parray_t
 * 
 * @param initial_capacity size_t of inital pointer capacity.
 * 
 * @return Returns allocated pointer on success. NULL otherwise.
 */
si_parray_t* si_parray_new_1(const size_t initial_capacity);
si_parray_t* si_parray_new();

/** Doxygen
 * @brief Returns the memory size in bytes allocated for pointers.
 * 
 * @param p_array Pointer to si_parray_t struct
 * 
 * @return Returns byte count as size_t on success. SIZE_MAX otherwise.
 */
size_t si_parray_size(const si_parray_t* const p_array);

/** Doxygen
 * @brief Returns the number pointers in the array.
 * 
 * @param p_array Pointer to si_parray_t to count.
 * 
 * @return Returns the usage as a size_t on success. SIZE_MAX otherwise.
 */
size_t si_parray_count(const si_parray_t* const p_array);

/** Doxygen
 * @brief Shrinks array down to fit it's item count.
 * 
 * @param p_array Pointer to array struct to shrink.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_parray_fit(si_parray_t* const p_array);

/** Doxygen
 * @brief Determines if a pointer is within the allocated bounds.
 * 
 * @param p_array Pointer to si_parray_t struct
 * @param p_pointer Pointer into memory.
 * 
 * @return Returns stdbool true if within. Returns false otherwise.
 */
bool si_parray_is_pointer_within(si_parray_t* const p_array,
	const void* const p_pointer);

/** Doxygen
 * @brief Determines in pointer is aligned with an array item address.
 * 
 * @param p_array Pointer to the array.
 * @param pp_pointer Pointer into memory.
 * 
 * @return Returns true if pointing at aligned item address otherwise false.
 */
bool si_parray_is_pointer_valid(si_parray_t* const p_array,
	const void** const pp_pointer);

/** Doxygen
 * @brief Attempts to convert a memory address back to an array index.
 * 
 * @param p_array Pointer to array to check.
 * @param pp_pointer Possible indexed pointer.
 * 
 * @return Returns index on success. Returns SIZE_MAX otherwise.
 */
size_t si_parray_pointer_to_index(si_parray_t* const p_array,
	const void** const pp_pointer);

/** Doxygen
 * @brief Finds matching value/pointer's index in array at pointer.
 * 
 * @param p_array Pointer to array to be searched.
 * @param p_value Pointer to value to be compared
 * @param p_cmp_f Optional comparison function to be used.
 * 
 * @return Returns index matching p_value on success. SIZE_MAX otherwise.
 */
size_t si_parray_find(const si_parray_t* const p_array,
	const void* const p_value, int (*p_cmp_f)(const void*, const void*));

/** Doxygen
 * @brief Determines if item pointed at is within the array.
 * 
 * @param p_array Pointer to the si_parray_t struct to search.
 * @param p_value Pointer to value to be compared.
 * @param p_cmp_f Optional comparison function to be used.
 * 
 * @return Returns stdbool true if found. Returns false otherwise.
 */
bool si_parray_contains(const si_parray_t* const p_array,
	const void* const p_value, int (*p_cmp_f)(const void*, const void*));

/** Doxygen
 * @brief Fetches the value of type pointer at index in array.
 * 
 * @param p_array Pointer to array to read from.
 * @param index size_t index into the array to get from.
 * 
 * @return Returns pointer value on success. NULL otherwise.
 */
void* si_parray_at(const si_parray_t* const p_array,
	const size_t index);

/** Doxygen
 * @brief Determines if a value has been set in an array.
 * 
 * @param p_array Pointer to array to check.
 * @param index Index into array to check.
 * 
 * @return Returns stdbool true if pointer is set. Returns false otherwise.
 */
bool si_parray_is_set(const si_parray_t* const p_array, const size_t index);

/** Doxygen
 * @brief Fetches the address of the first pointer of the array.
 * 
 * @param p_array Pointer to the si_parray_t struct to read from.
 * 
 * @return Returns address pointer on success. Returns NULL otherwise.
 */
void** si_parray_first(const si_parray_t* const p_array);

/** Doxygen
 * @brief Fetches the last allocated address of the array.
 * 
 * @param p_array Pointer to the array to read from.
 * 
 * @return Returns address pointer on success. NULL otherwise.
 */
void** si_parray_last(const si_parray_t* const p_array);

/** Doxygen
 * @brief Fetches last contiguously used item address from array.
 * 
 * @param p_array Pointer to array to read from.
 * 
 * @return Returns address pointer on success. Returns NULL otherwise.
 */
void** si_parray_tail(const si_parray_t* const p_array);

/** Doxygen
 * @brief Removes item at index in array. Shifts values to maintain continuity.
 * 
 * @param p_array Pointer to array to modify
 * @param index size_t index into array to remove item at.
 * 
 * @return Returns true on removal. Returns false otherwise.
 */
bool si_parray_remove_at(si_parray_t* const p_array, const size_t index);

/** Doxygen
 * @brief Removes all items from array.
 * 
 * @param p_array Pointer to the array to clear.
 */
void si_parray_clear(si_parray_t* const p_array);

/** Doxygen
 * @brief Assigns a pointer value at index in pointer array.
 * 
 * @param p_array Pointer to the array to be modified.
 * @param index size_t offset into array to be set.
 * @param p_value Pointer value to be assigned.
 */
void si_parray_set(si_parray_t* const p_array, const size_t index,
	const void* p_value);

/** Doxygen
 * @brief Clones data into heap and assigns cloned data to index in array.
 * 
 * @param p_array Pointer to array to modify.
 * @param index size_t offset into array to be set.
 * @param p_source Pointer to memory to clone from.
 * @param source_size Size in bytes of the source.
 */
void si_parray_set_clone(si_parray_t* const p_array, const size_t index,
	const void* p_source, const size_t source_size);

/** Doxygen
 * @brief Adds pointer to array at first open spot. Grows if needed.
 * 
 * @param p_array Pointer to array to modify.
 * @param p_value Pointer value to be added to the array.
 * 
 * @return Returns index appended to on success. SIZE_MAX otherwise.
 */
size_t si_parray_append(si_parray_t* const p_array, const void* const p_value);

/** Doxygen
 * @brief Clones data into heap and appends pointer to clone to array.
 * 
 * @param p_array Pointer to the array to be modified.
 * @param p_source Pointer to memory to clone from.
 * @param source_size Size to clone in bytes.
 * 
 * @return Returns index appended to on success. SIZE_MAX otherwise.
 */
size_t si_parray_append_clone(si_parray_t* const p_array,
	const void* const p_source, const size_t source_size);

/** Doxygen
 * @brief Writes si_parray_t value(s) to FILE stream.
 * 
 * @param p_file FILE pointer to write to.
 * @param p_array Pointer to array to be written.
 * @param p_print_value Optional pointer to function to print values.
 */
void si_parray_fprint(FILE* p_file, const si_parray_t* const p_array,
	void (*p_print_value)(FILE*, void*));

/** Doxygen
 * @brief Frees values allocated inside a si_parray_t struct.
 * 
 * @param p_array Pointer to array to have contents freed.
 */
void si_parray_free(si_parray_t* const p_array);

/** Doxygen
 * @brief Frees si_parray_t being pointed at and it's contents.
 * 
 * @param pp_array Pointer to the allocated array pointer.
 */
void si_parray_destroy(si_parray_t** pp_array);

#endif//SI_PARRAY_H
