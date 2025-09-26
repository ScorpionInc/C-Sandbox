/* si_argparse.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines struct with functions for parsing program arguments.
 * Created: 20250807
 * Updated: 20250811
//*/

#include <stdbool.h>// true, false
#include <stddef.h> // NULL
#include <stdio.h> // fprintf(), FILE
#include <stdlib.h> // calloc(), free()

#ifdef __linux__

#include <libgen.h> // basename()

#else
#error Unsupported OS
#endif //__linux__

#include "si_array.h"
#include "si_realloc_settings.h"

// Arguments are required by default.
#define SI_ARGPARSE_DEFAULT_OPTIONAL (false)

#ifndef SI_ARGPARSE_H
#define SI_ARGPARSE_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_arg_t
{
	bool is_optional;
	const char* p_full;
	char flag;
	const char* p_help;
	size_t minimum_values;
	size_t maximum_values;
	si_array_t* p_values;
	bool  (*p_validate)(void*);
	void* (*p_parser)(const char*);
	void  (*p_free_value)(void*);
} si_arg_t;

/** Doxygen
 * @brief Initializes an existing argument struct to it's default values.
 * 
 * @param p_arg Pointer to the si_arg struct to be initialized.
 */
void si_arg_init(si_arg_t* const p_arg);

/** Doxygen
 * @brief Allocates and initializes a new si_arg on the heap.
 * 
 * @return Returns pointer to new si_arg on success. NULL otherwise.
 */
si_arg_t* si_arg_new();

/** Doxygen
 * @brief Conducts basic validation checks on a si_arg struct.
 * 
 * @param p_arg Pointer to the si_arg to be validated.
 * 
 * @return Returns true if valid. Returns false otherwise.
 * @details Validates the argument can be checked for, having either a flag or
 *          full pointer defined with value bounds that make sense. Doesn't
 *          check if the argument has been set or if the values are within the
 *          expected range see si_arg_is_valid_values() instead.
 */
bool si_arg_is_valid(const si_arg_t* const p_arg);

/** Doxygen
 * @brief Verifies that an argument is valid and has valid values.
 * 
 * @param p_arg Pointer to the si_arg to be validated.
 * 
 * @return Returns true if valid. Returns false otherwise.
 */
bool si_arg_is_valid_values(const si_arg_t* const p_arg);

/** Doxygen
 * @brief Tests if an argument has been set or not.
 * 
 * @param p_arg Pointer to si_arg to check.
 * 
 * @return Returns true if argument was found. Returns false otherwise.
 */
bool si_arg_is_set(const si_arg_t* const p_arg);

/** Doxygen
 * @brief Tests if either of an arguments ids match a string.
 * 
 * @param p_arg Pointer to the si_arg struct to check.
 * @param p_str String to check against.
 * 
 * @return Returns true if either flag or full match. Returns false otherwise.
 */
bool si_arg_matches(const si_arg_t* const p_arg, const char* p_str);

/** Doxygen
 * @brief Adds a new value pointer to an argument.
 * 
 * @param p_arg Pointer to the si_arg to append value to.
 * @param p_value Pointer to values to be appended.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_arg_append_value(si_arg_t* const p_arg, const void* const p_value);

/** Doxygen
 * @brief Writes id or ids of an argument to a file stream.
 * 
 * @param p_arg Pointer to the si_arg to get id(s) from.
 * @param p_file Pointer to the FILE to write id values to.
 */
void si_arg_fprint_id(const si_arg_t* const p_arg, FILE* const p_file);

/** Doxygen
 * @brief Writes argument information to a file stream.
 * 
 * @param p_arg Pointer to the si_arg to be read from.
 * @param p_file Pointer to the FILE to write to.
 */
void si_arg_fprint(const si_arg_t* const p_arg, FILE* const p_file);

/** Doxygen
 * @brief Release allocated data within an existing argument struct.
 * 
 * @param p_arg Pointer to argument struct to have it's contents freed.
 */
void si_arg_free(si_arg_t* const p_arg);

/** Doxygen
 * @brief Releases an allocated si_arg and everything within.
 * 
 * @param pp_arg Pointer to the si_arg pointer to be freed.
 */
void si_arg_destroy(si_arg_t** pp_arg);


typedef struct si_argparse_t
{
	const char* p_prefix;
	const char* p_program_name;
	const char* p_description;
	const char* p_suffix;
	si_array_t arguments;
	void  (*p_free_program_name)(void*);
} si_argparse_t;

