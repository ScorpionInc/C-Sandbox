// si_tcolor.c
#include "si_tcolor.h"

int si_tcolor_get_ansi_color_code_3(const ANSI_Color color,
	const bool foreground, const bool intense)
{
	int result = -1;
	if ((0 > color) || (ANSI_COLOR_INVALID <= color))
	{
		goto END;
	}
	result = (true == foreground) ? ANSI_FR : ANSI_BK;
	if (true == intense)
	{
		result += ANSI_INTENSE;
	}
	result += color;
END:
	return result;
}
inline int si_tcolor_get_ansi_color_code_2(const ANSI_Color color,
	const bool foreground)
{
	// Default value of intense is false.
	return si_tcolor_get_ansi_color_code_3(color, foreground, false);
}
inline int si_tcolor_get_ansi_color_code(const ANSI_Color color)
{
	// Default value of foreground is true.
	return si_tcolor_get_ansi_color_code_2(color, true);
}

void si_tcolor_set_color_4(FILE* const p_file, const ANSI_Color color,
	const bool foreground, const bool intense)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const int ansi_code = si_tcolor_get_ansi_color_code_3(
		color, foreground, intense
	);
	if (0 > ansi_code)
	{
		goto END;
	}
	si_tui_send_ansi(p_file, "\e[%dm", ansi_code);
END:
	return;
}
inline void si_tcolor_set_color_3(FILE* const p_file, const ANSI_Color color,
	const bool foreground)
{
	// Default value of intense is false.
	si_tcolor_set_color_4(p_file, color, foreground, false);
}
inline void si_tcolor_set_color(FILE* const p_file, const ANSI_Color color)
{
	// Default value of foreground is true.
	si_tcolor_set_color_3(p_file, color, true);
}

void si_true_color_init(si_true_color_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue)
{
	if (NULL == p_color)
	{
		goto END;
	}
	p_color->red = red;
	p_color->green = green;
	p_color->blue = blue;
END:
	return;
}

void si_true_color_from_basic(si_true_color_t* const p_color,
	const ANSI_Color color, const bool intense)
{
	// Mapping to eclipse palette from:
	// https://cli.r-lib.org/reference/ansi_palettes.html
	if (NULL == p_color)
	{
		goto END;
	}
	const uint8_t off_value = 0x00;
	const uint8_t low_value = 0xcd;
	const uint8_t mid_value = 0xe5;
	const uint8_t hig_value = 0xff;
	// Invalid color case intentionally falls through.
	switch (color)
	{
	case ANSI_COLOR_BLACK:
		si_true_color_init(p_color, off_value, off_value, off_value);
		break;
	case ANSI_COLOR_BLUE:
		si_true_color_init(
			p_color,
			intense ? low_value : off_value,
			intense ? low_value : off_value,
			intense ? hig_value : mid_value
		);
		break;
	case ANSI_COLOR_CYAN:
		si_true_color_init(
			p_color,
			off_value,
			intense ? hig_value : low_value,
			intense ? hig_value : low_value
		);
		break;
	case ANSI_COLOR_GREEN:
		si_true_color_init(
			p_color,
			off_value,
			intense ? hig_value : low_value,
			off_value
		);
		break;
	case ANSI_COLOR_PURPLE:
		si_true_color_init(
			p_color,
			intense ? hig_value : low_value,
			off_value,
			intense ? hig_value : low_value
		);
		break;
	case ANSI_COLOR_RED:
		si_true_color_init(
			p_color,
			intense ? hig_value : low_value,
			off_value,
			off_value
		);
		break;
	case ANSI_COLOR_WHITE:
		si_true_color_init(
			p_color,
			intense ? hig_value : mid_value,
			intense ? hig_value : mid_value,
			intense ? hig_value : mid_value
		);
		break;
	case ANSI_COLOR_YELLOW:
		si_true_color_init(
			p_color,
			intense ? hig_value : low_value,
			intense ? hig_value : low_value,
			off_value
		);
		break;
	case ANSI_COLOR_INVALID:
	default:
		break;
	}
END:
	return;
}

/** Doxygen
 * @brief Local function to determine if the true color would be considered
 *        intense or not for its equivilent basic color value.
 * 
 * @param p_color Pointer to a si_true_color_t struct.
 * 
 * @return Returns stdbool true if intense. Returns false otherwise.
 */
