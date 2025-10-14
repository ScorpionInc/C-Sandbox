/* si_rgb8.h - 20251006
*/

// Forward declare to prevent circular include
typedef struct si_rgba8_t si_rgba8_t;

#include <limits.h> // __DBL_MAX__, UINT8_MAX
#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint8_t
#include <stdio.h> // FILE
#include <stdlib.h> // calloc(), free()

#define SI_RGB8_DEFAULT_COLOR (255)
#define SI_RGB8_DEFAULT_FOREGROUND (true)

#ifndef SI_RGB8_H
#define SI_RGB8_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef struct si_rgb8_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} si_rgb8_t;

void si_rgb8_init_4(si_rgb8_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue);
void si_rgb8_init(si_rgb8_t* const p_color);

/** Doxygen
 * @brief Sets an RGB8 struct to a mapped ANSI 4-bit color.
 * 
 * @param p_rgb Pointer to the RGB8 struct to be set.
 * @param ansi_color 4-bit ANSI color value to be mapped.
 */
void si_rgb8_from_ansi_color(si_rgb8_t* const p_rgb,
	const uint8_t ansi_color);
/** Doxygen
 * @brief Sets an RGB8 struct to RGBA8 struct value loses alpha channel value.
 * 
 * @param p_rgb Pointer to the RGB8 struct to be set.
 * @param p_rgba Pointer to the RGBA8 struct to be read from.
 */
void si_rgb8_from_rgba8(si_rgb8_t* const p_rgb,
	const si_rgba8_t* const p_rgba);

/** Doxygen
 * @brief Allocates and initializes a new heap rgb8 struct.
 * 
 * @param red Initial value of the red channel.
 * @param green Initial value of the green channel.
 * @param blue Initial value of the blue channel.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_rgb8_t* si_rgb8_new_3(const uint8_t red, const uint8_t green,
	const uint8_t blue);
si_rgb8_t* si_rgb8_new();

/** Doxygen
 * @brief Determines the distance between two rgb8 colors.
 * 
 * @param p_left  Pointer to the first  RGB8 color to compare.
 * @param p_right Pointer to the second RGB8 color to compare.
 * 
 * @return Return double distance value on success. __DBL_MAX__ on error.
 */
double si_rgb8_distance(const si_rgb8_t* const p_left,
	const si_rgb8_t* const p_right);

/** Doxygen
 * @brief Using stdlib pseudo-random number generator rand(), randomizes color.
 * 
 * @param p_rgb Pointer to the rgb struct to have it's values randomized.
 */
void si_rgb8_randomize(si_rgb8_t* const p_rgb);

/** Doxygen
 * @brief Writes Truecolor ANSI escape code to FILE to apply color.
 * 
 * @details *WARNING* Doesn't check for ANSI or Truecolor support.
 *          *WARNING* Doesn't lock the FILE for writing.
 * 
 * @param p_file Pointer to the FILE to be written to.
 * @param p_color Pointer to the RGB8 struct to read desired color from.
 * @param foreground Flag stdbool determines if applied to fore/background.
 */
void si_rgb8_apply_3(FILE* const p_file, const si_rgb8_t* const p_color,
	const bool foreground);
void si_rgb8_apply(FILE* const p_file, const si_rgb8_t* const p_color);

/** Doxygen
 * @brief Reads the sizeof si_rgb8_t bytes from file in RGB order.
 * 
 * @param p_file Pointer to the FILE to read from.
 * @param p_color RGB struct to hold the read byte values in.
 */
void si_rgb8_fread(FILE* const p_file, si_rgb8_t* const p_color);
/** Doxygen
 * @brief Writes the si_rgb8_t bytes into a file in RGB order.
 * 
 * @param p_file Pointer to the FILE to write to.
 * @param p_color RGB struct to read byte values from.
 */
void si_rgb8_fwrite(FILE* const p_file, const si_rgb8_t* const p_color);

/** Doxygen
 * @brief Writes the values of a si_rgb8_t into a file in human-readable format
 * 
 * @param p_file Pointer to the FILE to write to.
 * @param p_color RGB struct to read color values from.
 */
void si_rgb8_fdebug(FILE* const p_file, const si_rgb8_t* const p_color);

/** Doxygen
 * @brief Frees a heap allocates si_rgb8_t struct and NULL's its pointer.
 * 
 * @param pp_color Pointer to a si_rgb8_t heap pointer to be freed.
 */
void si_rgb8_destroy(si_rgb8_t** const pp_color);

#include "si_ansi_color.h"
#include "si_rgba8.h"

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_RGB8_h