/** Doxygen
 * @brief Initializes a argparse struct to default values by pointer.
 * 
 * @param p_parse Pointer to the si_argparse to be initialized.
 */
void si_argparse_init(si_argparse_t* const p_parse);

/** Doxygen
 * @brief Allocate and initialize a new si_argparse struct on the heap.
 * 
 * @return Return pointer to new si_argparse on success. Returns NULL otherwise
 */
si_argparse_t* si_argparse_new();

/** Doxygen
 * @brief Tests an argparse's arguments for basic validity.
 * 
 * @param p_parse Pointer to the si_argparse struct to validate.
 * 
 * @return Returns stdbool true on valid. Returns false otherwise.
 */
bool si_argparse_is_valid(const si_argparse_t* const p_parse);

/** Doxygen
 * @brief Tests an argparse's arguments and their values for validity.
 * 
 * @param p_parse Pointer to argparse struct to have argument values validated.
 * 
 * @return Returns stdbool true on valid. Returns false otherwise.
 */
bool si_argparse_is_valid_values(const si_argparse_t* const p_parse);

/** Doxygen
 * @brief Adds an existing argument struct to argparse by pointer.
 * 
 * @param p_parse Pointer to the argparse to add to.
 * @param p_arg Pointer to the si_arg to be added.
 * 
 * @return Returns true on success. Returns false otherwise.
 */
bool si_argparse_add_argument(si_argparse_t* const p_parse,
	const si_arg_t* const p_arg);

/** Doxygen
 * @brief Returns reference to an argument by it's id string.
 * 
 * @param p_parse Pointer to the argparse to read from.
 * @param p_id C string to compare with argument ids.
 * 
 * @return Returns si_arg pointer on success. Returns NULL otherwise.
 */
si_arg_t* si_argparse_at(si_argparse_t* const p_parse,
	const char* const p_id);

/** Doxygen
 * @brief Walks the arguments on an argparse counting optional arguments.
 * 
 * @param p_parse Pointer to the si_argparse to walk/count.
 * 
 * @return Returns size_t count on success. Returns SIZE_MAX otherwise.
 */
size_t si_argparse_count_optional(const si_argparse_t* const p_parse);

/** Doxygen
 * @brief Walks the arguments on an argparse counting required arguments.
 * 
 * @param p_parse Pointer to the si_argparse to walk/count.
 * 
 * @return Returns size_t count on success. Returns SIZE_MAX otherwise.
 */
size_t si_argparse_count_required(const si_argparse_t* const p_parse);

/** Doxygen
 * @brief Returns count of arguments assigned to an argparse.
 * 
 * @param p_parse Pointer to the si_argparse to get count.
 * 
 * @return Returns size_t count on success. Returns SIZE_MAX otherwise.
 */
size_t si_argparse_count(const si_argparse_t* const p_parse);

/** Doxygen
 * @brief Parses main input into arguments and values in an argparse struct.
 * 
 * @param p_parse Pointer to the si_argparse to be used.
 * @param argc Count of arguments main received.
 * @param pp_argv Pointer array of strings containing main arguments.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_argparse_parse(si_argparse_t* const p_parse,
	const int argc, char** const pp_argv);

/** Doxygen
 * @brief Prints generated help to file stream from argparse struct.
 * 
 * @param p_parse Pointer to the si_argparse to read from.
 * @param p_file Pointer to the FILE to be written to.
 */
void si_argparse_fprint_help(si_argparse_t* const p_parse,
	FILE* const p_file);

/** Doxygen
 * @brief Prints generated error message to file stream from argparse struct.
 * 
 * @param p_parse Pointer to the si_argparse to read from.
 * @param p_file Pointer to the FILE to be written to.
 */
void si_argparse_fprint_error(si_argparse_t* const p_parse,
	FILE* const p_file);

/** Doxygen
 * @brief Frees allocated memory inside an argparse.
 * 
 * @param p_parse Pointer to the si_argparse to free contents of.
 */
void si_argparse_free(si_argparse_t* const p_parse);

/** Doxygen
 * @brief Frees allocates si_argparse and all memory within.
 * 
 * @param pp_parse Pointer to the si_argparse pointer to be freed.
 */
void si_argparse_destroy(si_argparse_t** pp_parse);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_ARGPARSE_H