static bool si_true_color_is_intense(const si_true_color_t* const p_color)
{
	bool result = false;
	if (NULL == p_color)
	{
		goto END;
	}

	// Uses the Luma or Y value
	const int luma = ((p_color->red   << 1) + p_color->red +
	                  (p_color->green << 2) + p_color->blue) >> 3;
	const int intense_threshold = (UINT8_MAX / 2);
	result = (intense_threshold <= luma);
END:
	return result;
}

/** Doxygen
 * @brief Returns the brightest/largest color channel value in si_true_color_t.
 * 
 * @param p_color Pointer to si_true_color_t struct
 * 
 * @return Returns largest color channel value in the si_true_color_t struct.
 */
static uint8_t si_true_color_channel_max(const si_true_color_t* const p_color)
{
	uint8_t result = 0u;
	if (NULL == p_color)
	{
		goto END;
	}
	result = p_color->red;
	if (result < p_color->green)
	{
		result = p_color->green;
	}
	if (result < p_color->blue)
	{
		result = p_color->blue;
	}
END:
	return result;
}

ANSI_Color si_true_color_to_basic(const si_true_color_t* const p_color)
{
	ANSI_Color result = ANSI_COLOR_INVALID;
	if (NULL == p_color)
	{
		goto END;
	}
	
	const uint8_t max_channel = si_true_color_channel_max(p_color);
	const bool mostly_red = (max_channel == p_color->red);
	const bool mostly_green = (max_channel == p_color->green);
	const bool mostly_blue = (max_channel == p_color->blue);

	if (true == mostly_red)
	{
		if (true == mostly_green)
		{
			result = mostly_blue ? ANSI_COLOR_WHITE : ANSI_COLOR_YELLOW;
			goto LUMA;
		}
		if (true == mostly_blue)
		{
			result = ANSI_COLOR_PURPLE;
			goto END;
		}
		result = ANSI_COLOR_RED;
		goto END;
	}
	if (true == mostly_green)
	{
		if (true == mostly_blue)
		{
			result = ANSI_COLOR_CYAN;
			goto END;
		}
		result = ANSI_COLOR_GREEN;
		goto END;
	}
	result = ANSI_COLOR_BLUE;
	goto END;

	// Handle greyscale
LUMA:
	const bool is_intense = si_true_color_is_intense(p_color);
	if ((ANSI_COLOR_WHITE == result) && (true != is_intense))
	{
		result = ANSI_COLOR_BLACK;
	}
END:
	return result;
}

/** Doxygen
 * @brief Returns the float clamped to be within a inclusive range.
 * 
 * @param input Float value to be clamped.
 * @param lower Lowest/Smallest valid float value.
 * @param upper Highest/Largest valid float value.
 * 
 * @return Returns the float clamped inclusivly inside a range.
 */
static float clamp_f(const float input,
	const float lower, const float upper)
{
	float result = input;
	if (lower > result)
	{
		result = lower;
	}
	if (upper < result)
	{
		result = upper;
	}
	return result;
}

/** Doxygen
 * @brief Returns an int clamped to be within a inclusive range.
 * 
 * @param input Integer value to be clamped.
 * @param lower Lowest/Smallest valid int value.
 * @param upper Highest/Largest valid int value.
 * 
 * @return Returns an int clamped inclusivly inside a range.
 */
static int clamp_i(const int input,
	const int lower, const int upper)
{
	int result = input;
	if (lower > result)
	{
		result = lower;
	}
	if (upper < result)
	{
		result = upper;
	}
	return result;
}

