// si_pthread_mutex.h - 20250908
// Shared common functions for working with pthread mutexs

#ifdef __linux__

#ifndef _POSIX_C_SOURCE
// Define default minimum POSIX Feature version
#define _POSIX_C_SOURCE 200809L
#endif//_POSIX_C_SOURCE

#ifndef _GNU_SOURCE
// Include GNU Sources
#define _GNU_SOURCE
#endif//_GNU_SOURCE

#include <pthread.h>

#include <stdlib.h>

/** Doxygen
 * @brief Generates and initializes a new heap attribute struct on the heap.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
pthread_mutexattr_t* si_mutexattr_new();

/** Doxygen
 * @brief Destroys and frees mutex attribute struct from a heap pointer.
 * 
 * @param pp_attr Pointer to heap of pthread_mutexattr_t to be freed/destroyed.
 */
void si_mutexattr_destroy(pthread_mutexattr_t** const pp_attr);

/** Doxygen
 * @brief Generates and initializes a new heap mutex pointer with attributes.
 * 
 * @param p_attr Pointer to pthread_mutexattr_t struct for initialization.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
pthread_mutex_t* si_mutex_new_1(const pthread_mutexattr_t* const p_attr);
pthread_mutex_t* si_mutex_new();

/** Doxygen
 * @brief Destroys and frees mutex struct from a heap pointer.
 * 
 * @param pp_mutex Pointer to heap of pthread_mutex_t to be freed/destroyed.
 */
void si_mutex_destroy(pthread_mutex_t** const pp_mutex);

#endif // __linux__