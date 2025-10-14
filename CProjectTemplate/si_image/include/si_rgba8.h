/* si_rgba8.h - 20251006
*/

// Forward declare to prevent circular include
typedef struct si_rgb8_t si_rgb8_t;

#include <stdint.h> // uint8_t
#include <stdio.h> // FILE
#include <stdlib.h> // calloc(), free()

#define SI_RGBA8_DEFAULT_ALPHA (255)
#define SI_RGBA8_DEFAULT_COLOR (255)

#ifndef SI_RGBA8_H
#define SI_RGBA8_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef struct si_rgba8_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} si_rgba8_t;

/** Doxygen
 * @brief Initializes an existing si_rgba8_t struct to it's default values.
 * 
 * @param p_color Pointer to the struct to be initialized.
 * @param red Desired initial value of the color's red channel.
 * @param green Desired initial value of the color's green channel.
 * @param blue Desired initial value of the color's blue channel.
 * @param alpha Desired initial value of the color's alpha channel.
 */
void si_rgba8_init_5(si_rgba8_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue,
	const uint8_t alpha);
void si_rgba8_init_4(si_rgba8_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue);
void si_rgba8_init(si_rgba8_t* const p_color);

/** Doxygen
 * @brief Initializes an existing si_rgba8_t struct from ansi_color values.
 * 
 * @param p_rgba Pointer to the struct to be initialized.
 * @param ansi_color 4-bit basic ANSI color code value to use for initial value
 * @param alpha Desired initial value of the color's alpha channel.
 */
void si_rgba8_from_ansi_color_3(si_rgba8_t* const p_rgba,
	const uint8_t ansi_color, const uint8_t alpha);
void si_rgba8_from_ansi_color(si_rgba8_t* const p_rgba,
	const uint8_t ansi_color);

/** Doxygen
 * @brief Initializes an existing si_rgba8_t struct from si_rgb8_t values.
 * 
 * @param p_rgba Pointer to the struct to be initialized.
 * @param p_rgb Pointer to the si_rgb8_t struct to read values from.
 * @param alpha Desired initial value of the color's alpha channel.
 */
void si_rgba8_from_rgb8_3(si_rgba8_t* const p_rgba, const si_rgb8_t* p_rgb,
	const uint8_t alpha);
void si_rgba8_from_rgb8(si_rgba8_t* const p_rgba, const si_rgb8_t* p_rgb);

/** Doxygen
 * @brief Allocates and initializes a new heap si_rgba8_t struct.
 * 
 * @param red Desired initial value of the color's red channel.
 * @param green Desired initial value of the color's green channel.
 * @param blue Desired initial value of the color's blue channel.
 * @param alpha Desired initial value of the color's alpha channel.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_rgba8_t* si_rgba8_new_4(const uint8_t red, const uint8_t green,
	const uint8_t blue, const uint8_t alpha);
si_rgba8_t* si_rgba8_new_3(const uint8_t red, const uint8_t green,
	const uint8_t blue);
si_rgba8_t* si_rgba8_new();

/** Doxygen
 * @brief Using stdlib pseudo-random number generator rand(), randomizes color.
 * 
 * @param p_rgba Pointer to the rgba struct to have it's values randomized.
 */
void si_rgba8_randomize(si_rgba8_t* const p_rgba);

/** Doxygen
 * @brief TODO
 */
void si_rgba8_fread(FILE* const p_file, si_rgba8_t* const p_color);

/** Doxygen
 * @brief TODO
 */
void si_rgba8_fwrite(FILE* const p_file, const si_rgba8_t* const p_color);

/** Doxygen
 * @brief TODO
 */
void si_rgba8_fdebug(FILE* const p_file, const si_rgba8_t* const p_color);

/** Doxygen
 * @brief TODO
 */
void si_rgba8_destroy(si_rgba8_t** const pp_color);

#include "si_ansi_color.h"
#include "si_rgb8.h"

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_RGBA8_H