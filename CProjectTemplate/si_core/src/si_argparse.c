// si_argparse.c
#include "si_argparse.h"

void si_arg_init(si_arg_t* const p_arg)
{
	if (NULL == p_arg)
	{
		goto END;
	}
	p_arg->is_optional = SI_ARGPARSE_DEFAULT_OPTIONAL;
	p_arg->p_full = NULL;
	p_arg->flag = '\0';
	p_arg->p_help = NULL;
	p_arg->minimum_values = 0u;
	p_arg->maximum_values = 0u;
	p_arg->p_values = NULL;
	p_arg->p_validate = NULL;
	p_arg->p_free_value = NULL;
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
		if (p_arg->is_optional)
		{
			// Not required and no values (which is fine)
			result = true;
			goto END;
		}
		goto END;
	}
	if ((p_arg->minimum_values > p_arg->p_values->capacity) ||
	    (p_arg->maximum_values < p_arg->p_values->capacity))
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_arg->p_values->capacity; iii++)
	{
		void** pp_value = si_array_at(p_arg->p_values, iii);
		if (NULL == pp_value)
		{
			goto END;
		}
		if (NULL != p_arg->p_validate)
		{
			const bool is_valid = p_arg->p_validate(*pp_value);
			if (false == is_valid)
			{
				goto END;
			}
		}
		else
		{
			// Simple default validation
			if (NULL == *pp_value)
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
	// Direct string compare(s)
	if (NULL != p_arg->p_full)
	{
		full_len = strnlen(p_arg->p_full, __INT_MAX__);
		if(full_len == str_len)
		{
			cmp = strncmp(p_arg->p_full, p_str, full_len);
		}
	}
	if (0 == cmp)
	{
		result = true;
		goto END;
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
		goto END;
	}
	const bool did_grow = si_array_resize(
		p_arg->p_values, p_arg->p_values->capacity + 1u
	);
	if (false == did_grow)
	{
		goto END;
	}
	si_array_set(p_arg->p_values, p_arg->p_values->capacity - 1u, &p_value);
	result = true;
END:
	return result;
}

void si_arg_fprint_id(const si_arg_t* const p_arg, FILE* const p_file)
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

void si_arg_fprint(const si_arg_t* const p_arg, FILE* const p_file)
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
	si_arg_fprint_id(p_arg, p_file);
	// Print value fields
	const size_t value_delta = (p_arg->maximum_values - p_arg->minimum_values);
	if ((0u < p_arg->minimum_values) || (0u < p_arg->maximum_values))
	{
		fprintf(p_file, " ");
	}
	if (0u < p_arg->minimum_values)
	{
		fprintf(p_file, "[v");
		if (1u < p_arg->maximum_values)
		{
			fprintf(p_file, "0");
		}
		if (1u < p_arg->minimum_values)
		{
			if (2u < p_arg->minimum_values)
			{
				fprintf(p_file, "]...[v%lu", p_arg->minimum_values - 1u);
			}
			else
			{
				fprintf(p_file, "] [v%lu", p_arg->minimum_values - 1u);
			}
		}
		fprintf(p_file, "]");
	}
	// Print optional fields
	if (0u != value_delta)
	{
		fprintf(p_file, "(");
		if (0u < p_arg->minimum_values)
		{
			if (1u < value_delta)
			{
				fprintf(p_file, "...");
			}
		}
		fprintf(p_file, "[v");
		if (0u < p_arg->minimum_values)
		{
			fprintf(p_file, "%lu", p_arg->maximum_values - 1u);
		}
		else
		{
			if (1u < p_arg->maximum_values)
			{
				fprintf(p_file, "0");
			}
		}
		fprintf(p_file, "]");
		if (0u >= p_arg->minimum_values)
		{
			if (2u < p_arg->maximum_values)
			{
				fprintf(p_file, "...");
			}
			if (1u < p_arg->maximum_values)
			{
				fprintf(p_file, "[v%lu]", p_arg->maximum_values - 1u);
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
	if ((NULL != p_arg->p_values) && (NULL != p_arg->p_free_value))
	{
		for (size_t iii = 0u; iii < p_arg->p_values->capacity; iii++)
		{
			void** pp_value = si_array_at(p_arg->p_values, iii);
			if (NULL != pp_value)
			{
				p_arg->p_free_value(*pp_value);
			}
		}
	}
	if (NULL != p_arg->p_values)
	{
		si_array_free(p_arg->p_values);
		free(p_arg->p_values);
		p_arg->p_values = NULL;
	}
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


void si_argparse_init(si_argparse_t* const p_parse)
{
	if (NULL == p_parse)
	{
		goto END;
	}
	p_parse->p_prefix = NULL;
	p_parse->p_program_name = NULL;
	p_parse->p_description = NULL;
	p_parse->p_suffix = NULL;
	si_array_init_3(&(p_parse->arguments), sizeof(si_arg_t), 0u);
END:
	return;
}

si_argparse_t* si_argparse_new()
{
	si_argparse_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_argparse_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_argparse_init(p_new);
END:
	return p_new;
}

bool si_argparse_is_valid(const si_argparse_t* const p_parse)
{
	bool result = false;
	if (NULL == p_parse)
	{
		goto END;
	}
	const size_t count = si_argparse_count(p_parse);
	if (SIZE_MAX == count)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < count; iii++)
	{
		si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			goto END;
		}
		const bool is_valid = si_arg_is_valid(p_arg);
		if (false == is_valid)
		{
			goto END;
		}
	}
	result = true;
END:
	return result;
}

bool si_argparse_is_valid_values(const si_argparse_t* const p_parse)
{
	bool result = false;
	if (NULL == p_parse)
	{
		goto END;
	}
	const size_t count = si_argparse_count(p_parse);
	if (SIZE_MAX == count)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < count; iii++)
	{
		si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			goto END;
		}
		const bool is_valid = si_arg_is_valid_values(p_arg);
		if (false == is_valid)
		{
			goto END;
		}
	}
	result = true;
END:
	return result;
}

