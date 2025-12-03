// si_strings.c
#include "si_strings.h"

#ifndef _GNU_SOURCE
char* strndup(const char* const p_src, const size_t size)
{
	char* p_result = NULL;
	// According to compiler this NULL-check is not needed.
	// This NULL-check has been retained as better safe than sorry(also BARR-C)
	if ((NULL == p_src) || (0u >= size))
	{
		goto END;
	}
	p_result = calloc(size + 1u, sizeof(char));
	if (NULL == p_result)
	{
		goto END;
	}
	memcpy(p_result, p_src, size);
	p_result[size] = '\0';
END:
	return p_result;
}
#endif//_GNU_SOURCE

#if defined(__FreeBSD__) || defined(__APPLE__)
// Has strnstr()
#else
char* strnstr(const char* const p_haystack, const char* const p_needle,
	const size_t hay_size)
{
	char* p_result = NULL;
	if ((NULL == p_haystack) || (0u >= hay_size))
	{
		goto END;
	}
	if (NULL == p_needle)
	{
		p_result = (char*)p_haystack;
		goto END;
	}

	const size_t max_len = (INT64_MAX > hay_size) ? (hay_size + 1) : INT64_MAX;
	const size_t needle_len = strnlen(p_needle, max_len);
	if (max_len <= needle_len)
	{
		goto END;
	}
	if (0u == needle_len)
	{
		p_result = (char*)p_haystack;
		goto END;
	}

	for (size_t iii = 0u; iii < hay_size - needle_len; iii++)
	{
		const int cmp_result = strncmp(&p_haystack[iii], p_needle, needle_len);
		if (0 == cmp_result)
		{
			p_result = (char*)&p_haystack[iii];
			goto END;
		}
	}
END:
	return p_result;
}
#endif//strnstr()

char* strn_new(const size_t str_len, const char* const p_pattern,
	const size_t pattern_len)
{
	char* p_result = NULL;
	if (0u >= str_len)
	{
		goto END;
	}
	p_result = calloc(str_len + 1u, sizeof(char));
	if ((NULL == p_result) || (NULL == p_pattern) || (0u >= pattern_len))
	{
		goto END;
	}
	const size_t full_patterns = (str_len / pattern_len);
	const size_t pattern_part = (str_len % pattern_len);
	for (size_t iii = 0u; iii < full_patterns; iii++)
	{
		// strncat()'s return type cannot be error checked.
		(void)strncat(p_result, p_pattern, pattern_len);
	}
	const size_t full_end = (full_patterns * pattern_len);
	// strncat()'s return type cannot be error checked.
	(void)strncat(&(p_result[full_end]), p_pattern, pattern_part);
	// Should still be NULL but NULL-Terminate just to be safe.
	p_result[str_len] = '\0';
END:
	return p_result;
}
char* str_new(const size_t str_len, const char* const p_pattern)
{
	char* p_result = NULL;
	if (0u >= str_len)
	{
		goto END;
	}
	size_t pattern_len = 0u;
	if (NULL != p_pattern)
	{
		pattern_len = strnlen(p_pattern, INT64_MAX);
		if (INT64_MAX <= pattern_len)
		{
			goto END;
		}
	}
	p_result = strn_new(str_len, p_pattern, pattern_len);
END:
	return p_result;
}


