/* si_strings.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with C Strings.
 * Created: 20250820
 * Updated: 20250820
//*/

#include <ctype.h> // toupper()
#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // calloc()
#include <string.h> // memcpy(), strnlen()

#ifndef SI_STRINGS_H
#define SI_STRINGS_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** Doxygen
 * @brief Creates new heap C string from joining two other strings by size.
 * 
 * @param p_left Pointer to the start of the left side of the string join.
 * @param left_size Size_t count of bytes in the left side.
 * @param p_right Pointer to the start of the right side of the string join.
 * @param right_size Size_t count of bytes in the right side.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char* strn_clone_concat(const char* const p_left, const size_t left_size,
	const char* const p_right, const size_t right_size);
char* str_clone_concat(const char* const p_left,
	const char* const p_right);

/** Doxygen
 * @brief Creates a new heap C string from joining/concatenating the strings in
 *        a string array seperated with an optional C string seperator.
 * 
 * @param p_seperator Optional C string pointer for seperating array's strings.
 * @param argc Size_t count of strings in the C String array.
 * @param pp_argv Pointer to the C string array to read from.
 * 
 * @return Returns heap string pointer on success. Returns NULL otherwise.
 */
char* str_clone_join(const size_t argc, const char** const pp_argv,
	const char* const p_seperator);

/** Doxygen
 * @brief Modifies inplace a C-String at p_input_str of size to use UPPER CASE
 *        letters.
 *
 * @param p_input_str Pointer to first char of C-String to be UPPER CASED.
 * @param input_size Size_t count of bytes in the input string.
 */
void strn_to_uppercase(char* const p_input_str, const size_t input_size);
void str_to_uppercase(char* const p_input_str);

/** Doxygen
 * @brief Modifies inplace a C-String at p_input_str of size to use lower case
 *        letters.
 *
 * @param p_input_str Pointer to first char of C-String to be lower cased.
 * @param input_size Size_t count of bytes in the input string.
 */
void strn_to_lowercase(char* const p_input_str, const size_t input_size);
void str_to_lowercase(char* const p_input_str);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_STRINGS_H
