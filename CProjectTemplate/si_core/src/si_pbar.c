// si_pbar.c
#include "si_pbar.h"

void si_pbar_init(si_pbar_t* const p_bar)
{
	if (NULL == p_bar)
	{
		goto END;
	}
	p_bar->alignment = PBAR_ALIGN_UNSPECIFIED;
	p_bar->bar_head_char = SI_TUI_PBAR_DEFAULT_BAR_HEAD;
	p_bar->empty_bar_char = SI_TUI_PBAR_DEFAULT_EMPTY_BAR;
	p_bar->enable_label = true;
	p_bar->filled_bar_char = SI_TUI_PBAR_DEFAULT_FILLED_BAR;
	p_bar->label_side = PBAR_LABEL_UNSPECIFIED;
	p_bar->left_bar_char = SI_TUI_PBAR_DEFAULT_LEFT_BAR;
	p_bar->max_bar_width = SI_TUI_PBAR_DISABLE_MAX_BAR;
	p_bar->p_begin_color = NULL;
	p_bar->p_end_color = NULL;
	p_bar->p_label_color = NULL;
	p_bar->p_prefix = NULL;
	p_bar->p_suffix = NULL;
	p_bar->precision = 1u;
	p_bar->right_bar_char = SI_TUI_PBAR_DEFAULT_RIGHT_BAR;
END:
	return;
}

si_pbar_t* si_pbar_new()
{
	si_pbar_t* p_new = calloc(1u, sizeof(si_pbar_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_pbar_init(p_new);
END:
	return p_new;
}

void si_pbar_terminal_setup(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo)
{
	if ((NULL == p_bar) || (NULL == p_terminfo))
	{
		goto END;
	}
	if (NULL == p_terminfo->p_file)
	{
		goto END;
	}

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_BOTTOM):
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
		// We print a line break here as it ensures we have a new line to use for
		// the progress bar if cursor is not at the terminal line begining.
		fprintf(p_terminfo->p_file, "\n");
		break;
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Save current cursor position (\0337)
	si_terminfo_send_ansi(p_terminfo, "\0337");
	si_terminfo_send_ansi(p_terminfo, "\033[s");

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_BOTTOM):
		// Set the scrollable margin
		si_terminfo_send_ansi(
			p_terminfo, "\033[0;%dr", (p_terminfo->ROWS - 1u)
		);
		break;
	case(PBAR_ALIGN_TOP):
		// Set the scrollable margin
		si_terminfo_send_ansi(
			p_terminfo, "\033[1;%dr", (p_terminfo->ROWS)
		);
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Restore original cursor position (\0338)
	si_terminfo_send_ansi(p_terminfo, "\033[u");
	si_terminfo_send_ansi(p_terminfo, "\0338");

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_BOTTOM):
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
		// Move cursor back up from new line
		si_terminfo_send_ansi(p_terminfo, "\033[1A");
		break;
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}
END:
	return;
}

/** Doxygen
 * @brief Local function to clamp a double percentage between 0.0 and 1.0
 * 
 * @param percentage Unclamped double value to be used.
 * 
 * @return Returns a double between 0.0 and 1.0
 */
static double clamp_percentage_d(const double percentage)
{
	// Clamps percentage value to valid range
	double mut_percentage = percentage;
	if (0.0 > mut_percentage)
	{
		mut_percentage = 0.0;
	}
	if (1.0 < mut_percentage)
	{
		mut_percentage = 1.0;
	}
	return mut_percentage;
}

/** Doxygen
 * @brief Returns the size of the progress bar label if it were printed.
 * 
 * @param p_bar Pointer to progress bar information.
 * @param percentage Double value of current percentage between 0 and 1.
 * 
 * @return Returns int character count of potential output string.
 */
static int si_pbar_label_strlen(const si_pbar_t* const p_bar,
	const double percentage)
{
	int result = 0u;
	if (NULL == p_bar)
	{
		goto END;
	}
	if (true != p_bar->enable_label)
	{
		goto END;
	}
	const double scaled_percentage = clamp_percentage_d(percentage) * 100.0;
	const int result_len = snprintf(
		NULL, 0, "%.*lf%%", p_bar->precision, scaled_percentage
	);
	if (0 > result_len)
	{
		goto END;
	}
	result = result_len;
END:
	return result;
}

/** Doxygen
 * @brief Prints the label of a progress bar to file at current position.
 * 
 * @param p_file Pointer to FILE to print to.
 * @param p_bar Pointer to progress bar information.
 * @param percentage Double value of current percentage between 0 and 1.
 */
