/* si_argparse.h
 * Language: C
 * Purpose : Defines struct with functions for parsing program arguments.
 * Created : 20250807
 * Updated : 20250811
//*/

#ifdef __linux__

#include <libgen.h> // basename()

#else
#error Unsupported OS
#endif //__linux__

#include "si_arg.h"
#include "si_array.h"
#include "si_realloc_settings.h"
#include "si_strings.h"

#include <limits.h> // INT_MAX
#include <stdbool.h>// true, false
#include <stddef.h> // NULL
#include <stdio.h> // fprintf(), FILE
#include <stdlib.h> // calloc(), free()


#ifndef SI_ARGPARSE_H
#define SI_ARGPARSE_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

typedef struct si_argparse_t
{
	const char* p_prefix;
	const char* p_program_name;
	const char* p_description;
	const char* p_suffix;
	si_array_t arguments;
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
 * @brief Determines if an argument is set within an argparse.
 * 
 * @param p_parse Pointer to the argparse to read from.
 * @param p_id C string to compare with argument ids.
 * 
 * @return Returns stdbool true is arg exists & is set. Returns false otherwise
 */
bool si_argparse_is_set(si_argparse_t* const p_parse,
	const char* const p_id);

/** Doxygen
 * @brief Determines the address of an argument value with in argparse.
 * 
 * @param p_parse Pointer to the argparse to read from.
 * @param p_id C string to compare with argument ids.
 * @param value_index Which value to fetch from the argument. Default(0)
 * 
 * @return Returns value address on success. Returns NULL otherwise
 */
void* si_argparse_value_of_3(si_argparse_t* const p_parse,
	const char* const p_id, const size_t value_index);
void* si_argparse_value_of(si_argparse_t* const p_parse,
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
	const int argc, const char** const pp_argv);

/** Doxygen
 * @brief Prints generated help to file stream from argparse struct.
 * 
 * @param p_parse Pointer to the si_argparse to read from.
 * @param p_file Pointer to the FILE to be written to.
 */
void si_argparse_fprint_help(FILE* const p_file,
	si_argparse_t* const p_parse);

/** Doxygen
 * @brief Prints generated error message to file stream from argparse struct.
 * 
 * @param p_parse Pointer to the si_argparse to read from.
 * @param p_file Pointer to the FILE to be written to.
 */
void si_argparse_fprint_error(FILE* const p_file,
	si_argparse_t* const p_parse);

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