bool si_argparse_add_argument(si_argparse_t* const p_parse,
	const si_arg_t* const p_arg)
{
	bool result = false;
	if (NULL == p_parse)
	{
		goto END;
	}
	const bool basic_valid = si_arg_is_valid(p_arg);
	if (false == basic_valid)
	{
		goto END;
	}
	const bool did_grow = si_array_resize(
		&(p_parse->arguments), p_parse->arguments.capacity + 1u
	);
	if (false == did_grow)
	{
		goto END;
	}
	si_array_set(
		&(p_parse->arguments),
		p_parse->arguments.capacity - 1u,
		p_arg
	);
	result = true;
END:
	return result;
}

si_arg_t* si_argparse_at(si_argparse_t* const p_parse,
	const char* const p_id)
{
	si_arg_t* p_result = NULL;
	if (NULL == p_parse)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
	{
		p_result = si_array_at(&(p_parse->arguments), iii);
		const bool is_match = si_arg_matches(p_result, p_id);
		if (true == is_match)
		{
			goto END;
		}
	}
	p_result = NULL;
END:
	return p_result;
}

bool si_argparse_is_set(si_argparse_t* const p_parse,
	const char* const p_id)
{
	bool result = false;
	if((NULL == p_parse) || (NULL == p_id))
	{
		goto END;
	}
	const si_arg_t* const p_arg = si_argparse_at(p_parse, p_id);
	if(NULL == p_arg)
	{
		goto END;
	}
	result = si_arg_is_set(p_arg);
END:
	return result;
}

size_t si_argparse_count_optional(const si_argparse_t* const p_parse)
{
	size_t result = SIZE_MAX;
	if (NULL == p_parse)
	{
		goto END;
	}
	result++;
	if (NULL == p_parse->arguments.p_data)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
	{
		si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			break;
		}
		if (false != p_arg->is_optional)
		{
			result++;
		}
	}
END:
	return result;
}

size_t si_argparse_count_required(const si_argparse_t* const p_parse)
{
	size_t result = SIZE_MAX;
	if (NULL == p_parse)
	{
		goto END;
	}
	result = si_argparse_count(p_parse);
	if (SIZE_MAX == result)
	{
		goto END;
	}
	result -= si_argparse_count_optional(p_parse);
END:
	return result;
}

