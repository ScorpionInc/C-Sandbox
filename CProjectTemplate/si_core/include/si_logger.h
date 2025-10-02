/* si_logger.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for selective logging to file stream.
 * Created: 20250809
 * Updated: 20250811
//*/

#include <stdarg.h> // ...
#include <stdbool.h> // true, false
#include <stdio.h> // FILE, fileno()
#include <stdlib.h> // calloc(), free()

#ifdef __linux__

#include <execinfo.h> // backtrace(), backtrace_symbols_fd()
#include <pthread.h> // pthread_mutex_t, pthread_mutex_unlock()
#include <unistd.h> // isatty()

#else
#error Unsupported OS
#endif //__linux__

#define SI_LOGGER_ALL (0)
#define SI_LOGGER_DEBUG (10)
#define SI_LOGGER_INFO (20)
#define SI_LOGGER_WARNING (30)
#define SI_LOGGER_ERROR (40)
#define SI_LOGGER_CRITICAL (50)
#define SI_LOGGER_MAX (__SIZE_MAX__)

#define SI_LOGGER_DEFAULT_STACKTRACE SI_LOGGER_CRITICAL
#define SI_LOGGER_DEFAULT_LEVEL      SI_LOGGER_WARNING

#ifndef SI_LOGGER_H
#define SI_LOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

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

typedef struct si_logger_t
{
	size_t stacktrace_level;
	size_t logging_level;
	pthread_mutex_t file_lock;
	FILE* p_file;
} si_logger_t;

/** Doxygen
 * @brief Initializes an existing si_logger struct by pointer.
 * 
 * @param p_logger Pointer to si_logger to initialize.
 * @param p_file Pointer to FILE to be logged to.
 * @param logging_level Initial log level of the logger.
 */
void si_logger_init_3(si_logger_t* const p_logger, FILE* const p_file,
	const size_t logging_level);
void si_logger_init_2(si_logger_t* const p_logger, FILE* const p_file);
void si_logger_init(si_logger_t* const p_logger);

/** Doxygen
 * @brief Allocates & initializes a new si_logger struct on the heap.
 * 
 * @param p_file Pointer to FILE to be logged to.
 * @param logging_level Initial log level of the logger.
 */
si_logger_t* si_logger_new_2(FILE* const p_file, const size_t logging_level);
si_logger_t* si_logger_new_1(FILE* const p_file);
si_logger_t* si_logger_new();

/** Doxygen
 * @brief Based on logger level calls a custom print function on data.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param msg_level size_t Log level of this message.
 * @param p_prefix Optional C string to print after header, but before function
 * @param p_data Pointer to data to pass to print function.
 * @param p_suffix Optional C string to print after the function.
 * @param p_print Optional function pointer(FILE*, void*) to print data value.
 */
void si_logger_custom(si_logger_t* const p_logger, const size_t msg_level,
	const char* p_prefix, const void* const p_data, const char* p_suffix,
	void p_print(FILE* const, const void* const));

/** Doxygen
 * @brief Logs a message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param msg_level size_t Log level of this message.
 * @param ... Variable arguments used with format string.
 */
void si_logger_log(si_logger_t* const p_logger,
	const char* const p_format, const size_t msg_level, ...);

/** Doxygen
 * @brief Logs a verbose message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_verbose(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Logs a debug message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_debug(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Logs a info message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_info(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Logs a warning message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_warning(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Logs a error message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_error(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Logs a critical message based on logger level to file.
 * 
 * @param p_logger Pointer to si_logger struct to use.
 * @param p_format Message format string.
 * @param ... Variable arguments used with format string.
 */
void si_logger_critical(si_logger_t* const p_logger,
	const char* const p_format, ...);

/** Doxygen
 * @brief Frees allocated data inside of an existing si_logger struct.
 * 
 * @param p_logger Pointer to si_logger_t struct to free contents of.
 */
void si_logger_free(si_logger_t* const p_logger);

/** Doxygen
 * @brief Frees an allocated si_logger struct on the heap.
 * 
 * @param pp_logger Pointer to the si_logger pointer to be freed.
 */
void si_logger_destroy(si_logger_t** pp_logger);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_LOGGER_H
