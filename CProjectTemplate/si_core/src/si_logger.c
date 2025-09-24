//si_logger.c
#include "si_logger.h"

void fprint_stacktrace_3(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const char* p_mut_prefix = "";
	if (NULL != p_prefix)
	{
		p_mut_prefix = p_prefix;
	}
#ifdef __linux__
	const size_t MAX_STACK_DEPTH = 256u;
	const size_t STACK_BUFFER_SIZE = (MAX_STACK_DEPTH * sizeof(void*));
	void* p_addresses = NULL;
	p_addresses = calloc(1u, STACK_BUFFER_SIZE);
	if (NULL == p_addresses)
	{
		goto END;
	}
	const size_t backtrace_size = backtrace(p_addresses, MAX_STACK_DEPTH);
	char** p_stack_strs = backtrace_symbols(p_addresses, backtrace_size);
	if (NULL == p_stack_strs)
	{
		free(p_addresses);
		p_addresses = NULL;
		goto END;
	}
	for (size_t iii = skip_count; iii < backtrace_size; iii++)
	{
		fprintf(p_file, "%s%s\n", p_mut_prefix, p_stack_strs[iii]);
	}
	if (backtrace_size >= MAX_STACK_DEPTH)
	{
		fprintf(p_file, "%s(callstack truncated)\n", p_mut_prefix);
	}
	free(p_stack_strs);
	p_stack_strs = NULL;
	free(p_addresses);
	p_addresses = NULL;
#else
	// Fails silently
#endif//__linux__
END:
	return;
}
inline void fprint_stacktrace_2(FILE* const p_file,
	const size_t skip_count)
{
	// Default value of p_prefix is NULL
	fprint_stacktrace_3(p_file, skip_count, NULL);
}
inline void fprint_stacktrace(FILE* const p_file)
{
	// Default value of skip_count is 1
	fprint_stacktrace_2(p_file, 1u);
}

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
	if (NULL == p_file)
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
 * @brief Maps msg_level to an associated preset header string.
 * 
 * @param msg_level Log level to get the header of.
 * 
 * @return Returns const C string on success. Returns NULL otherwise.
 */
static char* const si_logger_select_header(const size_t msg_level)
{
	const char* p_result = NULL;
	switch (msg_level)
	{
		case(SI_LOGGER_ALL):
			p_result = "VERBOSE";
			break;
		case(SI_LOGGER_DEBUG):
			p_result = "DEBUG";
			break;
		case(SI_LOGGER_INFO):
			p_result = "INFO";
			break;
		case(SI_LOGGER_WARNING):
			p_result = "WARNING";
			break;
		case(SI_LOGGER_ERROR):
			p_result = "ERROR";
			break;
		case(SI_LOGGER_CRITICAL):
			p_result = "CRITICAL";
			break;
		default:
			break;
	}
	return p_result;
}

/** Doxygen
 * @brief Maps msg_level to an associated preset ANSI color string.
 * 
 * @param msg_level Log level to get ANSI color string of.
 * 
 * @return Returns a const C string
 */
static char* const si_logger_select_color(const size_t msg_level)
{
	const char* p_ansi = "";
	if (SI_LOGGER_ALL == msg_level)
	{
		// Light Gray
		p_ansi = "\x1b[90m";
	}
	else if (SI_LOGGER_INFO >= msg_level)
	{
		// Green
		p_ansi = "\x1b[32m";
	}
	else if (SI_LOGGER_WARNING >= msg_level)
	{
		// Yellow
		p_ansi = "\x1b[33m";
	}
	else
	{
		// Red
		p_ansi = "\x1b[31m";
	}
	return p_ansi;
}

/** Doxygen
 * @brief Prints line header based upon severity level of the message.
 * 
 * @param p_file File pointer to write to.
 * @param msg_level size_t target message level
 * @param is_ansi Flag type of stdbool determines if ANSI Colors are printed.
 */
static void si_logger_fprint_header(FILE* p_file, const size_t msg_level,
	const bool is_ansi)
{
	if (NULL == p_file)
	{
		goto END;
	}
	fprintf(p_file, "[");
	// Start ANSI color
	if (true == is_ansi)
	{
		const char* const p_color = si_logger_select_color(msg_level);
		fprintf(p_file, "\x1b[1m%s", p_color); // Bold / Color
	}
	// Header text
	const char* p_header = si_logger_select_header(msg_level);
	if (NULL == p_header)
	{
		fprintf(p_file, "%8lu", msg_level);
	}
	else
	{
		fprintf(p_file, "%8s", p_header);
	}
	// End ANSI color
	if (true == is_ansi)
	{
		fprintf(p_file, "\x1b[0m");
	}
	fprintf(p_file, "]: ");
END:
	return;
}

