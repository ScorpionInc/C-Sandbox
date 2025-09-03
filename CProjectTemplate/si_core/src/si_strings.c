//si_strings.c
#include "si_strings.h"

char* strn_clone_concat(const char* const p_left, const size_t left_size,
	const char* const p_right, const size_t right_size)
{
	char* p_result = NULL;
	if((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	size_t new_size = (left_size + right_size + 1u);
	// Handle Overflow
	if((new_size < left_size) || (new_size < right_size))
	{
		goto END;
	}
	p_result = calloc(new_size, sizeof(char));
	if(NULL == p_result)
	{
		goto END;
	}
	memcpy(p_result, p_left, left_size);
	memcpy(&p_result[left_size], p_right, right_size);
	p_result[new_size - 1u] = '\0';
END:
	return p_result;
}
char* str_clone_concat(const char* const p_left,
	const char* const p_right)
{
	// Defaults to strnlen(p_str, INT64_MAX) for string size.
	char* p_value = NULL;
	if((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	p_value = strn_clone_concat(
		p_left, strnlen(p_left, INT64_MAX),
		p_right, strnlen(p_right, INT64_MAX)
	);
END:
	return p_value;
}

char* strv_clone_concat(const size_t argc, ...)
{
	char* p_result = NULL;
	if(0 >= argc)
	{
		goto END;
	}
	char* p_tmp = NULL;
	va_list args = {0};
	va_start(args, argc);
	for(size_t iii = 0u; iii < argc; iii++)
	{
		char* p_arg = va_arg(args, char*);
		if(p_arg == NULL)
		{
			continue;
		}
		if(NULL == p_result)
		{
			p_result = strdup(p_arg);
			continue;
		}
		p_tmp = str_clone_concat(p_result, p_arg);
		if(NULL == p_tmp)
		{
			continue;
		}
		free(p_result);
		p_result = p_tmp;
		p_tmp = NULL;
	}
END:
	return p_result;
}

char* str_clone_join(const size_t argc,	const char* const p_seperator,
	const char** const pp_argv)
{
	char* p_result = NULL;
	if((0u >= argc) || (NULL == pp_argv))
	{
		goto END;
	}
	size_t sep_len = 0u;
	size_t new_size = 0u;
	size_t counter = 0u;
	if(NULL != p_seperator)
	{
		sep_len = strnlen(p_seperator, INT64_MAX);
	}
	for(size_t op = 0u; op < 2u; op++)
	{
		for(size_t iii = 0u; iii < argc; iii++)
		{
			const char* const p_next = pp_argv[iii];
			if(NULL == p_next)
			{
				continue;
			}
			size_t next_len = strnlen(p_next, INT64_MAX);
			if((0u == op) && (iii < argc - 1u))
			{
				// Handle overflow
				if((SIZE_MAX - next_len) < sep_len)
				{
					break;
				}
				next_len += sep_len;
			}
			// Handle overflow
			if((SIZE_MAX - next_len) < new_size)
			{
				break;
			}
			if(0u == op)
			{
				new_size += next_len;
			}
			else
			{
				memcpy(&(p_result[counter]), p_next, next_len);
				if(iii < argc - 1u)
				{
					memcpy(&p_result[counter + next_len], p_seperator, sep_len);
				}
				counter += (next_len + sep_len);
			}
		}
		if(op == 0u)
		{
			p_result = calloc(++new_size, sizeof(char));
			if(NULL == p_result)
			{
				break;
			}
		}
		else
		{
			p_result[new_size - 1u] = '\0';
			break;
		}
	}
END:
	return p_result;
}

char* strv_clone_join(const size_t argc, const char* const p_seperator, ...)
{
	char* p_result = NULL;
	const char** pp_argv = calloc(argc, sizeof(char*));
	if(NULL == pp_argv)
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_seperator);
	size_t valid_counter = 0u;
	for(size_t iii = 0u; iii < argc; iii++)
	{
		char* p_arg = va_arg(args, char*);
		if(p_arg == NULL)
		{
			continue;
		}
		pp_argv[valid_counter++] = p_arg;
	}
	p_result = str_clone_join(valid_counter, p_seperator, pp_argv);
	free(pp_argv);
	pp_argv = NULL;
	va_end(args);
END:
	return p_result;
}

void strn_chr_remap(char* const p_input_str, const size_t input_size,
	chr_remap_f p_map_chr)
{
	if(NULL == p_input_str)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < input_size; iii++)
	{
		const char next_char = p_map_chr(p_input_str[iii], iii);
		p_input_str[iii] = next_char;
	}
END:
	return;
}
void str_chr_remap(char* const p_input_str, chr_remap_f p_map_chr)
{
	if((NULL == p_input_str) || (NULL == p_map_chr))
	{
		goto END;
	}
	const size_t str_len = strnlen(p_input_str, INT64_MAX);
	strn_chr_remap(p_input_str, str_len, p_map_chr);
END:
	return;
}

/** Doxygen
 * @brief Maps characters to UPPERCASE using ctype toupper().
 * 
 * @param old_char Original char value.
 * @param char_index Index of the char in the string. (Unused)
 * 
 * @return Returns a char from old_char as UPPERCASE on success.
 */
static inline char remap_to_upper(const char old_char, const size_t char_index)
{
	return toupper(old_char);
}
inline void strn_to_uppercase(char* const p_input_str, const size_t input_size)
{
	strn_chr_remap(p_input_str, input_size, remap_to_upper);
}
inline void str_to_uppercase(char* const p_input_str)
{
	str_chr_remap(p_input_str, remap_to_upper);
}

/** Doxygen
 * @brief Maps characters to lowercase using ctype tolower().
 * 
 * @param old_char Original char value.
 * @param char_index Index of the char in the string. (Unused)
 * 
 * @return Returns a char from old_char as lowercase on success.
 */
static inline char remap_to_lower(const char old_char, const size_t char_index)
{
	return tolower(old_char);
}
inline void strn_to_lowercase(char* const p_input_str, const size_t input_size)
{
	strn_chr_remap(p_input_str, input_size, remap_to_lower);
}
inline void str_to_lowercase(char* const p_input_str)
{
	str_chr_remap(p_input_str, remap_to_lower);
}

char* strn_clone_substitute_7(
	const char* const p_haystack, const size_t haystack_size,
	const char* const p_needle, const size_t needle_size,
	const char* const p_value, const size_t value_size,
	int (*p_strncmp)(const char* p_s1, const char* p_s2, const size_t n))
{
	char* p_result = NULL;
	if((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_value))
	{
		goto END;
	}
	if(0 >= haystack_size)
	{
		goto END;
	}
	if(needle_size > haystack_size)
	{
		p_result = strndup(p_haystack, haystack_size);
		goto END;
	}
	char* p_tmp = NULL;
	size_t current_size = 0u;
	size_t next_start = 0u;
	size_t tail_length = 0u;
	for(size_t iii = 0u; iii < haystack_size; iii++)
	{
		const char* const p_next = &(p_haystack[iii]);
		if((haystack_size - iii) < needle_size)
		{
			tail_length = (haystack_size - iii);
			break;
		}
		const int cmp_result = p_strncmp(p_next, p_needle, needle_size);
		if(0 == cmp_result)
		{
			if(NULL == p_result)
			{
				current_size = (iii - next_start);
				p_result = strndup(
					&(p_haystack[next_start]), current_size
				);
			}
			else
			{
				p_tmp = strn_clone_concat(
					p_result, current_size,
					&(p_haystack[next_start]), iii - next_start
				);
				current_size += (iii - next_start);
				free(p_result);
				p_result = p_tmp;
				p_tmp = NULL;
			}
			p_tmp = strn_clone_concat(
				p_result, current_size,
				p_value, value_size
			);
			current_size += value_size;
			free(p_result);
			p_result = p_tmp;
			p_tmp = NULL;
			next_start = iii + needle_size;
			iii += needle_size - 1u;
			continue;
		}
	}
	if(0u < tail_length)
	{
		current_size += tail_length;
		if(NULL != p_result)
		{
			p_tmp = str_clone_concat(
				p_result, &(p_haystack[haystack_size - tail_length])
			);
			if(NULL != p_tmp)
			{
				free(p_result);
				p_result = p_tmp;
				p_tmp = NULL;
			}
		}
		else
		{
			p_result = strndup(
				&(p_haystack[haystack_size - tail_length]), tail_length
			);
		}
	}
END:
	return p_result;
}
inline char* strn_clone_substitute(
	const char* const p_haystack, const size_t haystack_size,
	const char* const p_needle, const size_t needle_size,
	const char* const p_value, const size_t value_size)
{
	// Default value of p_strncmp if strncmp.
	return strn_clone_substitute_7(
		p_haystack, haystack_size,
		p_needle, needle_size,
		p_value, value_size,
		strncmp
	);
}

char* str_clone_substitute_4(const char* const p_haystack,
	const char* const p_needle, const char* const p_value,
	int (*p_strncmp)(const char* p_s1, const char* p_s2, const size_t n))
{
	char* p_result = NULL;
	if((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_value))
	{
		goto END;
	}
	const size_t haystack_len = strnlen(p_haystack, INT64_MAX);
	const size_t needle_len = strnlen(p_needle, INT64_MAX);
	const size_t value_len = strnlen(p_value, INT64_MAX);
	p_result = strn_clone_substitute_7(
		p_haystack, haystack_len,
		p_needle, needle_len,
		p_value, value_len,
		p_strncmp
	);
END:
	return p_result;
}
inline char* str_clone_substitute(const char* const p_haystack,
	const char* const p_needle, const char* const p_value)
{
	// Default value of p_strncmp if strncmp.
	return str_clone_substitute_4(
		p_haystack, p_needle, p_value, strncmp
	);
}

char* pop_str_from_heap(uint8_t** const pp_buffer, size_t* const p_buffer_size)
{
	char* p_result = NULL;
	if((NULL == pp_buffer) || (NULL == p_buffer_size))
	{
		goto END;
	}
	if(0u >= *p_buffer_size)
	{
		goto END;
	}
	size_t string_length = strnlen((char*)*pp_buffer, *p_buffer_size);
	if(string_length >= *p_buffer_size)
	{
		goto END;
	}
	p_result = calloc(string_length + 1u, sizeof(char));
	if(NULL == p_result)
	{
		goto END;
	}
	strncpy(p_result, (const char*)(*pp_buffer), string_length + 1u);
	p_result[string_length] = '\0';
	const size_t non_str_len = *p_buffer_size - (string_length + 1u);
	if((0u < non_str_len) && ((string_length + 1u) < *p_buffer_size))
	{
		memcpy(
			&((*pp_buffer)[0]), &((*pp_buffer)[string_length + 1u]), non_str_len
		);
	}
	*pp_buffer = realloc(*pp_buffer, non_str_len);
	*p_buffer_size = non_str_len;
END:
	return p_result;
}
