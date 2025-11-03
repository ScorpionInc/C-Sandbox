/* si_strings.h
 * Language: C
 * Purpose: Defines function(s) for working with C Strings.
 * Created: 20250820
 * Updated: 20250825
//*/

#include <ctype.h> // isprint(), tolower(), toupper()
#include <stdarg.h> // ...
#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include <stdio.h> // FILE
#include <stdlib.h> // calloc(), realloc(), free()
#include <string.h> // memcpy(), memmove(), strnlen()

#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif//_WIN32

#ifndef SI_STRINGS_H
#define SI_STRINGS_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifndef _GNU_SOURCE
/** Doxygen
 * @brief Cover down for possible missing strndup() GNU function (non-standard)
 * 
 * @param p_src Source C string to be duplicated onto the heap.
 * @param size Number of narrow characters in the C string to be duplicated.
 * 
 * @return Return NULL-terminated heap string on success returns NULL otherwise
 */
char* strndup(const char* const p_src, const size_t size);
#endif//_GNU_SOURCE

#if defined(__FreeBSD__) || defined(__APPLE__)
// Has strnstr() as part of string.h
#else
// Doesn't have a strnstr() implementation.
char* strnstr(const char* const p_haystack, const char* const p_needle,
	const size_t hay_size);
#endif//strnstr()

/** Doxygen
 * @brief Generates a new heap C String from an optional C string pattern.
 * 
 * @details Generates a new heap C String from an optional C string pattern.
 *          Resulting heap string if created will always be a minimum of 1 byte
 *          in size(NULL-Terminator). If the optional pattern is not
 *          provided(NULL), the string is still initialized but filled with
 *          0x00(NULL) characters. (Basically it's calloc() call cast to char*)
 * 
 * @param str_len Desired output length of string(Not counting NULL terminator)
 * @param p_pattern C string character pattern to repeat in string values.(OPT)
 * @param pattern_len Count of characters in the pattern to be repeated.(OPT)
 * 
 * @return Returns heap c string on success. Returns NULL otherwise.
 */
char* strn_new(const size_t str_len, const char* const p_pattern,
	const size_t pattern_len);
char* str_new(const size_t str_len, const char* const p_pattern);

/** Doxygen
 * @brief Applies left or right space padding to an existing heap string.
 * 
 * @details If pad is zero string will still be resized to hold only strlen
 *          characters + NULL.
 * 
 * @param pp_str Pointer to heap string pointer to be padded.
 * @param str_len Current length of the string in characters. (Not NULL)
 * @param pad Desired padding value of the heap string.
 * 
 * @return Returns new size of string on success. Returns SIZE_MAX on error.
 */
size_t strn_pad(char** const pp_str, const size_t str_len, const int pad);
size_t str_pad(char** const pp_str, const int pad);

/** Doxygen
 * @brief Clones string onto heap with left or right space padding applied.
 * 
 * @details If pad is zero this is essentially a call to strndup().
 * 
 * @param p_str Pointer to the C String to be cloned and padded.
 * @param str_len Current length of the string in characters. (Not NULL)
 * @param pad Desired padding value of the heap string.
 * 
 * @return Returns new heap string on success. Returns NULL otherwise.
 */
char* strn_clone_pad(const char* const p_str, const size_t str_len, const int pad);
char* str_clone_pad(const char* const p_str, const int pad);


/** Doxygen
 * @brief Grows the left string as needed to hold the right string's value.
 * 
 * @param p_left Heap C string to grow and to hold string results on success.
 * @param left_size Size of the left heap string initially.
 * @param p_right C String to be concated onto the end of the left string.
 * @param right_size Size of the right C string to be concated.
 * 
 * @return Returns size_t SIZE_MAX on failure, returns new strlen on success.
 */
size_t strn_lgrow_concat(char** pp_left, const size_t left_size,
	const char* const p_right, const size_t right_size);
size_t str_lgrow_concat(char** pp_left, const char* const p_right);

/** Doxygen
 * @brief Grows the right string as needed to hold the left string's value.
 * 
 * @param p_left C string to be concated before the start of the right string.
 * @param left_size Size of the left C string to be concated.
 * @param p_right Heap C String to grow and to hold string results on success.
 * @param right_size Size of the right heap string initially.
 * 
 * @return Returns size_t SIZE_MAX on failure, returns new strlen on success.
 */
size_t strn_rgrow_concat(const char* const p_left, const size_t left_size,
	char** pp_right, const size_t right_size);
size_t str_rgrow_concat(const char* const p_left, char** pp_right);

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
 * @brief Creates a new NULL-terminated heap C string from joining an arbitrary
 *        number of other C strings by variadic arguments.
 * 
 * @param argc Only required parameter size_t specifying the number of C strs
 *             that are being joined.
 * @param ... Any number of NULL-terminated C Strings to be joined.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char* strv_clone_concat(const size_t argc, ...);


/** Doxygen
 * @brief Creates a new heap C string from joining/concatenating the strings in
 *        a string array separated with an optional C string separator.
 * 
 * @param argc Size_t count of strings in the C String array.
 * @param p_separator Optional C string pointer for separating array's strings.
 * @param pp_argv Pointer to the C string array to read from.
 * 
 * @return Returns heap string pointer on success. Returns NULL otherwise.
 */
