// si_arg.c
#include "si_arg.h"

void si_arg_init(si_arg_t* const p_arg)
{
	if (NULL == p_arg)
	{
		goto END;
	}
	p_arg->bit_flags = (SI_ARG_DEFAULT_IS_OPTIONAL | SI_ARG_DEFAULT_PROMPTS);
	p_arg->p_full = NULL;
	p_arg->flag = '\0';
	p_arg->p_help = NULL;
	p_arg->minimum_values = 0;
	p_arg->maximum_values = 0;
	p_arg->p_values = NULL;
	p_arg->p_validate = NULL;
END:
	return;
}

si_arg_t* si_arg_new()
{
	si_arg_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_arg_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_arg_init(p_new);
END:
	return p_new;
}

bool si_arg_is_valid(const si_arg_t* const p_arg)
{
	bool result = false;
	if (NULL == p_arg)
	{
		goto END;
	}
	if ((NULL == p_arg->p_full) && ('\0' == p_arg->flag))
	{
		goto END;
	}
	if (p_arg->maximum_values < p_arg->minimum_values)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

bool si_arg_is_valid_values(const si_arg_t* const p_arg)
{
	bool result = false;
	const bool basic_valid = si_arg_is_valid(p_arg);
	if (false == basic_valid)
	{
		goto END;
	}
	if (NULL == p_arg->p_values)
	{
		const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
		if (true == is_optional)
		{
			// Not required and no values (which is fine)
			result = true;
			goto END;
		}
		goto END;
	}
	const size_t value_count = si_parray_count(p_arg->p_values);
	const size_t min_values = (
		(0 > p_arg->minimum_values) ? 0u : (size_t)p_arg->minimum_values
	);
	const size_t max_values = (
		(0 > p_arg->maximum_values) ? 0u : (size_t)p_arg->maximum_values
	);
	if ((min_values > value_count) ||
	    (max_values < value_count))
	{
		goto END;
	}
	for (size_t iii = 0u; iii < value_count; iii++)
	{
		void* const p_value = si_parray_at(p_arg->p_values, iii);
		// Simple Validation
		if (NULL == p_value)
		{
			goto END;
		}
		if (NULL != p_arg->p_validate)
		{
			const bool is_valid = p_arg->p_validate(p_value);
			if (false == is_valid)
			{
				goto END;
			}
		}
	}
	result = true;
END:
	return result;
}

bool si_arg_is_set(const si_arg_t* const p_arg)
{
	bool result = false;
	if (NULL == p_arg)
	{
		goto END;
	}
	result = (NULL != p_arg->p_values);
END:
	return result;
}

bool si_arg_matches(const si_arg_t* const p_arg, const char* p_str)
{
	bool result = false;
	if ((NULL == p_arg) || (NULL == p_str))
	{
		goto END;
	}
	// String lengths
	size_t       full_len = 0u;
	const size_t str_len  = strnlen(p_str, __INT_MAX__);
	
	int cmp = -1;
	// Direct full string compare
	if (NULL != p_arg->p_full)
	{
		full_len = strnlen(p_arg->p_full, __INT_MAX__);
		if (full_len == str_len)
		{
			cmp = strncmp(p_arg->p_full, p_str, full_len);
		}
	}
	if (0 == cmp)
	{
		result = true;
		goto END;
	}
	// Direct flag compare
	if (str_len == 1u)
	{
		if (p_arg->flag == p_str[0])
		{
			result = true;
			goto END;
		}
	}

	// Handles (-/--) formats
	if (2u > str_len)
	{
		goto END;
	}
	if ('-' != p_str[0u])
	{
		goto END;
	}
	if (('-' != p_str[1u]) && ('\0' != p_arg->flag))
	{
		// Handle flags (-?)
		// strnchr() doesn't exist, so using memchr() instead.
		char* p_match = (char*)memchr(&p_str[1u], p_arg->flag, (str_len - 1u));
		if (NULL != p_match)
		{
			result = true;
			goto END;
		}
	}
	if (('-' == p_str[1u]) && (NULL != p_arg->p_full))
	{
		// Handle fulls (--help)
		if (full_len != (str_len - 2u))
		{
			goto END;
		}
		cmp = strncmp(p_arg->p_full, &p_str[2u], str_len - 2u);
	}
	if (0 == cmp)
	{
		result = true;
		goto END;
	}
END:
	return result;
}

bool si_arg_append_value(si_arg_t* const p_arg, const void* const p_value)
{
	bool result = false;
	if (NULL == p_arg)
	{
		goto END;
	}
	if (NULL == p_arg->p_values)
	{
		p_arg->p_values = si_parray_new();
		result = (NULL == p_arg->p_values);
		if (true != result)
		{
			goto END;
		}
		p_arg->p_values->p_free_value = free;
	}
	const size_t value_index = si_parray_append(p_arg->p_values, p_value);
	result = (SIZE_MAX != value_index);
END:
	return result;
}

size_t si_arg_prompt(si_arg_t* const p_arg)
{
	size_t result = 0u;
	if (NULL == p_arg)
	{
		goto END;
	}
	size_t value_count = 0u;
	if (NULL != p_arg->p_values)
	{
		value_count = si_parray_count(p_arg->p_values);
	}
	if (INT_MAX <= value_count)
	{
		goto END;
	}
	const int value_count_i = (int)value_count;
	const int loops         = (p_arg->minimum_values - value_count_i);
	if (0 >= loops)
	{
		goto END;
	}
	const size_t loops_s = (size_t)loops;

	size_t read_size      = 0u;
	char*  p_input_line   = NULL;
	void*  p_parsed_value = NULL;

	while (result < loops_s)
	{
		// Prompt user for input.
		fprintf(stdout, "Please enter value for argument '");
		si_arg_fprint_id(stdout, p_arg);
		fprintf(stdout, "%s", "': ");

		// Read the user's response
		p_input_line = fread_alloc_line(stdin, &read_size);

		// Parse input string
		if (NULL != p_arg->p_parser)
		{
			p_parsed_value = p_arg->p_parser(p_input_line);
		}
		else
		{
			p_parsed_value = strndup(p_input_line, read_size);
		}
		if (NULL == p_parsed_value)
		{
			fprintf(stdout,
				"Failed to parse input value: '%s'.\n", p_input_line);
			goto BAD_INPUT;
		}

		// Validate parsed value
		bool is_valid = true;
		if (NULL != p_arg->p_validate)
		{
			is_valid = p_arg->p_validate(p_parsed_value);
		}
		if (true != is_valid)
		{
			fprintf(stdout, "Input '%s' was invalid.\n", p_input_line);
			goto BAD_INPUT;
		}
		free((void*)p_input_line);
		p_input_line = NULL;

		// Append value to value list.
		bool was_added = si_arg_append_value(p_arg, p_parsed_value);
		if (true != was_added)
		{
			// Failed to add valid parsed value to argument list.
			free((void*)p_parsed_value);
			p_parsed_value = NULL;
			break;
		}
		result++;
		continue;
BAD_INPUT:
		free((void*)p_input_line);
		p_input_line = NULL;
		free((void*)p_parsed_value);
		p_parsed_value = NULL;
	}
END:
	return result;
}

size_t si_arg_parse(si_arg_t* const p_arg,
	const int argc, const char** const pp_argv)
{
	size_t result = 0u;
	if ((NULL == p_arg) || (0 >= argc) || (NULL == pp_argv))
	{
		goto END;
	}
	// Determine if the first argument is an id string.
	int value_read_offset = 0;
	const bool zero_is_id = si_arg_matches(p_arg, pp_argv[0]);
	if (true == zero_is_id)
	{
		value_read_offset++;
	}
	// For each value string: parse, validate, and append all(up to max)
	const int max = p_arg->maximum_values;
	const int loops = max > argc ? argc : max;
	for (int iii = value_read_offset; iii < loops; iii++)
	{
		const char* p_next_str = pp_argv[iii];
		if (NULL == p_next_str)
		{
			break;
		}

		// Parse the string into a type.
		void* p_parse_result = NULL;
		if (NULL != p_arg->p_parser)
		{
			p_parse_result = p_arg->p_parser(p_next_str);
		}
		else
		{
			// No parsing result is the C String.
			p_parse_result = (void*)p_next_str;
		}
		if (NULL == p_parse_result)
		{
			// Parse Failed.
			break;
		}

		// Validate the type before appending
		if (NULL != p_arg->p_validate)
		{
			const bool is_valid = p_arg->p_validate(p_parse_result);
			if (true != is_valid)
			{
				// Invalid parsed value. Free it if needed and break.
				if (NULL != p_arg->p_parser)
				{
					free(p_parse_result);
				}
				break;
			}
		}

		// Add the validated parsed value to the argument's values list.
		const bool was_appended = si_arg_append_value(p_arg, p_parse_result);
		if (true != was_appended)
		{
			// Failed to append the parsed value to the argument's value list.
			break;
		}
		result++;
	}

	// Check if we need to prompt and if prompting is enabled.
	const bool values_are_valid = si_arg_is_valid_values(p_arg);
	const bool do_prompt = (
		SI_ARG_DOES_PROMPT(p_arg->bit_flags) && (true != values_are_valid)
	);
	if (true == do_prompt)
	{
		printf("ai_arg_parse() calling arg_prompt()!\n");//!Debugging
		result += si_arg_prompt(p_arg);
	}
END:
	return result;
}

void si_arg_fprint_id(FILE* const p_file, const si_arg_t* const p_arg)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const bool basic_valid = si_arg_is_valid(p_arg);
	if (false == basic_valid)
	{
		goto END;
	}
	// Start of printing
	if ('\0' != p_arg->flag)
	{
		fprintf(p_file, "-%c", p_arg->flag);
		if (NULL != p_arg->p_full)
		{
			fprintf(p_file, ", ");
		}
	}
	if (NULL != p_arg->p_full)
	{
		fprintf(p_file, "--%s", p_arg->p_full);
	}
END:
	return;
}

