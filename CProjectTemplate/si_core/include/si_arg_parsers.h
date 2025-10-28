/* si_arg_parsers.h
 * Language: C
 * Purpose : Created to provide common parsers for use with si_argparse.
 * Created : 20250926
 */

#include <errno.h>  // errno, ERANGE
#include <limits.h> // INT_MAX, UINT16_MAX
#include <stddef.h> // NULL
#include <stdint.h> // uint16_t

#include "si_strings.h" // str_clone_concat()

#ifndef SI_ARG_PARSERS_H
#define SI_ARG_PARSERS_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/** Doxygen
 * @brief Attempts to parse a uint16 port number from string.
 * 
 * @param p_str C String to parse port number from.
 * 
 * @return Returns heap pointer to port number on success. NULL otherwise.
 */
void* port_parser(const char* const p_str);

/** Doxygen
 * @brief Attempts to parse a string to a new heap path.
 * 
 * @param p_str C str to be parsed.
 * 
 * @return Heap pointer to new path on success. NULL otherwise.
 */
void* dir_parser(const char* const p_str);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_ARG_PARSERS_H
