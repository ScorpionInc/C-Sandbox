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

	// Save current cursor position (\e7)
	si_terminfo_send_ansi(p_terminfo, "\e7");
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
				p_terminfo, "\e[%hu;%huH", next_y, next_x
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
			si_terminfo_send_ansi(p_terminfo, "\e[%dm", ANSI_BK_RESET);
		} // For xxx in width
		if (true != was_success)
		{
			break;
		}
	}

	// Restore original cursor position (\e8)
	si_terminfo_send_ansi(p_terminfo, "\033[u");
	si_terminfo_send_ansi(p_terminfo, "\e8");

	result = true;
END:
	return result;
}

bool si_terminfo_fdraw_tga_at(si_terminfo_t* const p_terminfo,
    const char* const p_path, const uint16_t x_pos, const uint16_t y_pos)
{
	bool result = false;
	if((NULL == p_terminfo) || (NULL == p_path))
	{
		goto END;
	}
	if((p_terminfo->COLUMNS <= x_pos) || (p_terminfo->ROWS <= y_pos))
	{
		goto END;
	}
	si_tga_t tga = {0};
	const bool read_result = si_tga_fread_from(&tga, p_path);
	if(true != read_result)
	{
		goto END;
	}
	const bool draw_result = si_terminfo_draw_tga_at(
		p_terminfo, &tga, x_pos, y_pos
	);
	si_tga_free(&tga);
	if(true != draw_result)
	{
		goto END;
	}
	result = true;
END:
	return result;
}
