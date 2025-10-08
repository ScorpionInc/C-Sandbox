// si_logger.c
#include "si_logger.h"

void si_logger_init_2(si_logger_t* const p_logger, const size_t logging_level)
{
	p_logger->logging_level = logging_level;
	si_parray_init(&(p_logger->levels));

	const size_t level_size = sizeof(si_logger_level_t);
	si_logger_level_t default_levels[] = {
		si_all_level_t,
		si_debug_level_t,
		si_info_level_t,
		si_warning_level_t,
		si_error_level_t,
		si_critical_level_t
	};
	const size_t default_levels_count = sizeof(default_levels) / level_size;
	for(size_t iii = 0u; iii < default_levels_count; iii++)
	{
		(void)si_parray_append_clone(
			&(p_logger->levels), &(default_levels[iii]), level_size
		);
	}
}
inline void si_logger_init(si_logger_t* const p_logger)
{
	// Default logging_level is SI_LOGGER_DEFAULT_LEVEL (WARNING)
	si_logger_init_2(p_logger, SI_LOGGER_DEFAULT_LEVEL);
}

si_logger_t* si_logger_new_1(const size_t logging_level)
{
	si_logger_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_logger_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_logger_init_2(p_new, logging_level);
END:
	return p_new;
}
inline si_logger_t* si_logger_new()
{
	// Default logging_level is SI_LOGGER_DEFAULT_LEVEL (WARNING)
	return si_logger_new_1(SI_LOGGER_DEFAULT_LEVEL);
}

/** Doxygen
 * @brief Gets the longest header string in the logger levels array.
 * 
 * @param p_logger Pointer to the si_logger_t to get largest header length from
 * 
 * @return Returns size_t longest string header length.
 */
static int _si_logger_header_len(si_logger_t* const p_logger)
{
	int result = 0;
	if(NULL == p_logger)
	{
		goto END;
	}
	const size_t level_count = si_parray_count(&(p_logger->levels));
	for(size_t iii = 0u; iii < level_count; iii++)
	{
		si_logger_level_t* p_next = si_parray_at(&(p_logger->levels), iii);
		if(NULL == p_next)
		{
			break;
		}
		if(NULL == p_next->p_header)
		{
			continue;
		}
		const size_t next_str_len = strnlen(p_next->p_header, INT_MAX);
		if ((INT_MAX > next_str_len) && (next_str_len > (size_t)result))
		{
			result = next_str_len;
		}
	}
END:
	return result;
}

/** Doxygen
 * @brief Selects the nearest logging level for msg_level.
 * 
 * @param p_logger Pointer to the si_logger_t to get level from.
 * @param msg_level Log level of the message.
 * 
 * @return Returns nearest level address if levels exist or default (NULL).
 */
static si_logger_level_t* _si_logger_level(si_logger_t* const p_logger,
	const size_t msg_level)
{
	si_logger_level_t* p_result = NULL;
	if (NULL == p_logger)
	{
		goto END;
	}
	const size_t level_count = si_parray_count(&(p_logger->levels));
	size_t closest_level = 0u;
	// Find the closest msg_level
	for (size_t iii = 0u; iii < level_count; iii++)
	{
		si_logger_level_t* p_next = si_parray_at(&(p_logger->levels), iii);
		if (NULL == p_next)
		{
			break;
		}
		if ((msg_level >= p_next->log_level) &&
		    (closest_level <= p_next->log_level))
		{
			closest_level = p_next->log_level;
		}
		// Perfect match?
		if (closest_level == msg_level)
		{
			p_result = p_next;
			goto END;
		}
	}
	// Get closest match
	for (size_t iii = 0u; iii < level_count; iii++)
	{
		si_logger_level_t* p_next = si_parray_at(&(p_logger->levels), iii);
		if (NULL == p_next)
		{
			break;
		}
		if (closest_level == p_next->log_level)
		{
			p_result = p_next;
			break;
		}
	}
END:
	return p_result;
}

/** Doxygen
 * @brief Local function to handle variadic arguments.
 * 
 * @param p_logger Pointer to si_logger struct
 * @param p_format C format string
 * @param msg_level Size_t level of the current message
 * @param args Variadic heap arguments
 */
static void _si_logger_log(si_logger_t* const p_logger,
	const char* const p_format, const size_t msg_level, va_list args)
{
	if (NULL == p_logger)
	{
		goto END;
	}
	if (p_logger->logging_level > msg_level)
	{
		goto END;
	}
	si_logger_level_t* p_level = _si_logger_level(p_logger, msg_level);
	if(NULL == p_level)
	{
		goto END;
	}

	si_flock(p_level->p_file);
	const int header_len = _si_logger_header_len(p_logger);
	si_logger_level_fprint_header(p_level, header_len, msg_level);
	vfprintf(p_level->p_file, p_format, args);
	si_funlock(p_level->p_file);
END:
	return;
}

void si_logger_custom(si_logger_t* const p_logger, const size_t msg_level,
	const char* p_prefix, const void* const p_data, const char* p_suffix,
	void p_print(FILE* const, const void* const))
{
	if((NULL == p_logger) || (NULL == p_data) || (NULL == p_print))
	{
		goto END;
	}
	si_logger_level_t* p_level = _si_logger_level(p_logger, msg_level);
	if(NULL == p_level)
	{
		goto END;
	}
	if(NULL == p_level->p_file)
	{
		goto END;
	}
	const int header_padding = _si_logger_header_len(p_logger);
	si_flock(p_level->p_file);
	si_logger_level_fprint_header(p_level, header_padding, msg_level);
	fprintf(p_level->p_file, "%s", p_prefix);
	p_print(p_level->p_file, p_data);
	fprintf(p_level->p_file, "%s", p_suffix);
	si_funlock(p_level->p_file);
END:
	return;
}

void si_logger_log(si_logger_t* const p_logger,
	const char* const p_format, size_t msg_level, ...)
{
	va_list args;
	va_start(args, msg_level);
	_si_logger_log(p_logger, p_format, msg_level, args);
	va_end(args);
}
void si_logger_verbose(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_ALL, args);
	va_end(args);
}
void si_logger_debug(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_DEBUG, args);
	va_end(args);
}
void si_logger_info(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_INFO, args);
	va_end(args);
}
void si_logger_warning(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_WARNING, args);
	va_end(args);
}
void si_logger_error(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_ERROR, args);
	va_end(args);
}
void si_logger_critical(si_logger_t* const p_logger,
	const char* const p_format, ...)
{
	va_list args;
	va_start(args, p_format);
	_si_logger_log(p_logger, p_format, SI_LOGGER_CRITICAL, args);
	va_end(args);
}

void si_logger_free(si_logger_t* const p_logger)
{
	if (NULL == p_logger)
	{
		goto END;
	}
	si_parray_free(&(p_logger->levels));
END:
	return;
}

void si_logger_destroy(si_logger_t** pp_logger)
{
	if (NULL == pp_logger)
	{
		goto END;
	}
	if (NULL == *pp_logger)
	{
		// Already freed
		goto END;
	}
	si_logger_free(*pp_logger);
	free(*pp_logger);
	*pp_logger = NULL;
END:
	return;
}
