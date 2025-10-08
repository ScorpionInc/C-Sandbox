/* si_logger_level.h - 20251006 */

#include "si_io.h" // si_flock(), si_funlock()
#include "si_strings.h" // str_lgrow_concat()
#include "si_terminfo.h" // si_tui_is_ansi()
#include "si_to_string.h" // char_to_string()

#ifdef __linux__

#include <execinfo.h> // backtrace(), backtrace_symbols_fd()

#elif defined(_WIN32)

#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

#else
#error Unknown/Unsupported OS
#endif// OS Specific includes

#include <stdarg.h> // ...
#include <stdbool.h> // bool, false, true
#include <stddef.h> // size_t
#include <stdio.h> // FILE

#define MAX_CALLSTACK_DEPTH (256)

#define SI_LOGGER_ALL (0)
#define SI_LOGGER_DEBUG (10)
#define SI_LOGGER_INFO (20)
#define SI_LOGGER_WARNING (30)
#define SI_LOGGER_ERROR (40)
#define SI_LOGGER_CRITICAL (50)
#define SI_LOGGER_MAX (__SIZE_MAX__)

#ifndef SI_LOGGER_LEVEL_H
#define SI_LOGGER_LEVEL_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifdef __linux__
/** Doxygen
 * @brief Attempts to print the current call stack to FILE on Linux.
 * 
 * @param p_file Pointer to FILE to print the call stack.
 * @param skip_count Optional Number of function calls to skip printing.
 * @param p_prefix Optional C string prefix.
 */
void fprint_stacktrace_l(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix);
#elif defined(_WIN32)
/** Doxygen
 * @brief Attempts to print the current call stack to FILE on Windows.
 * 
 * @param p_file Pointer to FILE to print the call stack.
 * @param skip_count Optional Number of function calls to skip printing.
 * @param p_prefix Optional C string prefix.
 */
void fprint_stacktrace_w(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix);
#endif// OS Specific Implementations

/** Doxygen
 * @brief Attempts to print the currect call stack to FILE.
 * 
 * @param p_file Pointer to FILE to print the call stack.
 * @param skip_count Optional Number of function calls to skip printing.
 * @param p_prefix Optional C string prefix.
 */
void fprint_stacktrace_3(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix);
void fprint_stacktrace_2(FILE* const p_file,
	const size_t skip_count);
void fprint_stacktrace(FILE* const p_file);

typedef struct si_logger_level_t
{
	bool do_stacktrace;
	size_t log_level;
	const char* p_header_ansi;
	const char* p_header;
	FILE* p_file;
} si_logger_level_t;

// Defines default logger levels
#define si_all_level_t \
	((si_logger_level_t){false, SI_LOGGER_ALL, "\x1b[90m", NULL, stdout})
#define si_debug_level_t \
	((si_logger_level_t){false, SI_LOGGER_DEBUG, "\x1b[32m", "DEBUG", stdout})
#define si_info_level_t \
	((si_logger_level_t){false, SI_LOGGER_INFO, "\x1b[32m", "INFO", stdout})
#define si_warning_level_t \
	((si_logger_level_t){false, SI_LOGGER_WARNING, "\x1b[33m", "WARNING", stdout})
#define si_error_level_t \
	((si_logger_level_t){false, SI_LOGGER_ERROR, "\x1b[31m", "ERROR", stderr})
#define si_critical_level_t \
	((si_logger_level_t){true, SI_LOGGER_CRITICAL, "\x1b[31m", "CRITICAL", stderr})

/** Doxygen
 * @brief Generates a heap header string for the logger level header.
 * 
 * @param p_level Pointer to the si_logger_level_t to make the header of.
 * @param padding Header formatting int padding width.
 * @param msg_level Level of the message header to be printed.
 * 
 * @return Returns heap string of logging level header on Success or NULL.
 */
char* si_logger_level_generate_header(si_logger_level_t* const p_level,
	const int padding, const size_t msg_level);

/** Doxygen
 * @brief Attempts to print the logger level header to it's file. (NO FLOCK)
 * 
 * @param p_level Pointer to the si_logger_level_t to print the header of.
 * @param padding Header formatting int padding width.
 * @param msg_level Level of the message header to be printed.
 */
void si_logger_level_fprint_header(si_logger_level_t* const p_level,
	const int padding, const size_t msg_level);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_LOGGER_LEVEL_H
