// si_ansi_color.c
#include "si_ansi_color.h"

uint8_t si_ansi_hue_red(const si_ansi_hue_t hue, const bool intense)
{
	uint8_t red = 0u;
	const bool is_valid = SI_ANSI_HUE_IS_VALID(hue);
	if(true != is_valid)
	{
		goto END;
	}
	const uint8_t low_value = 128u;
	const uint8_t med_value = 192u;
	const uint8_t max_value = 255u;
	// Case fall through is intentional behavior
	switch (hue)
	{
	case ANSI_COLOR_BLACK:
		red = intense ? low_value : 0u;
	break;
	case ANSI_COLOR_RED:
	case ANSI_COLOR_YELLOW:
	case ANSI_COLOR_PURPLE:
		red = intense ? max_value : low_value;
	break;
	case ANSI_COLOR_WHITE:
		red = intense ? max_value : med_value;
	break;
	default:
		goto END;
	break;
	}
END:
	return red;
}
uint8_t si_ansi_hue_green(const si_ansi_hue_t hue, const bool intense)
{
	uint8_t green = 0u;
	const bool is_valid = SI_ANSI_HUE_IS_VALID(hue);
	if(true != is_valid)
	{
		goto END;
	}
	const uint8_t low_value = 128u;
	const uint8_t med_value = 192u;
	const uint8_t max_value = 255u;
	// Case fall through is intentional behavior
	switch (hue)
	{
	case ANSI_COLOR_BLACK:
		green = intense ? low_value : 0u;
	break;
	case ANSI_COLOR_GREEN:
	case ANSI_COLOR_YELLOW:
	case ANSI_COLOR_CYAN:
		green = intense ? max_value : low_value;
	break;
	case ANSI_COLOR_WHITE:
		green = intense ? max_value : med_value;
	break;
	default:
		goto END;
	break;
	}
END:
	return green;
}
uint8_t si_ansi_hue_blue(const si_ansi_hue_t hue, const bool intense)
{
	uint8_t blue = 0u;
	const bool is_valid = SI_ANSI_HUE_IS_VALID(hue);
	if(true != is_valid)
	{
		goto END;
	}
	const uint8_t low_value = 128u;
	const uint8_t med_value = 192u;
	const uint8_t max_value = 255u;
	// Case fall through is intentional behavior
	switch (hue)
	{
	case ANSI_COLOR_BLACK:
		blue = intense ? low_value : 0u;
	break;
	case ANSI_COLOR_BLUE:
	case ANSI_COLOR_PURPLE:
	case ANSI_COLOR_CYAN:
		blue = intense ? max_value : low_value;
	break;
	case ANSI_COLOR_WHITE:
		blue = intense ? max_value : med_value;
	break;
	default:
		goto END;
	break;
	}
END:
	return blue;
}


bool si_ansi_color_is_valid(const si_ansi_color_t color)
{
	bool result = false;
	const uint8_t min_valid_color = (SI_ANSI_FR + ANSI_COLOR_BLACK);
	const uint8_t max_valid_color = (SI_ANSI_BK_INTENSE + ANSI_COLOR_WHITE);
	if ((min_valid_color > color) || (max_valid_color < color))
	{
		// Out of bound check
		goto END;
	}
	if ((SI_ANSI_FR_TRUECOLOR == color) || (SI_ANSI_FR_RESET == color))
	{
		// Control codes not 4-bit colors
		goto END;
	}
	if ((SI_ANSI_BK_TRUECOLOR <= color) && (SI_ANSI_FR_INTENSE > color))
	{
		// Control codes and non-colors
		goto END;
	}
	result = true;
END:
	return result;
}
bool si_ansi_color_is_foreground(const si_ansi_color_t color)
{
	bool result = false;
	const bool is_valid = si_ansi_color_is_valid(color);
	if (true != is_valid)
	{
		goto END;
	}
	const bool is_fr = (
		(SI_ANSI_FR <= color) && (SI_ANSI_FR_TRUECOLOR > color)
	);
	const uint8_t intense_invalid = (SI_ANSI_FR_INTENSE + ANSI_COLOR_INVALID);
	const bool is_fr_intense = (
		((SI_ANSI_FR_INTENSE <= color) && (intense_invalid > color))
	);
	result = ((true == is_fr) || (true == is_fr_intense));
END:
	return result;
}
bool si_ansi_color_is_background(const si_ansi_color_t color)
{
	bool result = false;
	const bool is_valid = si_ansi_color_is_valid(color);
	if (true != is_valid)
	{
		goto END;
	}
	const bool is_bk = (
		(SI_ANSI_BK <= color) && (SI_ANSI_BK_TRUECOLOR > color)
	);
	const uint8_t intense_invalid = (SI_ANSI_BK_INTENSE + ANSI_COLOR_INVALID);
	const bool is_bk_intense = (
		((SI_ANSI_BK_INTENSE <= color) && (intense_invalid > color))
	);
	result = ((true == is_bk) || (true == is_bk_intense));
END:
	return result;
}
bool si_ansi_color_is_intense(const si_ansi_color_t color)
{
	bool result = false;
	const bool is_valid = si_ansi_color_is_valid(color);
	if (true != is_valid)
	{
		goto END;
	}
	const uint8_t fr_intense_bad = (
		SI_ANSI_FR_INTENSE + ANSI_COLOR_INVALID
	);
	const uint8_t bk_intense_bad = (
		SI_ANSI_BK_INTENSE + ANSI_COLOR_INVALID
	);
	const bool fr_intense = (
		((SI_ANSI_FR_INTENSE <= color) && (fr_intense_bad > color))
	);
	const bool bk_intense = (
		((SI_ANSI_BK_INTENSE <= color) && (bk_intense_bad > color))
	);
	result = ((true == fr_intense) || (true == bk_intense));
END:
	return result;
}