size_t si_argparse_count(const si_argparse_t* const p_parse)
{
	size_t result = SIZE_MAX;
	if (NULL == p_parse)
	{
		goto END;
	}
	result = p_parse->arguments.capacity;
END:
	return result;
}

/** Doxygen
 * @brief Finds the next required unset argument at or after index in p_parse.
 * 
 * @param p_parse Pointer to the argparse struct to get args from.
 * @param index Where in the arguments array to start from.
 * 
 * @return Returns si_arg_t on success. Returns NULL otherwise.
 */
static si_arg_t* si_argparse_next_required_arg_at(si_argparse_t* const p_parse,
	const size_t index)
{
	si_arg_t* p_result = NULL;
	if (NULL == p_parse)
	{
		goto END;
	}
	for (size_t iii = index; iii < p_parse->arguments.capacity; iii++)
	{
		si_arg_t* const p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			goto END;
		}
		if (true == p_arg->is_optional)
		{
			continue;
		}
		if (NULL != p_arg->p_values)
		{
			continue;
		}
		p_result = p_arg;
		break;
	}
END:
	return p_result;
}

bool si_argparse_parse(si_argparse_t* const p_parse,
	const int argc, char** const pp_argv)
{
	bool result = false;
	if ((NULL == p_parse) || (NULL == pp_argv))
	{
		goto END;
	}
	if (1 > argc)
	{
		goto END;
	}
	if (NULL == p_parse->p_program_name)
	{
		p_parse->p_program_name = strdup(pp_argv[0u]);
		if (NULL == p_parse->p_program_name)
		{
			goto END;
		}
		p_parse->p_free_program_name = free;
	}
	size_t required_counter = 0u;
	for (int iii = 1; iii < argc; iii++)
	{
		const char* p_next = pp_argv[iii];
		if (NULL == p_next)
		{
			continue;
		}
		si_arg_t* p_arg = si_argparse_at(p_parse, p_next);
		size_t offset = 0u;
		if (NULL == p_arg)
		{
			p_arg = si_argparse_next_required_arg_at(p_parse, required_counter);
			if (NULL == p_arg)
			{
				fprintf(stderr, "Unhandled/Unknown positional parameter: '%s'.\n", p_next);
				goto END;
			}
			required_counter++;
		}
		else
		{
			offset = 1u;
			if (NULL == p_arg->p_values)
			{
				p_arg->p_values = si_array_new(sizeof(void*));
			}
			if (!p_arg->is_optional)
			{
				required_counter++;
			}
		}
		for (size_t jjj = 0u; jjj < p_arg->maximum_values; jjj++)
		{
			if (argc <= iii + jjj + offset)
			{
				if (jjj < p_arg->minimum_values)
				{
					fprintf(stderr, "Reached the end of parameter list "
						"missing required parameters for argument: '"
					);
					si_arg_fprint_id(p_arg, stderr);
					fprintf(stderr, "'.\n");
					goto END;
				}
				break;
			}
			p_next = pp_argv[iii + jjj + offset];
			void* p_parsed_value = NULL;
			if (NULL != p_arg->p_parser)
			{
				p_parsed_value = p_arg->p_parser(p_next);
			}
			else
			{
				// No parsing is done. Value is C String
				p_parsed_value = (char*)p_next;
			}
			if (NULL == p_parsed_value)
			{
				fprintf(stderr, "Failed to parse parameter value: '%s' for '", p_next);
				si_arg_fprint_id(p_arg, stderr);
				fprintf(stderr, "'.\n");
				goto END;
			}
			if (NULL == p_arg->p_values)
			{
				p_arg->p_values = si_array_new(sizeof(void*));
			}
			bool const added_value = si_arg_append_value(p_arg, p_parsed_value);
			if (false == added_value)
			{
				fprintf(stderr, "Failed to add parsed item '%s' to argument '", p_next);
				si_arg_fprint_id(p_arg, stderr);
				fprintf(stderr, "' values.\n");
			}
		}
		if (NULL != p_arg->p_values)
		{
			iii += p_arg->p_values->capacity - (!offset);
		}
	}
	result = (required_counter == si_argparse_count_required(p_parse));
END:
	return result;
}

