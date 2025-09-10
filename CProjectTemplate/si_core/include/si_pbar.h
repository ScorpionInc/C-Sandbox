/* si_pbar.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with ANSI Progress Bars
 * Created: 20250910
 * Updated: 20250910
//*/

#include <stdlib.h> // calloc(), free()

#include "si_terminfo.h" // si_terminfo_t
#include "si_tcolor.h" // ANSI_Color

#define SI_TUI_PBAR_DISABLE_MAX_BAR (0u)
#define SI_TUI_PBAR_DEFAULT_LEFT_BAR ('{')
#define SI_TUI_PBAR_DEFAULT_FILLED_BAR ('=')
#define SI_TUI_PBAR_DEFAULT_BAR_HEAD ('>')
#define SI_TUI_PBAR_DEFAULT_EMPTY_BAR ('-')
#define SI_TUI_PBAR_DEFAULT_RIGHT_BAR ('}')

#ifndef SI_PBAR_H
#define SI_PBAR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum si_pbar_alignment
{
	PBAR_ALIGN_UNSPECIFIED = 0,
	PBAR_ALIGN_INLINE = 1,
	PBAR_ALIGN_TOP = 2,
	PBAR_ALIGN_BOTTOM = 3,
	PBAR_ALIGN_INVALID = 4
} si_pbar_alignment;

typedef enum si_pbar_label
{
	PBAR_LABEL_UNSPECIFIED = 0,
	PBAR_LABEL_LEFT = 1,
	PBAR_LABEL_RIGHT = 2,
	PBAR_LABEL_INVALID = 3
} si_pbar_label;

typedef struct si_pbar_t
{
	const char* p_prefix;
	const char* p_suffix;
	bool enable_label;
	si_pbar_alignment alignment;
	si_pbar_label label_side;
	si_true_color_t* p_begin_color;
	si_true_color_t* p_end_color;
	si_true_color_t* p_label_color;
	uint16_t max_bar_width;
	uint8_t precision;
	char left_bar_char;
	char filled_bar_char;
	char bar_head_char;
	char empty_bar_char;
	char right_bar_char;
} si_pbar_t;

void si_pbar_init(si_pbar_t* const p_bar);

si_pbar_t* si_pbar_new();

void si_pbar_terminal_setup(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo);

void si_pbar_fprint(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo, const double percentage);

void si_pbar_terminal_restore(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SI_PBAR_H