size_t strn_pad(char** const pp_str, const size_t str_len, const int pad)
{
	size_t result = 0u;
	if (NULL == pp_str)
	{
		goto END;
	}
	if (NULL == *pp_str)
	{
		goto END;
	}
	const char padding_char = ' ';
	const int abs_pad = (0 > pad) ? (-1 * pad) : pad;
	const size_t abs_pad_s = (size_t)abs_pad;
	const size_t expect_len = (str_len >= abs_pad_s) ? str_len : abs_pad_s;
	const size_t pad_cnt = (expect_len > str_len) ? expect_len - str_len : 0u;
	char* const p_realloc = realloc(*pp_str, expect_len + 1u);
	if (NULL == p_realloc)
	{
		goto END;
	}
	*pp_str = p_realloc;
	// Was going to use snprintf() for padding here but it doesn't handle
	// memory overlapping for in place string formatting.
	if (0 > pad)
	{
		// Padding goes on the right-side.
		memset(&((*pp_str)[str_len]), padding_char, pad_cnt);
	}
	else
	{
		// Padding goes on the left-side.
		memmove(&((*pp_str)[pad_cnt]), *pp_str, expect_len - pad_cnt);
		memset(*pp_str, padding_char, pad_cnt);
	}
	// NULL-Terminate the C-String result.
	(*pp_str)[expect_len] = '\0';
	result = expect_len;
END:
	return result;
}
size_t str_pad(char** const pp_str, const int pad)
{
	size_t result = 0;
	if (NULL == pp_str)
	{
		goto END;
	}
	if (NULL == *pp_str)
	{
		goto END;
	}
	const size_t str_len = strnlen(*pp_str, INT64_MAX);
	if (str_len >= INT64_MAX)
	{
		goto END;
	}
	result = strn_pad(pp_str, str_len, pad);
END:
	return result;
}
char* strn_clone_pad(const char* const p_str, const size_t str_len, const int pad)
{
	char* p_result = NULL;
	if ((NULL == p_str) || (0u >= str_len))
	{
		goto END;
	}
	const int abs_pad = (0 > pad) ? (-1 * pad) : pad;
	const size_t abs_pad_s = (size_t)abs_pad;
	const size_t expect_len = (str_len >= abs_pad_s) ? str_len : abs_pad_s;
	p_result = strndup(p_str, str_len);
	const size_t new_size = strn_pad(&p_result, str_len, pad);
	if ((0 >= new_size) || (expect_len != new_size))
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
END:
	return p_result;
}
char* str_clone_pad(const char* const p_str, const int pad)
{
	char* p_result = NULL;
	if (NULL == p_str)
	{
		goto END;
	}
	const size_t str_len = strnlen(p_str, INT64_MAX);
	if (INT64_MAX <= str_len)
	{
		goto END;
	}
	p_result = strn_clone_pad(p_str, str_len, pad);
END:
	return p_result;
}


size_t strn_lgrow_concat(char** pp_left, const size_t left_size,
	const char* const p_right, const size_t right_size)
{
	size_t result = SIZE_MAX;
	if ((NULL == pp_left) || (NULL == p_right))
	{
		goto END;
	}
	const size_t new_size = (left_size + right_size);
	if (0u >= right_size)
	{
		result = new_size;
		goto END;
	}
	if (NULL == *pp_left)
	{
		*pp_left = calloc(new_size + 1u, sizeof(char));
		if (NULL == *pp_left)
		{
			goto END;
		}
	}
	else
	{
		char* const p_grow = realloc(*pp_left, new_size + 1u);
		if (NULL == p_grow)
		{
			goto END;
		}
		*pp_left = p_grow;
	}
	// strncat()'s return value is not useful.
	(void)strncat(*pp_left, p_right, right_size);
	result = new_size;
END:
	return result;
}
size_t str_lgrow_concat(char** pp_left, const char* const p_right)
{
	size_t result = SIZE_MAX;
	if ((NULL == pp_left) || (NULL == p_right))
	{
		goto END;
	}
	size_t left_size = 0u;
	if (NULL != *pp_left)
	{
		left_size = strnlen(*pp_left, INT64_MAX);
	}
	const size_t right_size = strnlen(p_right, INT64_MAX);
	if ((left_size >= INT64_MAX) || (right_size >= INT64_MAX))
	{
		goto END;
	}
	result = strn_lgrow_concat(pp_left, left_size, p_right, right_size);
END:
	return result;
}

size_t strn_rgrow_concat(const char* const p_left, const size_t left_size,
	char** pp_right, const size_t right_size)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_left) || (NULL == pp_right))
	{
		goto END;
	}
	const size_t new_size = (left_size + right_size);
	if (0u >= left_size)
	{
		result = new_size;
		goto END;
	}
	if (NULL == *pp_right)
	{
		*pp_right = calloc(new_size + 1u, sizeof(char));
		if (NULL == *pp_right)
		{
			goto END;
		}
	}
	else
	{
		char* const p_grow = realloc(*pp_right, new_size + 1u);
		if (NULL == p_grow)
		{
			goto END;
		}
		*pp_right = p_grow;
	}
	char* const p_right_start = &((*pp_right)[left_size]);
	// Memory may overlap, must use memmove() not memcpy()
	memmove(p_right_start, *pp_right, right_size);
	memmove(*pp_right, p_left, left_size);
	(*pp_right)[new_size] = '\0';
	result = new_size;