static void si_pbar_fprint_label(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo, const double percentage)
{
	if ((NULL == p_bar) || (NULL == p_terminfo))
	{
		goto END;
	}
	if ((NULL == p_terminfo->p_file) || (true != p_bar->enable_label))
	{
		goto END;
	}
	const double scaled_percentage = clamp_percentage_d(percentage) * 100.0;
	if (NULL != p_bar->p_label_color)
	{
		si_terminfo_set_color(p_terminfo, p_bar->p_label_color);
	}
	fprintf(p_terminfo->p_file, "%.*lf%%", p_bar->precision, scaled_percentage);
	if (NULL != p_bar->p_label_color)
	{
		si_terminfo_send_ansi(p_terminfo, "\033[%dm", ANSI_FR_RESET);
	}
	fflush(p_terminfo->p_file);
END:
	return;
}

/** Doxygen
 * @brief Returns the size of the progress bar's bar if it were printed.
 * 
 * @param p_file Pointer to FILE to print to.
 * @param p_bar Pointer to progress bar information.
 * @param percentage Double value of current percentage between 0 and 1.
 */
const uint16_t si_pbar_bar_strlen(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo, const double percentage)
{
	uint16_t bar_size = 0u;
	if ((NULL == p_bar) || (NULL == p_terminfo))
	{
		goto END;
	}
	if (NULL == p_terminfo->p_file)
	{
		goto END;
	}
	const double clamped_percentage = clamp_percentage_d(percentage);

	bar_size = p_terminfo->COLUMNS;
	if ((0u < p_bar->max_bar_width) &&
	   (p_terminfo->COLUMNS > p_bar->max_bar_width))
	{
		bar_size = p_bar->max_bar_width;
	}
	const size_t label_size = (si_pbar_label_strlen(p_bar, percentage) + 1u);
	if (bar_size <= label_size)
	{
		goto END;
	}
	bar_size -= label_size;
	if (NULL != p_bar->p_prefix)
	{
		const size_t prefix_len = strnlen(p_bar->p_prefix, INT_MAX);
		bar_size -= prefix_len;
	}
	if (NULL != p_bar->p_suffix)
	{
		const size_t suffix_len = strnlen(p_bar->p_suffix, INT_MAX);
		bar_size -= suffix_len;
	}
END:
	return bar_size;
}

/** Doxygen
 * @brief Prints the bar of a progress bar to file at current position.
 * 
 * @param p_file Pointer to FILE to print to.
 * @param p_bar Pointer to progress bar information.
 * @param percentage Double value of current percentage between 0 and 1.
 */
static void si_pbar_fprint_bar(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo, const double percentage)
{
	if ((NULL == p_bar) || (NULL == p_terminfo))
	{
		goto END;
	}
	if (NULL == p_terminfo->p_file)
	{
		goto END;
	}
	const double clamped_percentage = clamp_percentage_d(percentage);
	const uint16_t bar_size = si_pbar_bar_strlen(
		p_bar, p_terminfo, percentage
	);
	uint16_t inner_size = bar_size;
	if ('\0' != p_bar->left_bar_char)
	{
		inner_size -= 1u;
	}
	if ('\0' != p_bar->right_bar_char)
	{
		inner_size -= 1u;
	}

	si_true_color_t next_color = {0};
	si_true_color_init(&next_color, 0xFF, 0xFF, 0xFF);

	fprintf(p_terminfo->p_file, "%c", p_bar->left_bar_char);
	for (size_t iii = 0u; iii < inner_size; iii++)
	{
		const double progress = (((double)iii) / inner_size);
		const double next_progress = (((double)(iii + 1u)) / inner_size);
		if (NULL != p_bar->p_begin_color)
		{
			memcpy(&next_color, p_bar->p_begin_color, sizeof(si_true_color_t));
		}
		if (NULL != p_bar->p_end_color)
		{
			// Accuracy loss due to cast here is fine affecting only visuals.
			si_true_color_mix(&next_color, p_bar->p_end_color, (float)progress);
		}
		if (NULL != p_bar->p_begin_color)
		{
			si_terminfo_set_color(p_terminfo, &next_color);
		}
		const bool is_filled = (progress <= clamped_percentage);
		char next_bar = is_filled ?
			p_bar->filled_bar_char : p_bar->empty_bar_char;
		const bool is_head = is_filled && (next_progress > clamped_percentage);
		if (is_head && ('\0' != p_bar->bar_head_char))
		{
			next_bar = p_bar->bar_head_char;
		}
		fprintf(p_terminfo->p_file, "%c", next_bar);
	}
	if (NULL != p_bar->p_begin_color)
	{
		if (NULL != p_bar->p_label_color)
		{
			si_terminfo_set_color(p_terminfo, p_bar->p_label_color);
		}
		else
		{
			si_terminfo_send_ansi(p_terminfo, "\033[%dm", ANSI_FR_RESET);
		}
	}
	fprintf(p_terminfo->p_file, "%c", p_bar->right_bar_char);
	fflush(p_terminfo->p_file);
END:
	return;
}