void si_true_color_mix(si_true_color_t* const p_left,
	const si_true_color_t* const p_right, const float percent)
{
	if ((NULL == p_left) || (NULL == p_right) || (0.0f >= percent))
	{
		goto END;
	}
	const float red_delta = (((float)p_right->red) - ((float)p_left->red));
	const float green_delta = (
		((float)p_right->green) - ((float)p_left->green)
	);
	const float blue_delta = (((float)p_right->blue) - ((float)p_left->blue));

	const float clamped_percent = clamp_f(percent, 0.0, 1.0);
	const float red_scaled = (red_delta * clamped_percent);
	const float green_scaled = (green_delta * clamped_percent);
	const float blue_scaled = (blue_delta * clamped_percent);

	// Truncation here is intentionally used for rounding down
	const int red_rounded = ((int)red_scaled);
	const int green_rounded = ((int)green_scaled);
	const int blue_rounded = ((int)blue_scaled);

	const int red_new = clamp_i(
		((int)p_left->red) + red_rounded, 0, UINT8_MAX
	);
	const int green_new = clamp_i(
		((int)p_left->green) + green_rounded, 0, UINT8_MAX
	);
	const int blue_new = clamp_i(
		((int)p_left->blue) + blue_rounded, 0, UINT8_MAX
	);

	p_left->red = (uint8_t)red_new;
	p_left->green = (uint8_t)green_new;
	p_left->blue = (uint8_t)blue_new;
END:
	return;
}

void si_true_color_apply_3(FILE* const p_file,
	const si_true_color_t* const p_color, const bool foreground)
{
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
	const bool is_supported = si_tui_supports_truecolor();
	if (true != is_supported)
	{
		// Fall back on basic ANSI color support
		ANSI_Color a_color = si_true_color_to_basic(p_color);
		const bool is_intense = si_true_color_is_intense(p_color);
		si_tcolor_set_color_4(p_file, a_color, foreground, is_intense);
		goto END;
	}
	const int op_code = foreground ? ANSI_FR_TRUE : ANSI_BK_TRUE;
	si_tui_send_ansi(
		p_file, "\e[%d;2;%d;%d;%dm", op_code,
		p_color->red, p_color->green, p_color->blue
	);
END:
	return;
}
inline void si_true_color_apply(FILE* const p_file,
	const si_true_color_t* const p_color)
{
	// Default value of foreground is true.
	si_true_color_apply(p_file, p_color);
}


bool si_terminfo_set_ansi_color_4(si_terminfo_t* const p_terminfo,
	const ANSI_Color color, const bool foreground, const bool intense)
{
	bool result = false;
	if ((NULL == p_terminfo) || (ANSI_COLOR_INVALID <= color))
	{
		goto END;
	}
	const int ansi_code = si_tcolor_get_ansi_color_code_3(
		color, foreground, intense
	);
	if (0 > ansi_code)
	{
		goto END;
	}
	si_terminfo_send_ansi(p_terminfo, "\e[%dm", ansi_code);
END:
	return result;
}
inline bool si_terminfo_set_ansi_color_3(si_terminfo_t* const p_terminfo,
	const ANSI_Color color, const bool foreground)
{
	// Default value of intense is false.
	return si_terminfo_set_ansi_color_4(p_terminfo, color, foreground, false);
}
inline bool si_terminfo_set_ansi_color(si_terminfo_t* const p_terminfo,
	const ANSI_Color color)
{
	// Default value of foreground is true.
	return si_terminfo_set_ansi_color_3(p_terminfo, color, true);
}

bool si_terminfo_set_color_3(si_terminfo_t* const p_terminfo,
	const si_true_color_t* const p_color, const bool foreground)
{
	bool result = false;
	if ((NULL == p_terminfo) || (NULL == p_color))
	{
		goto END;
	}
	if (NULL == p_terminfo->p_file)
	{
		goto END;
	}
	const bool is_supported = p_terminfo->supports_true_color;
	if (true != is_supported)
	{
		// Falls back on basic ANSI Color support
		ANSI_Color a_color = si_true_color_to_basic(p_color);
		const bool is_intense = si_true_color_is_intense(p_color);
		result = si_terminfo_set_ansi_color_4(
			p_terminfo, a_color, foreground, is_intense
		);
		goto END;
	}
	const int op_code = foreground ? ANSI_FR_TRUE : ANSI_BK_TRUE;
	si_terminfo_send_ansi(
		p_terminfo, "\e[%d;2;%d;%d;%dm", op_code,
		p_color->red, p_color->green, p_color->blue
	);
	result = true;
END:
	return result;
}
inline bool si_terminfo_set_color(si_terminfo_t* const p_terminfo,
	const si_true_color_t* const p_color)
{
	// Default value of foreground is true.
	return si_terminfo_set_color_3(p_terminfo, p_color, true);
}