END:
	return result;
}
size_t str_rgrow_concat(const char* const p_left, char** pp_right)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_left) || (NULL == pp_right))
	{
		goto END;
	}
	size_t right_size = 0u;
	if (NULL != *pp_right)
	{
		right_size = strnlen(*pp_right, INT64_MAX);
	}
	const size_t left_size = strnlen(p_left, INT64_MAX);
	if ((left_size >= INT64_MAX) || (right_size >= INT64_MAX))
	{
		goto END;
	}
	result = strn_rgrow_concat(p_left, left_size, pp_right, right_size);
END:
	return result;
}

char* strn_clone_concat(const char* const p_left, const size_t left_size,
	const char* const p_right, const size_t right_size)
{
	char* p_result = NULL;
	if ((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	size_t new_size = (left_size + right_size + 1u);
	// Handle Overflow
	if ((new_size < left_size) || (new_size < right_size))
	{
		goto END;
	}
	p_result = calloc(new_size, sizeof(char));
	if (NULL == p_result)
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
	if ((NULL == p_left) || (NULL == p_right))
	{
		goto END;
	}
	const size_t left_size = strnlen(p_left, INT64_MAX);
	const size_t right_size = strnlen(p_right, INT64_MAX);
	if ((left_size >= INT64_MAX) || (right_size >= INT64_MAX))
	{
		goto END;
	}
	p_value = strn_clone_concat(
		p_left, left_size,
		p_right, right_size
	);
END:
	return p_value;
}

char* strv_clone_concat(const size_t argc, ...)
{
	char* p_result = NULL;
	if (0 >= argc)
	{
		goto END;
	}
	char* p_tmp = NULL;
	va_list args = {0};
	va_start(args, argc);
	for (size_t iii = 0u; iii < argc; iii++)
	{
		char* p_arg = va_arg(args, char*);
		if (NULL == p_arg)
		{
			continue;
		}
		if (NULL == p_result)
		{
			p_result = strdup(p_arg);
			continue;
		}
		p_tmp = str_clone_concat(p_result, p_arg);
		if (NULL == p_tmp)
		{
			continue;
		}
		free(p_result);
		p_result = p_tmp;
		p_tmp = NULL;
	}
	va_end(args);
END:
	return p_result;
}


char* str_clone_join(const size_t argc,	const char* const p_seperator,
	const char** const pp_argv)
{
	char* p_result = NULL;
	if ((0u >= argc) || (NULL == pp_argv))
	{
		goto END;
	}
	size_t sep_len = 0u;
	size_t new_size = 0u;
	size_t counter = 0u;
	if (NULL != p_seperator)
	{
		sep_len = strnlen(p_seperator, INT64_MAX);
		if (INT64_MAX <= sep_len)
		{
			sep_len = 0u;
		}
	}
	for (size_t opr = 0u; opr < 2u; opr++)
	{
		for (size_t iii = 0u; iii < argc; iii++)
		{
			const char* const p_next = pp_argv[iii];
			if (NULL == p_next)
			{
				continue;
			}
			size_t next_len = strnlen(p_next, INT64_MAX);
			if (INT64_MAX <= next_len)
			{
				continue;
			}
			if ((0u == opr) && (iii < argc - 1u))
			{
				// Handle overflow
				if ((SIZE_MAX - next_len) < sep_len)
				{
					break;
				}
				next_len += sep_len;
			}
			// Handle overflow
			if ((SIZE_MAX - next_len) < new_size)
			{
				break;
			}
			if (0u == opr)
			{
				new_size += next_len;
			}
			else
			{
				memcpy(&(p_result[counter]), p_next, next_len);
				if (iii < argc - 1u)
				{
					memcpy(&p_result[counter + next_len], p_seperator, sep_len);
				}
				counter += (next_len + sep_len);
			}
		}
		if (0u == opr)
		{
			p_result = calloc(++new_size, sizeof(char));
			if (NULL == p_result)
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
	if (NULL == pp_argv)
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_seperator);
	size_t valid_counter = 0u;
	for (size_t iii = 0u; iii < argc; iii++)
	{
		char* p_arg = va_arg(args, char*);
		if (NULL == p_arg)
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


char** strn_split(const char* const p_haystack, const size_t haystack_len,
	const char* const p_needle, const size_t needle_len,
	size_t* const p_count)
{
	char** pp_result = NULL;
	if ((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_count))
	{
		goto END;
	}
	if (0u >= haystack_len)
	{
		goto END;
	}
	size_t tail = 0u;
	if ((0u >= needle_len) || (needle_len > haystack_len))
	{
		// No matches possible. Returns one element containing haystack clone.
		goto TAIL;
	}
	for (size_t iii = 0u; iii < (haystack_len - needle_len); iii++)
	{
		const int cmp_result = memcmp(&(p_haystack[iii]), p_needle, needle_len);
		if (0 == cmp_result)
		{
			*p_count += 1u;
			char** pp_tmp = NULL;
			pp_tmp = realloc(pp_result, (*p_count) * sizeof(char*));
			if (NULL == pp_tmp)
			{
				goto END;
			}
			pp_result = pp_tmp;
			const size_t next_len = (iii - tail) + 1u;
			pp_result[*p_count - 1u] = calloc(next_len, sizeof(char));
			if (NULL == pp_result[*p_count - 1u])
			{
				goto END;
			}
			memcpy(
				pp_result[*p_count - 1u], &(p_haystack[tail]), next_len - 1u
			);
			pp_result[*p_count - 1u][next_len - 1u] = '\0';
			tail = iii + needle_len;
			iii += needle_len;
		}
	}
TAIL:
	if ((haystack_len - 1u) <= tail)
	{
		goto END;
	}
	*p_count += 1u;
	pp_result = realloc(pp_result, (*p_count) * sizeof(char*));
	if (NULL == pp_result)
	{
		goto END;
	}
	const size_t tail_len = (haystack_len - tail);
	pp_result[*p_count - 1u] = strndup(&(p_haystack[tail]), tail_len);
END:
	return pp_result;
}
char** str_split(const char* const p_haystack, const char* p_needle,
	size_t* const p_count)
{
	char** pp_result = NULL;
	if ((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_count))
	{
		goto END;
	}
	const size_t haystack_len = strnlen(p_haystack, INT64_MAX);
	if (INT64_MAX <= haystack_len)
	{
		goto END;
	}
	const size_t needle_len = strnlen(p_needle, INT64_MAX);
	if (INT64_MAX <= needle_len)
	{
		goto END;
	}
	pp_result = strn_split(
		p_haystack, haystack_len, p_needle, needle_len, p_count
	);
END:
	return pp_result;
}

void str_split_destroy(char*** const ppp_array, const size_t arg_count)
{
	if (NULL == ppp_array)
	{
		goto END;
	}
	if (NULL == *ppp_array)
	{
		// Already freed
		goto END;
	}
	for (size_t iii = 0u; iii < arg_count; iii++)
	{
		free((*ppp_array)[iii]);
		(*ppp_array)[iii] = NULL;
	}
	free(*ppp_array);
	*ppp_array = NULL;
END:
	return;
}


size_t strn_countf(const char* const p_str, const size_t max_len,
	should_count_char_f should_count_char)
{
	size_t result = max_len;
	if ((NULL == p_str) || (0u >= max_len) || (NULL == should_count_char))
	{
		goto END;
	}
	result = 0u;
	for (size_t iii = 0u; iii < max_len; iii++)
	{
		const char next_char = p_str[iii];
		const int should_be_counted = should_count_char(next_char);
		if (0 < should_be_counted)
		{
			result++;
		}
		if ('\0' == next_char)
		{
			break;
		}
	}
END:
	return result;
}
size_t str_countf(const char* const p_str,
	should_count_char_f should_count_char)
{
	size_t result = SIZE_MAX;
	if ((NULL == p_str) || (NULL == should_count_char))
	{
		goto END;
	}
	const size_t str_size = strnlen(p_str, INT64_MAX);
	if (INT64_MAX <= str_size)
	{
		goto END;
	}
	result = strn_countf(p_str, str_size + 1u, should_count_char);
END:
	return result;
}


void strn_chr_remap(char* const p_input_str, const size_t input_size,
	chr_remap_f p_map_chr)
{
	if (NULL == p_input_str)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < input_size; iii++)
	{
		const char next_char = p_map_chr(p_input_str[iii], iii);
		p_input_str[iii] = next_char;
	}
END:
	return;
}
void str_chr_remap(char* const p_input_str, chr_remap_f p_map_chr)
{
	if ((NULL == p_input_str) || (NULL == p_map_chr))
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
	// NOP to make -Wpedantic happy.
	(void)char_index;
	return (char)toupper(old_char);
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
	// NOP to make -Wpedantic happy.
	(void)char_index;
	return (char)tolower(old_char);
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
	if ((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_value))
	{
		goto END;
	}
	if (0 >= haystack_size)
	{
		goto END;
	}
	if (needle_size > haystack_size)
	{
		p_result = strndup(p_haystack, haystack_size);
		goto END;
	}
	char* p_tmp = NULL;
	size_t current_size = 0u;
	size_t next_start = 0u;
	size_t tail_length = 0u;
	for (size_t iii = 0u; iii < haystack_size; iii++)
	{
		const char* const p_next = &(p_haystack[iii]);
		if ((haystack_size - iii) < needle_size)
		{
			tail_length = (haystack_size - iii);
			break;
		}
		const int cmp_result = p_strncmp(p_next, p_needle, needle_size);
		if (0 == cmp_result)
		{
			if (NULL == p_result)
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
	if (0u < tail_length)
	{
		current_size += tail_length;
		if (NULL != p_result)
		{
			p_tmp = str_clone_concat(
				p_result, &(p_haystack[haystack_size - tail_length])
			);
			if (NULL != p_tmp)
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
	if ((NULL == p_haystack) || (NULL == p_needle) || (NULL == p_value))
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
	if ((NULL == pp_buffer) || (NULL == p_buffer_size))
	{
		goto END;
	}
	if (0u >= *p_buffer_size)
	{
		goto END;
	}
	size_t string_length = strnlen((char*)*pp_buffer, *p_buffer_size);
	if (string_length >= *p_buffer_size)
	{
		goto END;
	}
	p_result = calloc(string_length + 1u, sizeof(char));
	if (NULL == p_result)
	{
		goto END;
	}
	strncpy(p_result, (const char*)(*pp_buffer), string_length + 1u);
	p_result[string_length] = '\0';
	const size_t non_str_len = *p_buffer_size - (string_length + 1u);
	if ((0u < non_str_len) && ((string_length + 1u) < *p_buffer_size))
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

char* vstr_format(const char* const p_format, va_list args)
{
	char* p_result = NULL;
	if (NULL == p_format)
	{
		goto END;
	}
#ifdef _GNU_SOURCE
	const int heap_print_result = vasprintf(&p_result, p_format, args);
	if (0 > heap_print_result)
	{
		perror("vasprintf failure");
		goto END;
	}
#else
	va_list arg_clone = {0};
	va_copy(arg_clone, args);
	const int result_size = vsnprintf(NULL, 0, p_format, arg_clone);
	va_end(arg_clone);
	if (0 > result_size)
	{
		fprintf(stderr, "vsnprintf() returned invalid size.\n");
		goto END;
	}

	p_result = calloc(result_size + 1u, sizeof(char));
	if (NULL == p_result)
	{
		fprintf(stderr, "calloc() failed allocate size: %d.\n", result_size);
		goto END;
	}

	const int printed_size = vsnprintf(
		p_result, result_size + 1u, p_format, args
	);
	p_result[result_size] = '\0';

	if (printed_size != result_size)
	{
		fprintf(
			stderr, "vsnprintf() %d expected %d.\n",
			printed_size, result_size
		);
		goto END;
	}
#endif//_GNU_SOURCE
END:
	return p_result;
}
char* str_format(const char* const p_format, ...)
{
	char* p_result = NULL;
	if (NULL == p_format)
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_format);
	p_result = vstr_format(p_format, args);
	va_end(args);
END:
	return p_result;
}

char* str_from_fprint(str_fprint_f fprint_f, const void* const p_value, ...)
{
	char* p_buffer = NULL;
	if ((NULL == fprint_f) || (NULL == p_value))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_value);
#ifdef __linux__
	size_t buffer_size = 0u;
	FILE* const p_file = open_memstream(&p_buffer, &buffer_size);
	if (NULL == p_file)
	{
		goto CLEAN;
	}
	const int fprint_result = fprint_f(p_file, p_value, args);
	const int flush_result = fflush(p_file);
	(void)fclose(p_file);
	if ((0 > fprint_result) || (0 > flush_result))
	{
		goto CLEAN;
	}
	p_buffer[fprint_result] = '\0';
	goto END;
#else
#warning Unsupported/Unknown OS
#endif//__linux__
CLEAN:
	va_end(args);
	if (NULL != p_buffer)
	{
		free(p_buffer);
		p_buffer = NULL;
	}
END:
	return p_buffer;
}
