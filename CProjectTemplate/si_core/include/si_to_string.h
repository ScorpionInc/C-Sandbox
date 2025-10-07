/* si_to_string.h - 20251007 */

#include <stdarg.h> // ...
#include <stdbool.h> // bool, false, true
#include <stddef.h> // size_t
#include <stdlib.h> // calloc(), free()
#include <stdint.h> // int8_t
#include <stdio.h> // snprintf()
#include <string.h> // strnlen()
#include <limits.h> // INT_MAX
#include <math.h> // pow()

#ifndef SI_TO_STRING_H
#define SI_TO_STRING_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/** Doxygen
 * @brief Allocates a heap string to hold the formatted output from snprintf().
 * 
 * @param max_str_len Size of the buffer to allocate to hold formatted output.
 * @param format_str C formatting string to be used with snprintf().
 * @param args Optional variadic arguments to be passed to snprintf().
 * 
 * @return Returns C string heap pointer on success. Returns NULL otherwise.
 */
char* vto_string(const size_t max_str_len, const char* const format_str, va_list args);
char* to_string(const size_t max_str_len, const char* const format_str, ...);

/** Doxygen
 * @brief Shrinks size of heap string to fit it's first NULL character.
 * 
 * @param pp_str Pointer to the string heap pointer to be shrunk.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool fit_str(char** const pp_str);

/** Doxygen
 * @brief Converts primitive char value to a heap string.
 * 
 * @param value Value to be converted into a heap string.
 * 
 * @return Returns heap string on success. Returns NULL otherwise.
 */
char* char_to_string(const char value);
/** Doxygen
 * @brief Converts primitive bool value to a heap string.
 * 
 * @param value Value to be converted into a heap string.
 * 
 * @return Returns heap string on success. Returns NULL otherwise.
 */
char* bool_to_string(const bool value);

/** Doxygen
 * @brief Converts a signed integer value to a heap string.
 * 
 * @param p_value Pointer to integer value to be converted into a heap string.
 * @param size Number of bytes in the value type to be converted.
 * 
 * @return Returns heap string on success. Returns NULL otherwise.
 */
char* signed_to_string(const void* const p_value, const size_t size);

// Inline support functions for all primitive signed integer types.
char* short_to_string(const short value);
char* int_to_string(const int value);
char* long_to_string(const long int value);
char* llong_to_string(const long long int value);

/** Doxygen
 * @brief Converts an unsigned integer value to a heap string.
 * 
 * @param p_value Pointer to integer value to be converted into a heap string.
 * @param size Number of bytes in the value type to be converted.
 * 
 * @return Returns heap string on success. Returns NULL otherwise.
 */
char* unsigned_to_string(const void* const p_value, const size_t size);

// Inline support functions for all primitive unsigned integer types.
char* ushort_to_string(const unsigned short value);
char* uint_to_string(const unsigned int value);
char* ulong_to_string(const unsigned long int value);
char* ullong_to_string(const unsigned long long int value);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_TO_STRING_H