void si_arg_fprint(FILE* const p_file, const si_arg_t* const p_arg)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const bool basic_valid = si_arg_is_valid(p_arg);
	if (false == basic_valid)
	{
		goto END;
	}
	// Start of printing
	si_arg_fprint_id(p_file, p_arg);
	// Print value fields
	const int value_delta = (p_arg->maximum_values - p_arg->minimum_values);
	if ((0 < p_arg->minimum_values) || (0 < p_arg->maximum_values))
	{
		fprintf(p_file, " ");
	}
	if (0 < p_arg->minimum_values)
	{
		fprintf(p_file, "[v");
		if (1 < p_arg->maximum_values)
		{
			fprintf(p_file, "0");
		}
		if (1 < p_arg->minimum_values)
		{
			if (2 < p_arg->minimum_values)
			{
				fprintf(p_file, "]...[v%d", p_arg->minimum_values - 1);
			}
			else
			{
				fprintf(p_file, "] [v%d", p_arg->minimum_values - 1);
			}
		}
		fprintf(p_file, "]");
	}
	// Print optional fields
	if (0 != value_delta)
	{
		fprintf(p_file, "(");
		if (0 < p_arg->minimum_values)
		{
			if (1 < value_delta)
			{
				fprintf(p_file, "...");
			}
		}
		fprintf(p_file, "[v");
		if (0 < p_arg->minimum_values)
		{
			fprintf(p_file, "%d", p_arg->maximum_values - 1);
		}
		else
		{
			if (1 < p_arg->maximum_values)
			{
				fprintf(p_file, "0");
			}
		}
		fprintf(p_file, "]");
		if (0 >= p_arg->minimum_values)
		{
			if (2 < p_arg->maximum_values)
			{
				fprintf(p_file, "...");
			}
			if (1 < p_arg->maximum_values)
			{
				fprintf(p_file, "[v%d]", p_arg->maximum_values - 1);
			}
		}
		fprintf(p_file, ")");
	}
	// Print help/description
	fprintf(p_file, " -\t ");
	if (NULL == p_arg->p_help)
	{
		fprintf(p_file, "A program argument");
	}
	else
	{
		fprintf(p_file, "%s", p_arg->p_help);
	}
END:
	return;
}

void si_arg_free(si_arg_t* const p_arg)
{
	if (NULL == p_arg)
	{
		goto END;
	}
	si_parray_destroy(&(p_arg->p_values));
END:
	return;
}

void si_arg_destroy(si_arg_t** pp_arg)
{
	if (NULL == pp_arg)
	{
		goto END;
	}
	if (NULL == *pp_arg)
	{
		// Already freed
		goto END;
	}
	si_arg_free(*pp_arg);
	free(*pp_arg);
	*pp_arg = NULL;
END:
	return;
}