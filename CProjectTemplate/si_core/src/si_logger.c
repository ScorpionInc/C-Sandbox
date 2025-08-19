//si_logger.c
#include "si_logger.h"

/** Doxygen
 * @brief Determines if ANSI color is supported
 *
 * @param p_file FILE pointer to test for ANSI support.
 * 
 * @return Returns true if ANSI colors are supported.
 */
static bool si_logger_is_ansi(FILE* p_file)
{
	bool result = false;
	if(NULL == p_file)
	{
		goto END;
	}
#ifdef __linux__
	result = (1 == isatty(fileno(p_file)));
#endif//__linux__
END:
	return result;
}

/** Doxygen
 * @brief Prints line header based upon severity level of the message.
 * 
 * @param p_file File pointer to write to.
 * @param msg_level size_t target message level
 */
static void si_logger_fprint_header(FILE* p_file, size_t msg_level)
{
	if(NULL == p_file)
	{
		goto END;
	}
	const bool is_ansi = si_logger_is_ansi(p_file);
	fprintf(p_file, "[");
	// Start color
	if(is_ansi)
	{
		fprintf(p_file, "\x1b[1m"); // Bold
		if(SI_LOGGER_ALL == msg_level)
		{
			// Light Gray
			fprintf(p_file, "\x1b[90m");
		}
		else if(SI_LOGGER_INFO >= msg_level)
		{
			// Green
			fprintf(p_file, "\x1b[32m");
		}
		else if(SI_LOGGER_WARNING >= msg_level)
		{
			// Yellow
			fprintf(p_file, "\x1b[33m");
		}
		else
		{
			// Red
			fprintf(p_file, "\x1b[31m");
		}
	}
	// Header text
	const char* const header_format = "%8s";
	switch(msg_level)
	{
		case(SI_LOGGER_ALL):
			fprintf(p_file, header_format, "VERBOSE");
			break;
		case(SI_LOGGER_DEBUG):
			fprintf(p_file, header_format, "DEBUG");
			break;
		case(SI_LOGGER_INFO):
			fprintf(p_file, header_format, "INFO");
			break;
		case(SI_LOGGER_WARNING):
			fprintf(p_file, header_format, "WARNING");
			break;
		case(SI_LOGGER_ERROR):
			fprintf(p_file, header_format, "ERROR");
			break;
		case(SI_LOGGER_CRITICAL):
			fprintf(p_file, header_format, "CRITICAL");
			break;
		default:
			fprintf(p_file, "%8lu", msg_level);
			break;
	}
	// End color
	if(is_ansi)
	{
		fprintf(p_file, "\x1b[0m");
	}
	fprintf(p_file, "]: ");
END:
	return;
}

void si_logger_init(si_logger_t* const p_logger)
{
	p_logger->logging_level = SI_LOGGER_DEFAULT;
	p_logger->p_file = NULL;
}

si_logger_t* si_logger_new()
{
	si_logger_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_logger_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_logger_init(p_new);
END:
	return p_new;
}

void si_logger_custom(si_logger_t* const p_logger,
	const void* const p_data, const size_t msg_level,
	void p_print(FILE*, void*))
{
	if(NULL == p_logger)
	{
		goto END;
	}
	if(NULL == p_logger->p_file)
	{
		goto END;
	}
	if(p_logger->logging_level > msg_level)
	{
		goto END;
	}
	si_logger_fprint_header(p_logger->p_file, msg_level);
	if(NULL != p_print)
	{
		p_print(p_logger->p_file, p_data);
	}
	else
	{
		fprintf(p_logger->p_file, "%p", p_data);
	}
	fprintf(p_logger, "\n");
END:
	return;
}

static void _si_logger_log(si_logger_t* const p_logger,
	const char* const p_format, const size_t msg_level, va_list args)
{
	if(NULL == p_logger)
	{
		goto END;
	}
	if(NULL == p_logger->p_file)
	{
		goto END;
	}
	if(p_logger->logging_level > msg_level)
	{
		goto END;
	}
	si_logger_fprint_header(p_logger->p_file, msg_level);
	vfprintf(p_logger->p_file, p_format, args);
	fprintf(p_logger->p_file, "\n");
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

void si_logger_destroy(si_logger_t** pp_logger)
{
	if(NULL == pp_logger)
	{
		goto END;
	}
	if(NULL == *pp_logger)
	{
		// Already freed
		goto END;
	}
	free(*pp_logger);
	*pp_logger = NULL;
END:
	return;
}