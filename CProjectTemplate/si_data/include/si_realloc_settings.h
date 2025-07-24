/* si_realloc_settings.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct si_realloc_settings along with several functions.
 *          These allow configuring dynamic memory grow/shrink methods/amounts.
 * Created: 20150611
 * Updated: 20250611
//*/

#include <math.h> //pow
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "si_dynamic.h"

#ifndef SI_REALLOC_SETTINGS
#define SI_REALLOC_SETTINGS

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


typedef enum si_resize_mode
{
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
void si_resize_mode_fprint(FILE* p_file, const si_resize_mode resize_mode);


typedef struct si_realloc_settings
{
	float grow_value;
	float shrink_value;
	si_resize_mode grow_mode;
	si_resize_mode shrink_mode;
} si_realloc_settings;

/** Doxygen
 * @brief Initializes si_realloc_settings struct pointed at by p_settings.
 * @param p_settings Pointer to si_realloc_settings struct to be initialized
 */
void si_realloc_settings_new(si_realloc_settings* p_settings);

/** Doxygen
 * @brief Finds the capacity of next grow with settings from current_capacity.
 *
 * @param p_settings Pointer to si_realloc_settings struct on how to grow.
 * @param current_capacity What capacity the buffer is already at.
 */
size_t si_realloc_settings_next_grow_capacity(
	const si_realloc_settings* p_settings, const size_t current_capacity);

/** Doxygen
 * @brief Find the capacity of next shrink with settings from current_capacity.
 *
 * @param p_settings Pointer to si_realloc_settings struct on how to shrink.
 * @param current_capacity What capacity the buffer is already at.
 */
size_t si_realloc_settings_next_shrink_capacity(
	const si_realloc_settings* p_settings, const size_t current_capacity);

/** Doxygen
 * @brief Increases the capacity of p_dynamic by grow_value using grow_method.
 *
 * @param p_dynamic Pointer to struct of allocated dynamic memory to be grown.
 * @param dynamics Pointer to si_realloc_settings struct holding grow settings.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_realloc_settings_grow(
	const si_realloc_settings* p_settings, si_dynamic_t* p_dynamic);

/** Doxygen
 * @brief Lowers the capacity of p_dynamic by shrink_value using shrink_method.
 *
 * @param p_dynamic Pointer to struct of allocated dynamic memory to be shrunk.
 * @param dynamics Pointer to si_dynamics struct holding shrink settings.
 *
 * @return Returns true on success. False otherwise.
 */
bool si_realloc_settings_shrink(
	const si_realloc_settings* p_settings, si_dynamic_t* p_dynamic);

/** Doxygen
 * @brief Writes the formatted data from si_realloc_settings struct to file.
 *
 * @param p_file Pointer to file to be written to.
 * @param p_settings Pointer to si_realloc_settings struct to be printed.
 */
void si_realloc_settings_fprint(FILE* p_file,
	const si_realloc_settings* p_settings);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_REALLOC_SETTINGS
