/* si_terminfo.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with Terminals
 * Created: 20250910
 * Updated: 20250910
//*/

#ifdef __linux__

#include <sys/ioctl.h> // winsize, ioctl()
#include <unistd.h> // isatty(), fileno()

#endif//__linux__

#include <limits.h> // INT_MAX
#include <locale.h> // setlocale(), LC_CTYPE
#include <stdarg.h> // va_list
#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint16_t
#include <stdio.h> // FILE
#include <stdlib.h> // calloc(), free(), getenv()
#include <string.h> // strnlen(), strncmp()

#include "si_strings.h" // str_countf, str_from_fprint()

// ANSI CODES (\x1b)
#define ANSI_RESET (0)
#define ANSI_BOLD (1)
#define ANSI_DIM (2)
#define ANSI_ITALIC (3)
#define ANSI_UNDERLINE (4)
#define ANSI_BLINK (5) // Also 6(?)
#define ANSI_CONTRAST (7)
#define ANSI_HIDDEN (8)
#define ANSI_STRIKE (9)

#define ANSI_DBL_UNDERLINE (21)

#define ANSI_SCROLL_REGION (33)

#ifndef SI_TERMINFO_H
#define SI_TERMINFO_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifdef __linux__

/** Doxygen
 * @brief Determines if ANSI is supported by File number on linux.
 *
 * @param file_d File number to check for ANSI support on.
 * 
 * @return Returns true if file ANSI codes are supported.
 */
bool si_tui_is_ansi_l(const int file_d);

/** Doxygen
 * @brief Determines the current terminal size and stores result on the heap.
 * 
 * @param p_size Pointer to a winsize struct to have it's values updated.
 * @param file_d File number to check for size of.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_tui_update_winsize(struct winsize* p_size, const int file_d);

#endif//__linux__


/** Doxygen
 * @brief Determines if ANSI is supported by FILE
 *
 * @param p_file FILE pointer to test for ANSI support.
 * 
 * @return Returns true if ANSI codes are supported.
 */
bool si_tui_is_ansi(FILE* const p_file);

/** Doxygen
 * @brief Attempts to determine if terminal supports ANSI 24-bit true colors.
 * 
 * @return Returns stdbool true if truecolor support found. False otherwise.
 */
bool si_tui_supports_truecolor();

/** Doxygen
 * @brief Attempts to determine if terminal supports Unicode characters.
 * 
 * @return Returns stdbool true if unicode support found. False otherwise.
 */
bool si_tui_supports_unicode();

/** Doxygen
 * @brief Sends ANSI code to FILE only if ANSI is supported by the target FILE.
 * 
 * @param p_file FILE pointer to test for ANSI support and write to.
 * @param p_code ANSI string code to send to FILE.
 */
void si_tui_send_ansi(FILE* const p_file, const char* p_code, ...);


typedef struct si_terminfo_t
{
	FILE* p_file;
	const char* p_terminal_name;
	bool supports_unicode;
	bool supports_ansi;
	bool supports_true_color;
	uint16_t ROWS;
	uint16_t COLUMNS;
} si_terminfo_t;

/** Doxygen
 * @brief Initializes an existing si_terminfo_t struct in memory by address.
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 */
void si_terminfo_init(si_terminfo_t* const p_terminfo, FILE* const p_file);

/** Doxygen
 * @brief Allocates & initializes a new si_terminfo_t struct on the heap.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_terminfo_t* si_terminfo_new(FILE* const p_file);

/** Doxygen
 * @brief Updates mutable terminal values in a si_terminfo_t struct by pointer.
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_terminfo_update(si_terminfo_t* const p_terminfo);

/** Doxygen
 * @brief Sends ANSI code to FILE only if ANSI is supported by the target FILE.
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 * @param p_code ANSI string code to send to FILE.
 */
void si_terminfo_send_ansi(si_terminfo_t* const p_terminfo,
	const char* p_code, ...);

/** Doxygen
 * @brief Prints formatted string to the terminal centered on the current line
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 * @param p_format C String formatting the desired output.
 * @param ... Variadic optional parameter values.
 * 
 * @return Positive int number of non-spacer chars printed on success else -1.
 */
int si_terminfo_printf_centered(si_terminfo_t* const p_terminfo,
	const char* const p_format, ...);

/** Doxygen
 * @brief Frees any allocated heap data inside an existing si_terminfo_t struct
 * 
 * @param p_terminfo Pointer to a si_terminfo_t struct.
 */
void si_terminfo_free(si_terminfo_t* const p_terminfo);

/** Doxygen
 * @brief Frees a heap si_terminfo_t by a heap pointer reference.
 * 
 * @param pp_terminfo Pointer to a heap pointer of a si_terminfo_t struct.
 */
void si_terminfo_destroy(si_terminfo_t** pp_terminfo);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SI_TERMINFO_H