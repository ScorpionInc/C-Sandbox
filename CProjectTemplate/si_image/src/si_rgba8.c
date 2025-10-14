// si_rgba8.c
#include "si_rgba8.h"

void si_rgba8_init_5(si_rgba8_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue,
	const uint8_t alpha)
{
	if (NULL == p_color)
	{
		goto END;
	}
	p_color->red = red;
	p_color->green = green;
	p_color->blue = blue;
	p_color->alpha = alpha;
END:
	return;
}
inline void si_rgba8_init_4(si_rgba8_t* const p_color,
	const uint8_t red, const uint8_t green, const uint8_t blue)
{
	// Default value of alpha is SI_RGBA8_DEFAULT_ALPHA(255).
	si_rgba8_init_5(p_color, red, green, blue, SI_RGBA8_DEFAULT_ALPHA);
}
inline void si_rgba8_init(si_rgba8_t* const p_color)
{
	// Default value of color channels is SI_RGBA8_DEFAULT_COLOR(255)
	si_rgb8_init_4(
		p_color,
		SI_RGBA8_DEFAULT_COLOR, SI_RGBA8_DEFAULT_COLOR, SI_RGBA8_DEFAULT_COLOR
	);
}

void si_rgba8_from_ansi_color_3(si_rgba8_t* const p_rgba,
	const uint8_t ansi_color, const uint8_t alpha)
{
	if (NULL == p_rgba)
	{
		goto END;
	}
	si_rgb8_t rgb = {0};
	si_rgb8_from_ansi_color(&rgb, ansi_color);
	si_rgba8_from_rgb8_3(p_rgba, &rgb, alpha);
END:
	return;
}
inline void si_rgba8_from_ansi_color(si_rgba8_t* const p_rgba,
	const uint8_t ansi_color)
{
	// Default value of alpha is SI_RGBA8_DEFAULT_ALPHA(255).
	si_rgba8_from_ansi_color_3(p_rgba, ansi_color, SI_RGBA8_DEFAULT_ALPHA);
}

void si_rgba8_from_rgb8_3(si_rgba8_t* const p_rgba, const si_rgb8_t* p_rgb,
	const uint8_t alpha)
{
	if ((NULL == p_rgba) || (NULL == p_rgb))
	{
		goto END;
	}
	si_rgba8_init_5(p_rgba, p_rgb->red, p_rgb->green, p_rgb->blue, alpha);
END:
	return;
}
inline void si_rgba8_from_rgb8(si_rgba8_t* const p_rgba, const si_rgb8_t* p_rgb)
{
	// Default value of alpha is SI_RGBA8_DEFAULT_ALPHA(255).
	return si_rgba8_from_rgb8_3(p_rgba, p_rgb, SI_RGBA8_DEFAULT_ALPHA);
}

si_rgba8_t* si_rgba8_new_4(const uint8_t red, const uint8_t green,
	const uint8_t blue, const uint8_t alpha)
{
	si_rgba8_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_rgba8_t));
	if (NULL == p_result)
	{
		goto END;
	}
	si_rgba8_init_5(p_result, red, green, blue, alpha);
END:
	return p_result;
}
inline si_rgba8_t* si_rgba8_new_3(const uint8_t red, const uint8_t green,
	const uint8_t blue)
{
	// Default value of alpha is SI_RGBA8_DEFAULT_ALPHA(255).
	return si_rgba8_new_4(red, green, blue, SI_RGBA8_DEFAULT_ALPHA);
}
inline si_rgba8_t* si_rgba8_new()
{
	// Default value of color channels is SI_RGBA8_DEFAULT_COLOR(255)
	return si_rgba8_new_3(
		SI_RGBA8_DEFAULT_COLOR, SI_RGBA8_DEFAULT_COLOR, SI_RGBA8_DEFAULT_COLOR
	);
}

void si_rgba8_randomize(si_rgba8_t* const p_rgba)
{
	if (NULL == p_rgba)
	{
		goto END;
	}
	p_rgba->red   = (uint8_t)(rand() % (UINT8_MAX + 1));
	p_rgba->green = (uint8_t)(rand() % (UINT8_MAX + 1));
	p_rgba->blue  = (uint8_t)(rand() % (UINT8_MAX + 1));
	p_rgba->alpha = (uint8_t)(rand() % (UINT8_MAX + 1));
END:
	return;
}

void si_rgba8_fread(FILE* const p_file, si_rgba8_t* const p_color)
{
	//!TODO
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
END:
	return;
}
void si_rgba8_fwrite(FILE* const p_file, const si_rgba8_t* const p_color)
{
	//!TODO
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
END:
	return;
}

void si_rgba8_fdebug(FILE* const p_file, const si_rgba8_t* const p_color)
{
	if ((NULL == p_file) || (NULL == p_color))
	{
		goto END;
	}
	fprintf(
		p_file, "{R: %hhu; G: %hhu; B: %hhu; A: %hhu}",
		p_color->red, p_color->green, p_color->blue, p_color->alpha
	);
END:
	return;
}

void si_rgba8_destroy(si_rgba8_t** const pp_color)
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
