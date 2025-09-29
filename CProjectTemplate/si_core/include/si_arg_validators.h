/* si_arg_validators.h - 20250926
 */

#ifdef __linux__
#include <linux/limits.h> // PATH_MAX
#endif//__linux__

#include <stdbool.h> // bool, false, true
#include <stddef.h> // NULL
#include <stdint.h> // uint16_t

#include "si_io.h" // path_is_dir()

#ifndef SI_ARG_VALIDATORS_H
#define SI_ARG_VALIDATORS_H

/** Doxygen
 * @brief Validates a pointed at uint16_t is a port number
 * 
 * @param p_port Pointer to uint16_t port number to validate.
 * 
 * @return Returns stdbool true on valid. Returns false otherwise.
 */
bool port_validator(const void* const p_port);

/** Doxygen
 * @brief Validates file path is in a valid format.
 * 
 * @param p_path Pointer to C string path to validate.
 * 
 * @return Returns stdbool true if p_path is valid. Returns false otherwise.
 */
bool file_path_validator(const void* const p_path);

/** Doxygen
 * @brief Validates path is to a directory.
 * 
 * @param p_path Pointer to C string path to validate.
 * 
 * @return Returns stdbool true when p_path is valid. Returns false otherwise.
 */
bool dir_validator(const void* const p_path);

#endif//SI_ARG_VALIDATORS_H