/** Doxygen
 * @brief Allocates and initializes a new header string on the heap.
 * 
 * @param Log level of the header to generate.
 * @param is_ansi Flag type of stdbool determines if ANSI Colors are included.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
static char* si_logger_header_new(const size_t msg_level, const bool is_ansi)
{
	char* p_header = NULL;
	const size_t MAX_LENGTH = 64u;
	p_header = calloc(MAX_LENGTH, sizeof(char));
	if (NULL == p_header)
	{
		goto END;
	}
	FILE* p_file = NULL;
	p_file = fmemopen(p_header, MAX_LENGTH, "w");
	if (NULL == p_file)
	{
		free(p_header);
		p_header = NULL;
		goto END;
	}
	si_logger_fprint_header(p_file, msg_level, is_ansi);
	fflush(p_file);
	(void)fclose(p_file);
	p_file = NULL;
END:
	return p_header;
}

void si_logger_init_3(si_logger_t* const p_logger, FILE* const p_file,
	const size_t logging_level)
{
	p_logger->stacktrace_level = SI_LOGGER_DEFAULT_STACKTRACE;
	p_logger->logging_level = logging_level;
	pthread_mutex_init(&p_logger->file_lock, NULL);
	p_logger->p_file = p_file;
}
inline void si_logger_init_2(si_logger_t* const p_logger, FILE* const p_file)
{
	// Default logging_level is SI_LOGGER_DEFAULT_LEVEL (WARNING)
	si_logger_init_3(p_logger, p_file, SI_LOGGER_DEFAULT_LEVEL);
}
inline void si_logger_init(si_logger_t* const p_logger)
{
	// Default p_file is stdout
	si_logger_init_2(p_logger, stdout);
}

si_logger_t* si_logger_new_2(FILE* const p_file, const size_t logging_level)
{
	si_logger_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_logger_t));
	if (NULL == p_new)
	{
		goto END;
	}
	si_logger_init_3(p_new, p_file, logging_level);
END:
	return p_new;
}
inline si_logger_t* si_logger_new_1(FILE* const p_file)
{
	// Default logging_level is SI_LOGGER_DEFAULT_LEVEL (WARNING)
	return si_logger_new_2(p_file, SI_LOGGER_DEFAULT_LEVEL);
}
inline si_logger_t* si_logger_new()
{
	// Default p_file is stdout
	return si_logger_new_1(stdout);
}

void si_logger_custom(si_logger_t* const p_logger, const size_t msg_level,
	const char* p_prefix, const void* const p_data, const char* p_suffix,
	void p_print(FILE* const, const void* const))
{
	if (NULL == p_logger)
	{
		goto END;
	}
	if (NULL == p_logger->p_file)
	{
		goto END;
	}
	if (p_logger->logging_level > msg_level)
	{
		goto END;
	}
	const bool is_ansi = si_logger_is_ansi(p_logger->p_file);
	pthread_mutex_lock(&(p_logger->file_lock));
	si_logger_fprint_header(p_logger->p_file, msg_level, is_ansi);
	if (NULL != p_prefix)
	{
		fprintf(p_logger->p_file, "%s", p_prefix);
	}
	if (NULL != p_print)
	{
		p_print(p_logger->p_file, p_data);
	}
	else
	{
		fprintf(p_logger->p_file, "%p", p_data);
	}
	if (NULL != p_suffix)
	{
		fprintf(p_logger->p_file, "%s", p_suffix);
	}
	fprintf(p_logger->p_file, "\n");
	pthread_mutex_unlock(&(p_logger->file_lock));
END:
	return;
}

/** Doxygen
 * @brief Local function to handle variatic arguments.
 * 
 * @param p_logger Pointer to si_logger struct
 * @param p_format C format string
 * @param msg_level Size_t level of the current message
 * @param args Variatic heap arguments
 */
static void _si_logger_log(si_logger_t* const p_logger,
	const char* const p_format, const size_t msg_level, va_list args)
{
	if (NULL == p_logger)
	{
		goto END;
	}
	if (NULL == p_logger->p_file)
	{
		goto END;
	}
	if (p_logger->logging_level > msg_level)
	{
		goto END;
	}
	const bool is_ansi = si_logger_is_ansi(p_logger->p_file);
	const char* p_header = si_logger_header_new(msg_level, is_ansi);
	if (p_logger->stacktrace_level <= msg_level)
	{
		// Skips printing the stackcall printing functions
		const size_t skip_count = 3u;
		fprint_stacktrace_3(p_logger->p_file, skip_count, p_header);
	}
	pthread_mutex_lock(&(p_logger->file_lock));
	fprintf(p_logger->p_file, "%s", p_header);
	vfprintf(p_logger->p_file, p_format, args);
	fprintf(p_logger->p_file, "\n");
	pthread_mutex_unlock(&(p_logger->file_lock));
	free((void*)p_header);
	p_header = NULL;
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
	pthread_mutex_destroy(&p_logger->file_lock); 
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
