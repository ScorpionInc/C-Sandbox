/* si_term_targa.h - 20250924
 */
#include "si_targa.h"
#include "si_terminfo.h"
#include "si_tcolor.h"

#ifndef SI_TERM_TARGA_H
#define SI_TERM_TARGA_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/** Doxygen
 * @brief Draws the color values from a tga onto the terminal.
 *
 * @param p_terminfo Pointer to a terminal information struct to draw on.
 * @param p_tga Pointer to an in-memory TGA image struct.
 * @param x_pos Horizontal position on the terminal to draw image at.
 * @param y_pos Vertical position on the terminal to draw image at.
 *
 * @param Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_draw_tga_at(si_terminfo_t* const p_terminfo,
	const si_tga_t* const p_tga, const uint16_t x_pos, const uint16_t y_pos);

/** Doxygen
 * @brief Draws the color values from a tga file onto the terminal.
 *
 * @param p_terminfo Pointer to a terminal information struct to draw on.
 * @param p_path File path C string to a tga file to read colors from.
 * @param x_pos Horizontal position on the terminal to draw image at.
 * @param y_pos Vertical position on the terminal to draw image at.
 *
 * @param Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_fdraw_tga_at(si_terminfo_t* const p_terminfo,
	const char* const p_path, const uint16_t x_pos, const uint16_t y_pos);

/** Doxygen
 * @brief Draws the color values from a tga centered onto the terminal.
 *
 * @param p_terminfo Pointer to a terminal information struct to draw on.
 * @param p_tga Pointer to an in-memory TGA image struct.
 *
 * @param Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_draw_tga_centered(si_terminfo_t* const p_terminfo,
	const si_tga_t* const p_tga);

/** Doxygen
 * @brief Draws the color values from a tga file centered onto the terminal.
 *
 * @param p_terminfo Pointer to a terminal information struct to draw on.
 * @param p_path File path C string to a tga file to read colors from.
 *
 * @param Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_fdraw_tga_centered(si_terminfo_t* const p_terminfo,
	const char* const p_path);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_TERM_TARGA_H
