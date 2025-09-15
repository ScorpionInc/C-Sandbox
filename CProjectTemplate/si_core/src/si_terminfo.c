// si_terminfo.c
#include "si_terminfo.h"

#ifdef __linux__

bool si_tui_is_ansi_l(const int file_d)
{
	bool result = false;
	if(0 > file_d)
	{
		goto END;
	}
	const int is_atty = isatty(file_d);
	result = (1 == is_atty);
END:
	return result;
}

bool si_tui_update_winsize(struct winsize* p_size, const int file_d)
{
	bool result = false;
	if((NULL == p_size) || (0 > file_d))
	{
		goto END;
	}
	const int ioctl_result = ioctl(file_d, TIOCGWINSZ, p_size);
	result = (0 == ioctl_result);
END:
	return result;
}

#endif//__linux__


bool si_tui_is_ansi(FILE* const p_file)
{
	bool result = false;
	if(NULL == p_file)
	{
		goto END;
	}
#ifdef __linux__
	const int file_no = fileno(p_file);
	if(0 > file_no)
	{
		goto END;
	}
	result = si_tui_is_ansi_l(file_no);
#endif//__linux__
END:
	return result;
}

bool si_tui_supports_truecolor()
{
	bool result = false;
	const char* const colorterm_env = getenv("COLORTERM");
	if(NULL == colorterm_env)
	{
		goto END;
	}
	
	const char* const p_valid_values[] = {
		"truecolor",
		"24bit"
	};
	// This could be a hard coded value, but doing these checks allows us to
	// easily change or add more valid values are needed in the future.
	const size_t valid_count = sizeof(p_valid_values) / sizeof(const char*);
	if(0u >= valid_count)
	{
		goto END;
	}
	size_t longest_valid_value = strnlen(p_valid_values[0], INT_MAX);
	for(size_t iii = 1u; iii < valid_count; iii++)
	{
		const size_t next_len = strnlen(p_valid_values[iii], INT_MAX);
		if(longest_valid_value < next_len)
		{
			longest_valid_value = next_len;
		}
	}
	if(0u >= longest_valid_value)
	{
		goto END;
	}

	// Actually checking of environment value
	for(size_t iii = 0u; iii < valid_count; iii++)
	{
		const int cmp_result = strncmp(
			colorterm_env, p_valid_values[iii], longest_valid_value
		);
		if(0 == cmp_result)
		{
			result = true;
			goto END;
		}
	}
END:
	return result;
}

bool si_tui_supports_unicode()
{
	bool result = false;
	const char* p_locale = setlocale(LC_CTYPE, NULL);
	if(NULL == p_locale)
	{
		goto END;
	}
	const char* const p_needle = "UTF";
	const char* const p_result = strstr(p_locale, p_needle);
	if(NULL == p_result)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

void si_tui_send_ansi(FILE* const p_file, const char* p_code, ...)
{
	if((NULL == p_file) || (NULL == p_code))
	{
		goto END;
	}
	const bool is_ansi = si_tui_is_ansi(p_file);
	if(true != is_ansi)
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_code);
	vfprintf(p_file, p_code, args);
	va_end(args);
END:
	return;
}


void si_terminfo_init(si_terminfo_t* const p_terminfo, FILE* const p_file)
{
	// Values that don't often change about the terminal get defined here.
	if(NULL == p_terminfo)
	{
		goto END;
	}
	p_terminfo->p_file = p_file;
	p_terminfo->p_terminal_name = NULL;
	p_terminfo->supports_unicode = si_tui_supports_unicode();
	p_terminfo->supports_ansi = si_tui_is_ansi(p_file);
	p_terminfo->supports_true_color = (
		p_terminfo->supports_ansi && si_tui_supports_truecolor()
	);
	p_terminfo->COLUMNS = 0u;
	p_terminfo->ROWS = 0u;
	if(NULL != p_file)
	{
		si_terminfo_update(p_terminfo);
	}
END:
	return;
}

si_terminfo_t* si_terminfo_new(FILE* const p_file)
{
	si_terminfo_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_terminfo_t));
	if(NULL == p_result)
	{
		goto END;
	}
	si_terminfo_init(p_result, p_file);
END:
	return p_result;
}

bool si_terminfo_update(si_terminfo_t* const p_terminfo)
{
	// Updates often changing terminal values here
	bool result = false;
	if(NULL == p_terminfo)
	{
		goto END;
	}
#ifdef __linux__
	struct winsize wsize = {0};
	if(NULL == p_terminfo->p_file)
	{
		goto END;
	}
	const int file_no = fileno(p_terminfo->p_file);
	if(0 > file_no)
	{
		goto END;
	}
	const bool update_result = si_tui_update_winsize(&wsize, file_no);
	if(true != update_result)
	{
		goto END;
	}
	p_terminfo->ROWS = wsize.ws_row;
	p_terminfo->COLUMNS = wsize.ws_col;
	result = true;
#endif//__linux__
END:
	return result;
}

void si_terminfo_send_ansi(si_terminfo_t* const p_terminfo,
	const char* p_code, ...)
{
	if((NULL == p_terminfo) || (NULL == p_code))
	{
		goto END;
	}
	if((NULL == p_terminfo->p_file) || (true != p_terminfo->supports_ansi))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_code);
	vfprintf(p_terminfo->p_file, p_code, args);
	va_end(args);
END:
	return;
}

void si_terminfo_free(si_terminfo_t* const p_terminfo)
{
	//! TODO No heap data (yet?)
	if(NULL == p_terminfo)
	{
		goto END;
	}
END:
	return;
}

void si_terminfo_destroy(si_terminfo_t** pp_terminfo)
{
	if(NULL == pp_terminfo)
	{
		goto END;
	}
	if(NULL == *pp_terminfo)
	{
		// Already Freed
		goto END;
	}
	si_terminfo_free(*pp_terminfo);
	free(*pp_terminfo);
	*pp_terminfo = NULL;
END:
	return;
}
