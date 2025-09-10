/* si_tcolor.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with Terminal colors
 * Created: 20250910
 * Updated: 20250910
//*/

#include <stdint.h> // uint8_t

#include "si_terminfo.h"

#define ANSI_FR (30)
#define ANSI_FR_TRUE (38)
#define ANSI_FR_RESET (39)

#define ANSI_BK (40)
#define ANSI_BK_TRUE (48)
#define ANSI_BK_RESET (49)

#define ANSI_TO_BK (ANSI_BK - ANSI_FR)

#define ANSI_FR_INTENSE (90)
#define ANSI_BK_INTENSE (100)

#define ANSI_INTENSE (ANSI_BK_INTENSE - ANSI_BK)

#ifndef SI_TCOLOR_H
#define SI_TCOLOR_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef enum ANSI_Color
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
} ANSI_Color;

/** Doxygen
 * @brief Determines the ANSI code for a color by enumerator and flags.
 * 
 * @param color ANSI_Color enum option to identify the desired color.
 * @param foreground Optional stdbool flag specifies fore/back ground.
 * @param intense Optional stdbool flag specified if high-intensity mode.
 * 
 * @return Returns ANSI Color code on success. Returns 0 otherwise.
 */
int si_tcolor_get_ansi_color_code_3(const ANSI_Color color,
	const bool foreground, const bool intense);
int si_tcolor_get_ansi_color_code_2(const ANSI_Color color,
	const bool foreground);
int si_tcolor_get_ansi_color_code(const ANSI_Color color);

/** Doxygen
 * @brief Sends ANSI code to FILE to apply desired color setting.
 * 
 * @param p_file FILE pointer to have the ANSI code written to.
 * @param color ANSI_Color enum option to identify the desired color.
 * @param foreground Optional stdbool flag specifies fore/back ground.
 * @param intense Optional stdbool flag specified if high-intensity mode.
 */
void si_tcolor_set_color_4(FILE* const p_file, const ANSI_Color color,
	const bool foreground, const bool intense);
void si_tcolor_set_color_3(FILE* const p_file, const ANSI_Color color,
	const bool foreground);
void si_tcolor_set_color(FILE* const p_file, const ANSI_Color color);


typedef struct si_true_color_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} si_true_color_t;

void si_true_color_init(si_true_color_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue);

void si_true_color_from_basic(si_true_color_t* const p_color,
	const ANSI_Color color, const bool intense);

ANSI_Color si_true_color_to_basic(const si_true_color_t* const p_color);

void si_true_color_mix(si_true_color_t* const p_left,
	const si_true_color_t* const p_right, const float percent);

/** Doxygen
 * @brief Sends ANSI code to FILE to apply desired true color setting.
 * 
 * @param p_color Pointer to si_true_color_t struct to identify the color.
 * @param foreground Optional stdbool flag specifies fore/back ground.
 */
void si_true_color_apply_3(FILE* const p_file,
	const si_true_color_t* const p_color, const bool foreground);
void si_true_color_apply(FILE* const p_file,
	const si_true_color_t* const p_color);


/** Doxygen
 * @brief Attempts to set the terminal color at current postion.
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 * @param p_color Pointer to a si_true_color_t struct.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_set_ansi_color_4(si_terminfo_t* const p_terminfo,
	const ANSI_Color color, const bool foreground, const bool intense);
bool si_terminfo_set_ansi_color_3(si_terminfo_t* const p_terminfo,
	const ANSI_Color color, const bool foreground);
bool si_terminfo_set_ansi_color(si_terminfo_t* const p_terminfo,
	const ANSI_Color color);

/** Doxygen
 * @brief Attempts to set the terminal color at current postion.
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 * @param p_color Pointer to a si_true_color_t struct.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_set_color_3(si_terminfo_t* const p_terminfo,
	const si_true_color_t* const p_color, const bool foreground);
bool si_terminfo_set_color(si_terminfo_t* const p_terminfo,
	const si_true_color_t* const p_color);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // SI_TCOLOR_H
