/* si_dynamic.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines two structs with functions for managing dynamically allocated buffers.
 * Created: 20150501
 * Updated: 20250611
//*/

#ifndef SI_DYNAMIC_H
#define SI_DYNAMIC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stddef.h>
#include <stdio.h>

typedef enum si_resize_mode {
	NEVER = 0,
	LINEAR,
	SCALAR,
	EXPONENTIAL,
} si_resize_mode;

#define SI_DEFAULT_RESIZE_MODE LINEAR
#define SI_DEFAULT_RESIZE_VALUE 32.0f

/** Doxygen
 * @brief Prints the string value of the si_resize_mode enum to file.
 *
 * @param p_file Pointer to file to write the string value to.
 * @param resize_mode Enum passed by value determines the string.
 */
void fprint_si_resize_mode(FILE* p_file, const si_resize_mode resize_mode);

typedef struct si_realloc_settings
{
	si_resize_mode grow_mode;
	float grow_value;
	si_resize_mode shrink_mode;
	float shrink_value;
} si_realloc_settings;

/** Doxygen
 * @brief Initializes si_realloc_settings struct pointed at by p_settings.
 * @param p_settings Pointer to si_realloc_settings struct to be initialized
 */
void init_si_realloc_settings(si_realloc_settings* p_settings);

/** Doxygen
 * @brief Finds the capacity of next grow with settings from current_capacity.
 *
 * @param p_settings Pointer to si_realloc_settings struct on how to grow.
 * @param current_capacity What capacity the buffer is already at.
 */
size_t si_realloc_next_grow_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity);

/** Doxygen
 * @brief Find the capacity of next shrink with settings from current_capacity.
 *
 * @param p_settings Pointer to si_realloc_settings struct on how to shrink.
 * @param current_capacity What capacity the buffer is already at.
 */
size_t si_realloc_next_shrink_capacity(const si_realloc_settings* p_settings,
	const size_t current_capacity);

/** Doxygen
 * @brief Writes the formatted data from si_realloc_settings struct to file.
 *
 * @param p_file Pointer to file to be written to.
 * @param p_settings Pointer to si_realloc_settings struct to be printed.
 */
void fprint_si_realloc_settings(FILE* p_file, const si_realloc_settings* p_settings);

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
void init_si_dynamic_4(si_dynamic* p_dynamic, const size_t element_size, const size_t capacity, const si_realloc_settings* settings);
void init_si_dynamic_3(si_dynamic* p_dynamic, const size_t element_size, const size_t capacity);
void init_si_dynamic(si_dynamic* p_dynamic, const size_t element_size);

/** Doxygen
 * @brief Calculates the size in bytes allocated for struct at p_dynamic
 *
 * @param p_dynamic Pointer to si_dynamic struct to size.
 * @return Returns the allocated size of the buffer in bytes.
 */
size_t si_dynamic_size(const si_dynamic* p_dynamic);

/** Doxygen
 * @brief Increases the capacity of p_buffer by amount. Ignores settings.
 *
 * @param p_dynamic Pointer to si_dynamic struct holding buffer and settings.
 * @param count How many elements to grow the buffer in the si_dynamic struct by.
 */
si_dynamic* si_dynamic_grow_by(si_dynamic* p_dynamic, const size_t count);

/** Doxygen
 * @brief Increases the capacity of p_dynamic by grow_value using grow_method.
 *
 * @param p_dynamic Pointer to allocated dynamic memory to be grown.
 * @param dynamics Pointer to si_realloc_settings struct holding grow settings.
 */
si_dynamic* si_dynamic_grow(si_dynamic* p_dynamic);

/** Doxygen
 * @brief Increases the capacity of p_buffer by amount. Ignores settings.
 *
 * @param p_dynamic Pointer to si_dynamic struct holding buffer and settings.
 * @param count How many elements to grow the buffer in the si_dynamic struct by.
 */
si_dynamic* si_dynamic_shrink_by(si_dynamic* p_dynamic, const size_t count);

/** Doxygen
 * @brief Decreases the capacity of p_dynamic by shrink_value using shrink_method.
 *
 * @param p_dynamic Pointer to allocated dynamic memory to be shrunk.
 * @param dynamics Pointer to si_dynamics struct holding shrink settings.
 */
si_dynamic* si_dynamic_shrink(si_dynamic* p_dynamic);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_dynamic Pointer to si_dynamic struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_dynamic_set(si_dynamic* p_dynamic, const size_t index, const void* p_item);

/** Doxygen
 * @brief Assigns aligned bytes inside allocated buffer to p_item bytes @ index
 *
 * @param p_dynamic Pointer to si_dynamic struct whose buffer is to be changed.
 * @param index Offset in blocks of element_size into buffer to start setting.
 * @param p_item Where to read the bytes from that will be used in buffer.
 */
void si_dynamic_get(const si_dynamic* p_dynamic, const size_t index, void* p_item);

/** Doxygen
 * @brief Frees buffer if allocated.
 */
void free_si_dynamic(si_dynamic* p_dynamic);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_DYNAMIC_H