char* str_clone_join(const size_t argc,	const char* const p_separator,
	const char** const pp_argv);

/** Doxygen
 * @brief Creates new heap C string from joining arbitrary string arguments.
 * 
 * @param argc Size_t count of strings in the C String array.
 * @param p_separator Optional C string that's used to separate arguments.
 * @param ... Arbitrary number of strings to be concated together.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char* strv_clone_join(const size_t argc, const char* const p_separator, ...);


/** Doxygen
 * @brief Splits the haystack string by needles into a pointer array size count
 * 
 * @param p_haystack Read only C-String to be split.
 * @param haystack_len Number of characters/bytes in the haystack to be read.
 * @param p_needle Read only C-String to determine where to split.
 * @param needle_len Number of characters/bytes in the needle to be read.
 * @param p_count Pointer to size_t to hold number of pointers in heap array.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char** strn_split(const char* const p_haystack, const size_t haystack_len,
	const char* const p_needle, const size_t needle_len,
	size_t* const p_count);
char** str_split(const char* const p_haystack, const char* p_needle,
	size_t* const p_count);

/** Doxygen
 * @brief Frees heap array of heap C-Strings generated by strn_split().
 * 
 * @param ppp_array Pointer to heap array of C-Strings to be freed.
 * @param arg_count Number of elements in the array to be freed.
 */
void str_split_destroy(char*** const ppp_array, const size_t arg_count);


typedef int (*should_count_char_f)(const char);

/** Doxygen
 * @brief Uses a provided function to determine if a character in a C str
 *        should be counted up to a maximum length specified.
 * 
 * @param p_str C string pointer to read narrow characters from.
 * @param max_len Maximum number of characters to read from string as size_t.
 * @param should_count_char Function pointer determines if character is counted
 */
size_t strn_countf(const char* const p_str, const size_t max_len,
	should_count_char_f should_count_char);
size_t str_countf(const char* const p_str,
	should_count_char_f should_count_char);


typedef char (* const chr_remap_f)(const char, const size_t);

/** Doxygen
 * @brief Inline modified a strings character value with a function that maps
 *        the original character and index to a new char value.
 * 
 * @param p_input_str Pointer to first char of C-String to be remapped.
 * @param input_size Size_t count of bytes/chars in the input string.
 * @param p_map_chr Pointer to the character mapping function to be used.
 */
void strn_chr_remap(char* const p_input_str, const size_t input_size,
	chr_remap_f p_map_chr);
void str_chr_remap(char* const p_input_str, chr_remap_f p_map_chr);

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
 * @brief Returns a new heap string cloned from haystack with all needle
 *        instances in that haystack replaced with value instances.
 * 
 * @param p_haystack C String to match in and read from.
 * @param haystack_size Size of the haystack string in characters/bytes.
 * @param p_needle C String pattern to match inside of haystack string.
 * @param needle_size Size of the pattern string in characters/bytes.
 * @param p_value C String to replace needle matches with in haystack.
 * @param value_size Size fo value string in characters/bytes.
 * @param p_strncmp Optional function pointer to strncmp signature for matching
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
char* strn_clone_substitute_7(
	const char* const p_haystack, const size_t haystack_size,
	const char* const p_needle, const size_t needle_size,
	const char* const p_value, const size_t value_size,
	int (*p_strncmp)(const char* p_s1, const char* p_s2, const size_t n));
char* strn_clone_substitute(
	const char* const p_haystack, const size_t haystack_size,
	const char* const p_needle, const size_t needle_size,
	const char* const p_value, const size_t value_size);
char* str_clone_substitute_4(const char* const p_haystack,
	const char* const p_needle, const char* const p_value,
	int (*p_strncmp)(const char* p_s1, const char* p_s2, const size_t n));
char* str_clone_substitute(const char* const p_haystack,
	const char* const p_needle, const char* const p_value);

/** Doxygen
 * @brief Separates the first found C string from a heap buffer of a known size
 * 
 * @param pp_buffer Pointer to heap buffer to have C string value removed from.
 * @param buffer_size Pointer to the size of the allocated heap buffer. Changes
 * 
 * @return Returns heap string on success and updates heap buffer/size in place
 *         Returns NULL otherwise leaving the heap buffer and size unchanged.
 */
char* pop_str_from_heap(uint8_t** const pp_buffer,
	size_t* const p_buffer_size);


/** Doxygen
 * @brief Returns a new heap C string from a formater and optionally values.
 * 
 * @param p_format C formatter string to be used.
 * @param args (Optional) Variadic arguments list.
 * 
 * @return Returns heap string on success. Returns NULL otherwise.
 */
char* vstr_format(const char* const p_format, va_list args);
char* str_format(const char* const p_format, ...);

typedef int (*str_fprint_f)(FILE* const, const void* const, ...);

/** Doxygen
 * @brief Calls a specified file print function with (a) specified value(s) and
 *        saves the printed output to a new returned heap string buffer.
 * 
 * @param fprint_f Function to call that will print a value to a FILE
 * @param p_value Pointer to the value that will be passed to the fprint funct.
 */
char* str_from_fprint(str_fprint_f fprint_f, const void* const p_value, ...);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_STRINGS_H
