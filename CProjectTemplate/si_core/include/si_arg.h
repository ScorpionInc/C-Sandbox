/* si_arg.h
 * Language: C
 * Purpose : Provide struct and functions to represent a single program
 *           argument and it's options for use with si_argparse.
 * Created : 20251001
 */

#include "si_io.h" // fread_alloc_line()
#include "si_parray.h" // si_parray_t

#include <stdbool.h> // bool, false, true
#include <stddef.h> // size_t
#include <stdint.h> // uint8_t

// Flags bit mask macros
#define SI_ARG_OPTIONAL  (0x01)
#define SI_ARG_IS_OPTIONAL(bit_flags) (0 < (bit_flags & SI_ARG_OPTIONAL))
#define SI_ARG_PROMPT    (0x02)
#define SI_ARG_DOES_PROMPT(bit_flags) (0 < (bit_flags & SI_ARG_PROMPT))
#define SI_ARG_STOP      (0x04)
#define SI_ARG_DOES_STOP(bit_flags)   (0 < (bit_flags & SI_ARG_STOP))

// Arguments are required by default.
#define SI_ARG_DEFAULT_IS_OPTIONAL (0x00)
// Prompts for missing required arguments by default.
#define SI_ARG_DEFAULT_PROMPT      (SI_ARG_PROMPT)
// Arguments do not halt all other parsing by default.
#define SI_ARG_DEFAULT_STOP        (0x00)
#define SI_ARG_DEFAULT_BIT_FLAGS \
	(SI_ARG_DEFAULT_IS_OPTIONAL | SI_ARG_DEFAULT_PROMPT | SI_ARG_DEFAULT_STOP)

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifndef SI_ARG_H
#define SI_ARG_H

typedef struct si_arg_t
{
	uint8_t bit_flags;
	char flag;
	int minimum_values;
	int maximum_values;
	const char* p_full;
	const char* p_help;
	si_parray_t* p_values;
	bool  (*p_validate)(const void* const);
	void* (*p_parser)(const char* const);
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
 * @brief Determines if this is the argument that halted the parsing or not.
 * 
 * @param p_arg Pointer to the si_arg_t struct to be checked.
 * 
 * @return Returns stdbool true if stopping. Returns false otherwise.
 */
bool si_arg_is_stopping(const si_arg_t* const p_arg);

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
 * @brief Handles prompting the user(stdin) for more argument values(up to min)
 * 
 * @param p_arg Pointer to the si_arg_t to do the prompting for.
 * 
 * @return Returns the additional number of provided values on success. Or 0u.
 */
size_t si_arg_prompt(si_arg_t* const p_arg);

/** Doxygen
 * @brief Attempts to parse as many valid values from string array as possible.
 * 
 * @param p_arg Pointer to the argument to hold the values and do the parsing.
 * @param argc int size of the string array to be parsed.
 * @param pp_argv Array of C strings to read values from.
 * 
 * @return Returns number of successfully read values from the array. 0 = error
 */
size_t si_arg_parse(si_arg_t* const p_arg,
	const int argc, const char** const pp_argv);

/** Doxygen
 * @brief Writes id or ids of an argument to a file stream.
 * 
 * @param p_arg Pointer to the si_arg to get id(s) from.
 * @param p_file Pointer to the FILE to write id values to.
 */
void si_arg_fprint_id(FILE* const p_file, const si_arg_t* const p_arg);

/** Doxygen
 * @brief Writes argument information to a file stream.
 * 
 * @param p_arg Pointer to the si_arg to be read from.
 * @param p_file Pointer to the FILE to write to.
 */
void si_arg_fprint(FILE* const p_file, const si_arg_t* const p_arg);

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

#endif// SI_ARG_H

#ifdef __cplusplus
}
#endif //__cplusplus