void si_argparse_fprint_help(si_argparse_t* const p_parse, FILE* const p_file)
{
	if ((NULL == p_parse) || (NULL == p_file))
	{
		goto END;
	}
	if (NULL != p_parse->p_prefix)
	{
		fprintf(p_file, "%s", p_parse->p_prefix);
	}
	if (NULL != p_parse->p_program_name)
	{
		fprintf(p_file, "%s", basename((char*)p_parse->p_program_name));
		if (NULL != p_parse->p_description)
		{
			fprintf(p_file, " -\n\t%s", p_parse->p_description);
		}
		fprintf(p_file, "\n");
	}
	else
	{
		// Default program
		fprintf(p_file, "\tProgram - an executable description.\n");
	}
	const size_t required_count = si_argparse_count_required(p_parse);
	if (NULL != p_parse->p_program_name)
	{
		fprintf(p_file, "Minimum usage example:\n\t%s", p_parse->p_program_name);
		for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
		{
			si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
			if (NULL == p_arg)
			{
				break;
			}
			if (!p_arg->is_optional)
			{
				if (NULL != p_arg->p_full)
				{
					fprintf(p_file, " <%s>", p_arg->p_full);
				}
				else
				{
					fprintf(p_file, " <%c>", p_arg->flag);
				}
			}
		}
		fprintf(p_file, "\n");
	}
	if (0u < required_count)
	{
		fprintf(p_file, "Required arguments:\n");
		for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
		{
			si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
			if (NULL == p_arg)
			{
				break;
			}
			if (!p_arg->is_optional)
			{
				fprintf(p_file, "\t");
				si_arg_fprint(p_arg, p_file);
				fprintf(p_file, "\n");
			}
		}
	}
	const size_t optional_count = si_argparse_count(p_parse) - required_count;
	if (0u < optional_count)
	{
		fprintf(p_file, "Optional arguments:\n");
		for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
		{
			si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
			if (NULL == p_arg)
			{
				break;
			}
			if (p_arg->is_optional)
			{
				fprintf(p_file, "\t");
				si_arg_fprint(p_arg, p_file);
				fprintf(p_file, "\n");
			}
		}
	}
	if (NULL != p_parse->p_suffix)
	{
		fprintf(p_file, "%s\n", p_parse->p_suffix);
	}
	else
	{
		// Default suffix
		fprintf(p_file, "\nOutput was generated by si_argparse\n");
	}
END:
	return;
}

void si_argparse_fprint_error(si_argparse_t* const p_parse, FILE* const p_file)
{
	if ((NULL == p_parse) || (NULL == p_file))
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
	{
		si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			break;
		}
		const bool arg_valid = si_arg_is_valid_values(p_arg);
		if (true != arg_valid)
		{
			fprintf(p_file, "Invalid or missing required argument: '");
			si_arg_fprint_id(p_arg, p_file);
			fprintf(p_file, "'.\n");
		}
	}
END:
	return;
}

void si_argparse_free(si_argparse_t* const p_parse)
{
	if (NULL == p_parse)
	{
		goto END;
	}
	if ((NULL != p_parse->p_program_name) &&
	    (NULL != p_parse->p_free_program_name))
	{
		p_parse->p_free_program_name((void*)p_parse->p_program_name);
		p_parse->p_program_name = NULL;
	}
	for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
	{
		si_arg_t* p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL != p_arg)
		{
			si_arg_free(p_arg);
		}
	}
	si_array_free(&(p_parse->arguments));
END:
	return;
}

void si_argparse_destroy(si_argparse_t** pp_parse)
{
	if (NULL == pp_parse)
	{
		goto END;
	}
	if (NULL == *pp_parse)
	{
		goto END;
	}
	si_argparse_free(*pp_parse);
	free(*pp_parse);
	*pp_parse = NULL;
END:
	return;
}