si_ansi_hue_t si_ansi_color_hue(const si_ansi_color_t color)
{
	si_ansi_hue_t result = ANSI_COLOR_INVALID;
	const bool is_valid = si_ansi_color_is_valid(color);
	if (true != is_valid)
	{
		goto END;
	}
	// Decimal is base 10.
	result = (si_ansi_hue_t)color % 10;
END:
	return result;
}

si_ansi_color_t si_ansi_color_3(const si_ansi_hue_t hue,
	const bool foreground, const bool intense)
{
	si_ansi_color_t result = 0u;
	const bool valid_hue = SI_ANSI_HUE_IS_VALID(hue);
	if (true != valid_hue)
	{
		goto END;
	}
	result = ((si_ansi_color_t)hue) + SI_ANSI_FR;
	if (true != foreground)
	{
		result += SI_ANSI_TO_BK;
	}
	if (true == intense)
	{
		result += SI_ANSI_INTENSE;
	}
END:
	return result;
}
inline si_ansi_color_t si_ansi_color_2(const si_ansi_hue_t hue,
	const bool foreground)
{
	// Default value of intense is SI_ANSI_COLOR_DEFAULT_INTENSE(false)
	return si_ansi_color_3(hue, foreground, SI_ANSI_COLOR_DEFAULT_INTENSE);
}
inline si_ansi_color_t si_ansi_color(const si_ansi_hue_t hue)
{
	// Default value of foreground is SI_ANSI_COLOR_DEFAULT_FOREGROUND(true)
	return si_ansi_color_2(hue, SI_ANSI_COLOR_DEFAULT_FOREGROUND);
}

si_ansi_color_t si_ansi_color_from_rgb8_2(const si_rgb8_t* const p_rgb,
	const bool foreground)
{
	si_ansi_color_t result = 0u;
	if (NULL == p_rgb)
	{
		goto END;
	}
	// Manual scoping for use of VLA with goto.
	{
		const size_t palette_size = (SI_ANSI_HUE_COUNT * 2u);
		si_rgb8_t p_palette[palette_size];
		memset(p_palette, 0x00, palette_size * sizeof(si_rgb8_t));

		// Generate palette contents
		for (size_t iii = 0u; iii < palette_size; iii++)
		{
			const bool gen_intense = (SI_ANSI_HUE_COUNT <= iii);
			const si_ansi_hue_t next_hue = iii % SI_ANSI_HUE_COUNT;
			si_rgb8_t* const p_next_rgb = &(p_palette[iii]);
			p_next_rgb->red = si_ansi_hue_red(next_hue, gen_intense);
			p_next_rgb->green = si_ansi_hue_green(next_hue, gen_intense);
			p_next_rgb->blue = si_ansi_hue_blue(next_hue, gen_intense);
		}

		// Find closest color in palette
		size_t best_index = SIZE_MAX;
		double min_distance = __DBL_MAX__;
		for(size_t iii = 0u; iii < palette_size; iii++)
		{
			si_rgb8_t* const p_next_rgb = &(p_palette[iii]);
			double next_distance = si_rgb8_distance(p_rgb, p_next_rgb);
			if (next_distance < min_distance)
			{
				min_distance = next_distance;
				best_index = iii;
			}
		}

		// Convert result to ANSI color.
		const si_ansi_hue_t result_hue = best_index % SI_ANSI_HUE_COUNT;
		const bool result_intense = (SI_ANSI_HUE_COUNT <= best_index);
		result = si_ansi_color_3(result_hue, foreground, result_intense);
	}
END:
	return result;
}
inline si_ansi_color_t si_ansi_color_from_rgb8(const si_rgb8_t* const p_rgb)
{
	// Default value of foreground is SI_ANSI_COLOR_DEFAULT_FOREGROUND(true)
	return si_ansi_color_from_rgb8_2(p_rgb, SI_ANSI_COLOR_DEFAULT_FOREGROUND);
}

