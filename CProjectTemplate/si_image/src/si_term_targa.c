// si_term_targa.c
#include "si_term_targa.h"

bool si_terminfo_draw_tga_at(si_terminfo_t* const p_terminfo,
    const si_tga_t* const p_tga, const uint16_t x_pos, const uint16_t y_pos)
{
	bool result = false;
	if ((NULL == p_terminfo) || (NULL == p_tga))
	{
		goto END;
	}
	if ((p_terminfo->COLUMNS <= x_pos) || (p_terminfo->ROWS <= y_pos))
	{
		goto END;
	}
	const bool is_valid = si_tga_header_is_valid(&p_tga->header);
	if (true != is_valid)
	{
		goto END;
	}
	const uint8_t channel_count = si_tga_header_channel_count(&p_tga->header);
	if (0u >= channel_count)
	{
		goto END;
	}

	// Save current cursor position (\0337)
	si_terminfo_send_ansi(p_terminfo, "\0337");
	si_terminfo_send_ansi(p_terminfo, "\033[s");

	// Draw image data
	printf("Terminal size: (%hu,%hu).\n", p_terminfo->COLUMNS, p_terminfo->ROWS);//!Debugging
	si_true_color_t terminal_color = {0};
	for (uint16_t yyy = 0u; yyy < p_tga->header.height; yyy++)
	{
		bool was_success = true;
		for (uint16_t xxx = 0u; xxx < p_tga->header.width; xxx++)
		{
			// Move cursor to next position
			const uint16_t next_x = x_pos + xxx;
			const uint16_t next_y = y_pos + yyy;
			if ((0u >= next_x) || (p_terminfo->COLUMNS <= next_x) ||
			    (0u >= next_y) || (p_terminfo->ROWS <= next_y))
			{
				// Clipping
				continue;
			}
			si_terminfo_send_ansi(
				p_terminfo, "\033[%hu;%huH", next_y, next_x
			);

			// Get color information.
			uint8_t* p_pixel = si_tga_pixel_at(p_tga, xxx, yyy);
			if (NULL == p_pixel)
			{
				was_success = false;
				break;
			}
			// TODO handle channel_count(4) channels (Alpha)
			memmove(&terminal_color, p_pixel, 3u);

			// Apply color
			was_success = si_terminfo_set_color_3(
				p_terminfo, &terminal_color, false
			);
			if (true != was_success)
			{
				break;
			}
			fprintf(p_terminfo->p_file, "%c", ' ');

			// Reset for next color
			si_terminfo_send_ansi(p_terminfo, "\033[%dm", ANSI_BK_RESET);
		} // For xxx in width
		if (true != was_success)
		{
			break;
		}
	}

	// Restore original cursor position (\0338)
	si_terminfo_send_ansi(p_terminfo, "\033[u");
	si_terminfo_send_ansi(p_terminfo, "\0338");

	result = true;
END:
	return result;
}

bool si_terminfo_fdraw_tga_at(si_terminfo_t* const p_terminfo,
    const char* const p_path, const uint16_t x_pos, const uint16_t y_pos)
{
	bool result = false;
	if ((NULL == p_terminfo) || (NULL == p_path))
	{
		goto END;
	}
	if ((p_terminfo->COLUMNS <= x_pos) || (p_terminfo->ROWS <= y_pos))
	{
		goto END;
	}
	si_tga_t tga = {0};
	const bool read_result = si_tga_fread_from(&tga, p_path);
	if (true != read_result)
	{
		goto END;
	}
	const bool draw_result = si_terminfo_draw_tga_at(
		p_terminfo, &tga, x_pos, y_pos
	);
	si_tga_free(&tga);
	if (true != draw_result)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

/** Doxygen 
 * @brief Local function determines the x coordinate to draw a centered tga at.
 * 
 * @param terminal_width Number of COLUMNS currently available in the terminal.
 * @param img_width Pixel width of the image to be drawn.
 * 
 * @return Returns x coordinate on success. Returns 1(home) otherwise.
 */
static uint16_t si_tga_x_centered(const uint16_t terminal_width,
	const uint16_t img_width)
{
	uint16_t result = 1u;
	if (img_width >= terminal_width)
	{
		goto END;
	}
	const uint16_t half_term = (terminal_width / 2);
	const uint16_t half_imag = (img_width / 2);
	result = (half_term - half_imag) + 1u;
	if (terminal_width <= result)
	{
		result = 1u;
	}
END:
	return result;
}

/** Doxygen 
 * @brief Local function determines the y coordinate to draw a centered tga at.
 * 
 * @param terminal_height Number of ROWS currently available in the terminal.
 * @param img_height Pixel height of the image to be drawn.
 * 
 * @return Returns x coordinate on success. Returns 1(home) otherwise.
 */
static uint16_t si_tga_y_centered(const uint16_t terminal_height,
	const uint16_t img_height)
{
	uint16_t result = 1u;
	if (img_height >= terminal_height)
	{
		goto END;
	}
	const uint16_t half_term = (terminal_height / 2);
	const uint16_t half_imag = (img_height / 2);
	result = (half_term - half_imag) + 1u;
	if (terminal_height <= result)
	{
		result = 1u;
	}
END:
	return result;
}

bool si_terminfo_draw_tga_centered(si_terminfo_t* const p_terminfo,
	const si_tga_t* const p_tga)
{
	bool result = false;
	
	// Fit to terminal (ignores aspect ratio)
	si_tga_t* p_clone = si_tga_clone(p_tga);
	if(NULL == p_clone)
	{
		goto END;
	}
	uint16_t fit_width = p_clone->header.width;
	uint16_t fit_height = p_clone->header.height;
	if(p_terminfo->COLUMNS <= p_clone->header.width)
	{
		fit_width = p_terminfo->COLUMNS;
	}
	if(p_terminfo->ROWS <= p_clone->header.height)
	{
		fit_height = p_terminfo->ROWS;
	}
	if((p_terminfo->COLUMNS <= p_clone->header.width) ||
	   (p_terminfo->ROWS <= p_clone->header.height))
	{
		const bool resize_result = si_tga_resize(
			p_clone, fit_width, fit_height
		);
		if(true != resize_result)
		{
			goto END;
		}
	}

	// Center img in terminal
	const uint16_t x_pos = si_tga_x_centered(
		p_terminfo->COLUMNS, p_clone->header.width
	);
	const uint16_t y_pos = si_tga_y_centered(
		p_terminfo->ROWS, p_clone->header.height
	);

	// Draws the terminal image
	result = si_terminfo_draw_tga_at(
		p_terminfo, p_clone, x_pos, y_pos
	);
END:
	return result;
}

bool si_terminfo_fdraw_tga_centered(si_terminfo_t* const p_terminfo,
	const char* const p_path)
{
	bool result = false;
	if ((NULL == p_terminfo) || (NULL == p_path))
	{
		goto END;
	}
	si_tga_t tga = {0};
	const bool read_result = si_tga_fread_from(&tga, p_path);
	if (true != read_result)
	{
		goto END;
	}
	const bool draw_result = si_terminfo_draw_tga_centered(
		p_terminfo, &tga
	);
	si_tga_free(&tga);
	if (true != draw_result)
	{
		goto END;
	}
	result = true;
END:
	return result;
}
