/* si_strings.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with C Strings.
 * Created: 20250820
 * Updated: 20250825
//*/

#include <ctype.h> // toupper()
#include <stdarg.h> // ...
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
 * @param argc Size_t count of strings in the C String array.
 * @param p_seperator Optional C string pointer for seperating array's strings.
 * @param pp_argv Pointer to the C string array to read from.
 * 
 * @return Returns heap string pointer on success. Returns NULL otherwise.
 */
char* str_clone_join(const size_t argc,	const char* const p_seperator,
	const char** const pp_argv);

/** Doxygen
 * @brief Creates new heap C string from joining arbitary string arguments.
 * 
 * @param argc Size_t count of strings in the C String array.
 * @param p_seperator Optional C string that's used to seperate arguments.
 * @param ... Arbitary number of strings to be concated together.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char* strv_clone_join(const size_t argc, const char* const p_seperator, ...);

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

/** Doxygen
 * @brief Seperates the first found C string from a heap buffer of a known size
 * 
 * @param pp_buffer Pointer to heap buffer to have C string value removed from.
 * @param buffer_size Pointer to the size of the allocated heap buffer. Changes
 * 
 * @return Returns heap string on success and updates heap buffer/size in place
 *         Returns NULL otherwise leaving the heap buffer and size unchanged.
 */
char* pop_str_from_heap(uint8_t** const pp_buffer,
	size_t* const p_buffer_size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_STRINGS_H
