// si_argparse.c
#include "si_argparse.h"

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

size_t si_argparse_is_stopped(const si_argparse_t* const p_parse)
{
	size_t result = SIZE_MAX;
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
		const bool is_stopping = si_arg_is_stopping(p_arg);
		if (true == is_stopping)
		{
			result = iii;
			break;
		}
	}
END:
	return result;
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
		const bool is_stopping = si_arg_is_stopping(p_arg);
		if (true == is_stopping)
		{
			break;
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
	if ((NULL == p_parse) || (NULL == p_id))
	{
		goto END;
	}
	const si_arg_t* const p_arg = si_argparse_at(p_parse, p_id);
	if (NULL == p_arg)
	{
		goto END;
	}
	result = si_arg_is_set(p_arg);
END:
	return result;
}

void* si_argparse_value_of_3(si_argparse_t* const p_parse,
	const char* const p_id, const size_t value_index)
{
	void* p_result = NULL;
	if ((NULL == p_parse) || (NULL == p_id))
	{
		goto END;
	}
	const si_arg_t* const p_arg = si_argparse_at(p_parse, p_id);
	if (NULL == p_arg)
	{
		goto END;
	}
	if (NULL == p_arg->p_values)
	{
		goto END;
	}
	p_result = si_parray_at(p_arg->p_values, value_index);
	if (NULL == p_result)
	{
		// Out of bounds or array was invalid.
		goto END;
	}
END:
	return p_result;
}
inline void* si_argparse_value_of(si_argparse_t* const p_parse,
	const char* const p_id)
{
	// Default value of value_index is (0u)
	return si_argparse_value_of_3(p_parse, p_id, 0u);
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
		const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
		if (false != is_optional)
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
 * @brief During parsing prompt for any missing argument values.
 * 
 * @param p_parse Pointer to the si_argparse_t to prompt user for.
 */
static void si_argparse_prompt_all(si_argparse_t* const p_parse)
{
	if (NULL == p_parse)
	{
		goto END;
	}
	const size_t stopped_index = si_argparse_is_stopped(p_parse);
	const bool is_stopped = (SIZE_MAX != stopped_index);
	if (true == is_stopped)
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
		const bool is_valid  = si_arg_is_valid_values(p_arg);
		if (true != is_valid)
		{
			(void)si_arg_prompt(p_arg);
		}
	}
END:
	return;
}

/** Doxygen
 * @brief Finds the next required unset argument in p_parse.
 * 
 * @param p_parse Pointer to the argparse struct to get args from.
 * 
 * @return Returns si_arg_t on success. Returns NULL otherwise.
 */
static si_arg_t* si_argparse_next_required_arg(si_argparse_t* const p_parse)
{
	si_arg_t* p_result = NULL;
	if (NULL == p_parse)
	{
		goto END;
	}
	for (size_t iii = 0u; iii < p_parse->arguments.capacity; iii++)
	{
		si_arg_t* const p_arg = si_array_at(&(p_parse->arguments), iii);
		if (NULL == p_arg)
		{
			goto END;
		}
		const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
		const bool is_set = si_arg_is_set(p_arg);
		if ((true != is_optional) && (true != is_set))
		{
			p_result = p_arg;
			break;
		}
	}
END:
	return p_result;
}

bool si_argparse_parse(si_argparse_t* const p_parse,
	const int argc, const char** const pp_argv)
{
	bool result = false;
	if ((NULL == p_parse) || (1 > argc) || (NULL == pp_argv))
	{
		// Expects at least one argument for the program name.
		goto END;
	}
	int arg_start_index = 0;
	// Handle program name
	if (NULL == p_parse->p_program_name)
	{
		p_parse->p_program_name = pp_argv[arg_start_index];
		arg_start_index++;
	}
	char* p_arg_id = NULL;
	while (arg_start_index < argc)
	{
		const char* const p_next_str = pp_argv[arg_start_index];
		// Try to find an argument by an id string E.G. (-?/--help)
		si_arg_t* p_arg = si_argparse_at(p_parse, p_next_str);
		if (NULL != p_arg)
		{
			// We have an ID string so we know that this is intended argument.
			arg_start_index++;
			p_arg_id = str_from_fprint(
				(str_fprint_f)si_arg_fprint_id, p_arg
			);
			if (NULL == p_arg->p_values)
			{
				p_arg->p_values = si_parray_new();
			}
			if (NULL == p_arg->p_values)
			{
				fprintf(
					stderr,
					"Failed to allocate value buffer for argument '%s'.\n",
					p_arg_id
				);
				break;
			}
			p_arg->p_values->p_free_value = free;
		}
		else
		{
			// Either not an ID string or the id was not found.
			// We will try the next required positional argument for parsing.
			p_arg = si_argparse_next_required_arg(p_parse);
			if (NULL == p_arg)
			{
				// This too has failed. This is an unexpected argument(?)
				fprintf(
					stderr,
					"Unhandled/Unknown positional parameter: '%s'.\n",
					p_next_str
				);
				break;
			}
			p_arg_id = str_from_fprint(
				(str_fprint_f)si_arg_fprint_id, p_arg
			);
		}
		// At this point we have identified the argument we want to parse.
		const size_t min_values = (
			(0 > p_arg->minimum_values) ? 0u : (size_t)p_arg->minimum_values
		);
		size_t values_parsed = si_arg_parse(
			p_arg, argc - arg_start_index, &(pp_argv[arg_start_index])
		);
		// Handle Overflows
		if (INT_MAX <= (values_parsed + ((size_t)arg_start_index)))
		{
			break;
		}
		arg_start_index += (int)values_parsed;
		if (values_parsed < min_values)
		{
			fprintf(
				stderr, "Argument: '%s' expected at least: %lu values and got: %lu "
				"values instead.\n", p_arg_id, min_values, values_parsed
			);
			result = false;
			si_parray_destroy(&(p_arg->p_values));
			break;
		}
		const bool does_stop = si_arg_is_stopping(p_arg);
		free((void*)p_arg_id);
		p_arg_id = NULL;
		if (true == does_stop)
		{
			break;
		}
	}
	free((void*)p_arg_id);
	p_arg_id = NULL;
	si_argparse_prompt_all(p_parse);
	result = si_argparse_is_valid_values(p_parse);
END:
	return result;
}

void si_argparse_fprint_help(FILE* const p_file, si_argparse_t* const p_parse)
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
			const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
			if (true != is_optional)
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
			const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
			if (true != is_optional)
			{
				fprintf(p_file, "\t");
				si_arg_fprint(p_file, p_arg);
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
			const bool is_optional = SI_ARG_IS_OPTIONAL(p_arg->bit_flags);
			if (true == is_optional)
			{
				fprintf(p_file, "\t");
				si_arg_fprint(p_file, p_arg);
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

void si_argparse_fprint_error(FILE* const p_file, si_argparse_t* const p_parse)
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
			si_arg_fprint_id(p_file, p_arg);
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
