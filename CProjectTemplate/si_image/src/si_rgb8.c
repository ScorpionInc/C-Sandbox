// si_rgb8.c
#include "si_rgb8.h"

void si_rgb8_init_4(si_rgb8_t* const p_color,
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
inline void si_rgb8_init(si_rgb8_t* const p_color)
{
	// Default color value is SI_RGB8_DEFAULT_COLOR(255).
	si_rgb8_init_4(
		p_color,
		SI_RGB8_DEFAULT_COLOR, SI_RGB8_DEFAULT_COLOR, SI_RGB8_DEFAULT_COLOR
	);
}

void si_rgb8_from_ansi_color(si_rgb8_t* const p_rgb,
	const uint8_t ansi_color)
{
	if (NULL == p_rgb)
	{
		goto END;
	}
	const bool is_valid = si_ansi_color_is_valid(ansi_color);
	if (true != is_valid)
	{
		goto END;
	}
	const bool is_intense = si_ansi_color_is_intense(ansi_color);
	const si_ansi_hue_t hue = si_ansi_color_hue(ansi_color);
	const uint8_t red = si_ansi_hue_red(hue, is_intense);
	const uint8_t green = si_ansi_hue_green(hue, is_intense);
	const uint8_t blue = si_ansi_hue_blue(hue, is_intense);
	si_rgb8_init_4(p_rgb, red, green, blue);
END:
	return;
}
void si_rgb8_from_rgba8(si_rgb8_t* const p_rgb,
	const si_rgba8_t* const p_rgba)
{
	if ((NULL == p_rgb) || (NULL == p_rgba))
	{
		goto END;
	}
	// *WARNING* Alpha channel value is lost here.
	si_rgb8_init_4(p_rgb, p_rgba->red, p_rgba->green, p_rgba->blue);
END:
	return;
}

si_rgb8_t* si_rgb8_new_3(const uint8_t red, const uint8_t green,
	const uint8_t blue)
{
	si_rgb8_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_rgb8_t));
	if (NULL == p_result)
	{
		goto END;
	}
	si_rgb8_init_4(p_result, red, green, blue);
END:
	return p_result;
}
inline si_rgb8_t* si_rgb8_new()
{
	// Default color value is SI_RGB8_DEFAULT_COLOR(255)
	return si_rgb8_new_3(
		SI_RGB8_DEFAULT_COLOR, SI_RGB8_DEFAULT_COLOR, SI_RGB8_DEFAULT_COLOR
	);
}

double si_rgb8_distance(const si_rgb8_t* const p_left,
	const si_rgb8_t* const p_right)
{
	double p_result = __DBL_MAX__;
	if ((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	const double red_delta = (
		((double)p_left->red) - ((double)p_right->red)
	);
	const double green_delta = (
		((double)p_left->green) - ((double)p_right->green)
	);
	const double blue_delta = (
		((double)p_left->blue) - ((double)p_right->blue)
	);
	// Squared Euclidean distance
	p_result = (
		(red_delta   * red_delta  ) +
		(green_delta * green_delta) +
		(blue_delta  * blue_delta )
	);
END:
	return p_result;
}

void si_rgb8_randomize(si_rgb8_t* const p_rgb)
{
	if (NULL == p_rgb)
	{
		goto END;
	}
	p_rgb->red   = (uint8_t)(rand() % (UINT8_MAX + 1));
	p_rgb->green = (uint8_t)(rand() % (UINT8_MAX + 1));
	p_rgb->blue  = (uint8_t)(rand() % (UINT8_MAX + 1));
END:
	return;
}

void si_rgb8_apply_3(FILE* const p_file, const si_rgb8_t* const p_color,
	const bool foreground)
{
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
	const uint8_t truecolor_code = (
		foreground ? SI_ANSI_FR_TRUECOLOR : SI_ANSI_BK_TRUECOLOR
	);
	fprintf(
		p_file, "\033[%hhu;2;%hhu;%hhu;%hhum",
		truecolor_code,
		p_color->red, p_color->green, p_color->blue
	);
END:
	return;
}
inline void si_rgb8_apply(FILE* const p_file, const si_rgb8_t* const p_color)
{
	// Default value of foreground is SI_RGB8_DEFAULT_FOREGROUND(true)
	return si_rgb8_apply_3(p_file, p_color, SI_RGB8_DEFAULT_FOREGROUND);
}

void si_rgb8_fread(FILE* const p_file, si_rgb8_t* const p_color)
{
	//!TODO
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
END:
	return;
}
void si_rgb8_fwrite(FILE* const p_file, const si_rgb8_t* const p_color)
{
	//!TODO
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
END:
	return;
}

void si_rgb8_fdebug(FILE* const p_file, const si_rgb8_t* const p_color)
{
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
	fprintf(
		p_file, "{R: %hhu; G: %hhu; B: %hhu}",
		p_color->red, p_color->green, p_color->blue
	);
END:
	return;
}

void si_rgb8_destroy(si_rgb8_t** const pp_color)
{
	if (NULL == pp_color)
	{
		goto END;
	}
	if (NULL == *pp_color)
	{
		// Already freed
		goto END;
	}
	free(*pp_color);
	*pp_color = NULL;
END:
	return;
}
