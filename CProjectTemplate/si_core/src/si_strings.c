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
inline char* str_clone_concat(const char* const p_left,
	const char* const p_right)
{
	// Defaults to strnlen(p_str, INT64_MAX) for string size.
	// *WARNING* This function still requres both C Strings to be properly
	//           NULL-Terminated.
	return strn_clone_concat(
		p_left, strnlen(p_left, INT64_MAX),
		p_right, strnlen(p_right, INT64_MAX)
	);
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
			p_result = calloc(new_size, sizeof(char));
			if(NULL == p_result)
			{
				goto END;
			}
		}
	}
END:
	return p_result;
}

char* strv_clone_join(const size_t argc, const char* const p_seperator, ...)
{
	char* p_result = NULL;
	if(0u >= argc)
	{
		goto END;
	}
	const char** pp_argv = calloc(argc, sizeof(char*));
	if(NULL == pp_argv)
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_seperator);
	for(size_t iii = 0u; iii < argc; iii++)
	{
		pp_argv[iii] = va_arg(args, char*);
	}
	p_result = str_clone_join(argc, p_seperator, pp_argv);
	free(pp_argv);
	pp_argv = NULL;
	va_end(args);
END:
	return p_result;
}

void strn_to_uppercase(char* const p_input_str, const size_t input_size)
{
	if(NULL == p_input_str)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < input_size; iii++)
	{
		char next_char = p_input_str[iii];
		p_input_str[iii] = toupper(next_char);
	}
END:
	return;
}
inline void str_to_uppercase(char* const p_input_str)
{
	// Defaults to strnlen(p_str, INT64_MAX) for string size.
	// *WARNING* This function still requres the C String to be properly
	//           NULL-Terminated.
	return strn_to_uppercase(p_input_str, strnlen(p_input_str, INT64_MAX));
}

void strn_to_lowercase(char* const p_input_str, const size_t input_size)
{
	if(NULL == p_input_str)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < input_size; iii++)
	{
		char next_char = p_input_str[iii];
		p_input_str[iii] = tolower(next_char);
	}
END:
	return;
}
void str_to_lowercase(char* const p_input_str)
{
	// Defaults to strnlen(p_str, INT64_MAX) for string size.
	// *WARNING* This function still requres the C String to be properly
	//           NULL-Terminated.
	return strn_to_lowercase(p_input_str, strnlen(p_input_str, INT64_MAX));
}
