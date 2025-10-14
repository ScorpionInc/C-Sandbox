/* si_ansi_color.h - 20251006
*/

// Forward declare to prevent circular include
typedef struct si_rgb8_t si_rgb8_t;
typedef struct si_rgba8_t si_rgba8_t;

#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint8_t
#include <stdio.h> // FILE
#include <string.h> // memset()

#define SI_ANSI_FR (30)
#define SI_ANSI_FR_TRUECOLOR (38)
#define SI_ANSI_FR_RESET (39)

#define SI_ANSI_BK (40)
#define SI_ANSI_BK_TRUECOLOR (48)
#define SI_ANSI_BK_RESET (49)

#define SI_ANSI_TO_BK (SI_ANSI_BK - SI_ANSI_FR)

#define SI_ANSI_FR_INTENSE (90)
#define SI_ANSI_BK_INTENSE (100)

#define SI_ANSI_INTENSE (SI_ANSI_BK_INTENSE - SI_ANSI_BK)

#define SI_ANSI_COLOR_DEFAULT_FOREGROUND (true)
#define SI_ANSI_COLOR_DEFAULT_INTENSE (false)

#ifndef SI_ANSI_COLOR_H
#define SI_ANSI_COLOR_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef enum si_ansi_hue_t
{
	ANSI_COLOR_BLACK = 0,
	ANSI_COLOR_RED = 1,
	ANSI_COLOR_GREEN = 2,
	ANSI_COLOR_YELLOW = 3,
	ANSI_COLOR_BLUE = 4,
	ANSI_COLOR_PURPLE = 5,
	ANSI_COLOR_CYAN = 6,
	ANSI_COLOR_WHITE = 7,
	ANSI_COLOR_INVALID = 8 // 9 is reset
} si_ansi_hue_t;

#define SI_ANSI_HUE_COUNT (8)
#define SI_ANSI_HUE_IS_VALID(hue) \
	((ANSI_COLOR_BLACK <= hue) && (ANSI_COLOR_INVALID > hue))

uint8_t si_ansi_hue_red(  const si_ansi_hue_t hue, const bool intense);
uint8_t si_ansi_hue_green(const si_ansi_hue_t hue, const bool intense);
uint8_t si_ansi_hue_blue( const si_ansi_hue_t hue, const bool intense);


// Basic ANSI is a 4-bit color pallette
#define si_ansi_color_t uint8_t

/** Doxygen
 * @brief Determine is the si_ansi_color_t is a valid 4-bit color value.
 * 
 * @param color The si_ansi_color_t to be checked.
 * 
 * @return Returns stdbool true if valid. Returns false otherwise.
 */
bool si_ansi_color_is_valid(const si_ansi_color_t color);
/** Doxygen
 * @brief Determine is the si_ansi_color_t affects the foreground color.
 * 
 * @param color The si_ansi_color_t to be checked.
 * 
 * @return Returns stdbool true if foreground. Returns false otherwise.
 */
bool si_ansi_color_is_foreground(const si_ansi_color_t color);
/** Doxygen
 * @brief Determine is the si_ansi_color_t affects the background color.
 * 
 * @param color The si_ansi_color_t to be checked.
 * 
 * @return Returns stdbool true if background. Returns false otherwise.
 */
bool si_ansi_color_is_background(const si_ansi_color_t color);
/** Doxygen
 * @brief Determine is the si_ansi_color_t is an intense color.
 * 
 * @param color The si_ansi_color_t to be checked.
 * 
 * @return Returns stdbool true if intense. Returns false otherwise.
 */
bool si_ansi_color_is_intense(const si_ansi_color_t color);

/** Doxygen
 * @brief Determine the chroma(hue) value of the si_ansi_color_t.
 * 
 * @param color The si_ansi_color_t to get the si_ansi_hue_t value from.
 * 
 * @return Returns a si_ansi_hue_t enum value.
 */
si_ansi_hue_t si_ansi_color_hue(const si_ansi_color_t color);

/** Doxygen
 * @brief Determines the ANSI code for a color by enumerator and flags.
 * 
 * @param color si_ansi_color_t option to identify the desired color.
 * @param foreground Optional stdbool flag specifies fore/back ground.
 * @param intense Optional stdbool flag specified if high-intensity mode.
 * 
 * @return Returns ANSI Color code on success. Returns 0 otherwise.
 */
si_ansi_color_t si_ansi_color_3(const si_ansi_hue_t hue,
	const bool foreground, const bool intense);
si_ansi_color_t si_ansi_color_2(const si_ansi_hue_t hue,
	const bool foreground);
si_ansi_color_t si_ansi_color(const si_ansi_hue_t hue);

/** Doxygen
 * @brief Converts and RGB8 color value to nearest ANSI 4-bit color.
 * 
 * @param p_rgb Pointer to the RGB8 color to be converted.
 * @param foreground Flag stdbool determines if color is fore/background.
 * 
 * @return Returns foreground si_ansi_color_t value of p_rgb.
 */
si_ansi_color_t si_ansi_color_from_rgb8_2(const si_rgb8_t* const p_rgb,
	const bool foreground);
si_ansi_color_t si_ansi_color_from_rgb8(const si_rgb8_t* const p_rgb);
/** Doxygen
 * @brief Converts and RGBA8 color value to nearest ANSI 4-bit color.
 * 
 * @param p_rgba Pointer to the RGBA8 color to be converted.
 * @param foreground Flag stdbool determines if color is fore/background.
 * 
 * @return Returns foreground si_ansi_color_t value of p_rgba.
 */
si_ansi_color_t si_ansi_color_from_rgba8_2(const si_rgba8_t* const p_rgba,
	const bool foreground);
si_ansi_color_t si_ansi_color_from_rgba8(const si_rgba8_t* const p_rgba);

/** Doxygen
 * @brief Sends ANSI code to FILE to apply desired ANSI color setting to.
 * 
 * @details WARNING Doesn't test FILE for ANSI support before sending the
 *          ANSI code to the file. Will write to valid FILE regardless.
 *          WARNING Doesn't lock FILE for writing.
 * 
 * @param p_file FILE pointer to have the ANSI code written to.
 * @param color si_ansi_color_t of the desired color.
 */
void si_ansi_color_apply(FILE* const p_file, const si_ansi_color_t color);

/** Doxygen
 * @brief Writes the si_ansi_color_t value to FILE in a human-readable way.
 * 
 * @details WARNING Doesn't lock FILE for writing.
 * 
 * @param p_file FILE pointer to be written to.
 * @param color si_ansi_color_t of the color.
 */
void si_ansi_color_fdebug(FILE* const p_file, const si_ansi_color_t color);

#include "si_rgb8.h" // si_rgb8_t
#include "si_rgba8.h" // si_rgba8_t

#ifdef __cplusplus
}
#endif //__cplusplus

#endif// SI_ANSI_COLOR_H