void si_pbar_fprint(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo, const double percentage)
{
	if ((NULL == p_terminfo) || (NULL == p_bar))
	{
		goto END;
	}
	if (NULL == p_terminfo->p_file)
	{
		goto END;
	}

	// Save current cursor position (\0337)
	si_terminfo_send_ansi(p_terminfo, "\0337");
	si_terminfo_send_ansi(p_terminfo, "\033[s");

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_TOP):
		// Move cursor to progress bar line (top)
		si_terminfo_send_ansi(
			p_terminfo, "\033[%d;0H", (0)
		);
		break;
	case(PBAR_ALIGN_BOTTOM):
		// Move cursor to progress bar line (bottom)
		si_terminfo_send_ansi(
			p_terminfo, "\033[%d;0H", (p_terminfo->ROWS)
		);
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_BOTTOM):
		// Clears the progress bar line.
		si_terminfo_send_ansi(p_terminfo, "\033[0K");
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	fprintf(p_terminfo->p_file, "\r");
	// Invalid & Unspecified case fall's through switch case intentionally.
	switch (p_bar->label_side)
	{
	case(PBAR_LABEL_UNSPECIFIED):
	case(PBAR_LABEL_RIGHT):
		si_pbar_fprint_bar(p_bar, p_terminfo, percentage);
		break;
	case(PBAR_LABEL_LEFT):
		si_pbar_fprint_label(p_bar, p_terminfo, percentage);
		fprintf(p_terminfo->p_file, " ");
		break;
	case(PBAR_LABEL_INVALID):
	default:
		break;
	}
	switch (p_bar->label_side)
	{
	case(PBAR_LABEL_UNSPECIFIED):
	case(PBAR_LABEL_RIGHT):
		fprintf(p_terminfo->p_file, " ");
		si_pbar_fprint_label(p_bar, p_terminfo, percentage);
		break;
	case(PBAR_LABEL_LEFT):
		si_pbar_fprint_bar(p_bar, p_terminfo, percentage);
		break;
	case(PBAR_LABEL_INVALID):
	default:
		break;
	}

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
		fprintf(p_terminfo->p_file, "\n");
		break;
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_BOTTOM):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Restore original cursor position (\0338)
	si_terminfo_send_ansi(p_terminfo, "\033[u");
	si_terminfo_send_ansi(p_terminfo, "\0338");
END:
	return;
}

void si_pbar_terminal_restore(const si_pbar_t* const p_bar,
	si_terminfo_t* const p_terminfo)
{
	// Save current cursor position (\0337)
	si_terminfo_send_ansi(p_terminfo, "\0337");
	si_terminfo_send_ansi(p_terminfo, "\033[s");
	
	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_BOTTOM):
		// Restores/Removes the scrollable margin
		si_terminfo_send_ansi(
			p_terminfo, "\033[0;%dr", (p_terminfo->ROWS)
		);
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_TOP):
		// Move cursor to progress bar line (top)
		si_terminfo_send_ansi(
			p_terminfo, "\033[%d;0H", (0)
		);
		break;
	case(PBAR_ALIGN_BOTTOM):
		// Move cursor to progress bar line (bottom)
		si_terminfo_send_ansi(
			p_terminfo, "\033[%d;0H", (p_terminfo->ROWS)
		);
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Switch case fall-through is an intended behavior.
	switch (p_bar->alignment)
	{
	case(PBAR_ALIGN_TOP):
	case(PBAR_ALIGN_BOTTOM):
		// Clears the progress bar line.
		si_terminfo_send_ansi(p_terminfo, "\033[0K");
		break;
	case(PBAR_ALIGN_UNSPECIFIED):
	case(PBAR_ALIGN_INLINE):
	case(PBAR_ALIGN_INVALID):
	default:
		break;
	}

	// Restore original cursor position (\0338)
	si_terminfo_send_ansi(p_terminfo, "\033[u");
	si_terminfo_send_ansi(p_terminfo, "\0338");
}