si_ansi_color_t si_ansi_color_from_rgba8_2(const si_rgba8_t* const p_rgba,
	const bool foreground)
{
	si_ansi_color_t result = 0u;
	if (NULL == p_rgba)
	{
		goto END;
	}
	// *WARNING* Alpha channel color value is lost/ignored.
	si_rgb8_t rgb = {0};
	si_rgb8_from_rgba8(&rgb, p_rgba);
	result = si_ansi_color_from_rgb8_2(&rgb, foreground);
END:
	return result;
}
inline si_ansi_color_t si_ansi_color_from_rgba8(const si_rgba8_t* const p_rgba)
{
	// Default value of foreground is SI_ANSI_COLOR_DEFAULT_FOREGROUND(true)
	return si_ansi_color_from_rgba8_2(
		p_rgba, SI_ANSI_COLOR_DEFAULT_FOREGROUND
	);
}

void si_ansi_color_apply(FILE* const p_file, const si_ansi_color_t color)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const bool is_valid = si_ansi_color_is_valid(color);
	if (true != is_valid)
	{
		goto END;
	}
	fprintf(p_file, "\033[%hhum", color);
END:
	return;
}

/** Doxygen
 * @brief Local function for mapping an ansi_hue enum value to a const string.
 * 
 * @param hue Hue value to get string of.
 * 
 * @return Returns C string value.
 */
static char* si_ansi_hue_string(si_ansi_hue_t hue)
{
	const char* p_result = "INVALID";
	const bool valid_hue = SI_ANSI_HUE_IS_VALID(hue);
	if (true != valid_hue)
	{
		goto END;
	}
	switch (hue)
	{
	case ANSI_COLOR_BLACK:
		p_result = "BLACK";
	break;
	case ANSI_COLOR_RED:
		p_result = "RED";
	break;
	case ANSI_COLOR_GREEN:
		p_result = "GREEN";
	break;
	case ANSI_COLOR_YELLOW:
		p_result = "YELLOW";
	break;
	case ANSI_COLOR_BLUE:
		p_result = "BLUE";
	break;
	case ANSI_COLOR_PURPLE:
		p_result = "PURPLE";
	break;
	case ANSI_COLOR_CYAN:
		p_result = "CYAN";
	break;
	case ANSI_COLOR_WHITE:
		p_result = "WHITE";
	break;
	default:
		goto END;
	break;
	}
END:
	return (char*)p_result;
}

void si_ansi_color_fdebug(FILE* const p_file, const si_ansi_color_t color)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const bool is_valid = si_ansi_color_is_valid(color);
	const si_ansi_hue_t hue = si_ansi_color_hue(color);
	if ((ANSI_COLOR_INVALID == hue) || (true != is_valid))
	{
		fprintf(p_file, "INVALID");
		goto END;
	}
	const char* p_hue_str = si_ansi_hue_string(hue);
	const bool is_intense = si_ansi_color_is_intense(color);
	if (true == is_intense)
	{
		fprintf(p_file, "INTENSE_");
	}
	fprintf(p_file, "%s_", p_hue_str);
	const bool is_background = si_ansi_color_is_background(color);
	if (true == is_background)
	{
		fprintf(p_file, "BACK");
	}
	else
	{
		fprintf(p_file, "FORE");
	}
	fprintf(p_file, "GROUND");
END:
	return;
}
