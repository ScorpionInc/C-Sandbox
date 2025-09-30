/* si_mutex.h
 * Created: 20250908
 * Updated: 20250930
 * Purpose: Generalize mutex functions for better cross-platform support.
 */

#define SI_PTHREAD_SUCCESS (0)
#define SI_PTHREAD_ERROR   (-1)

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)

#ifndef _POSIX_C_SOURCE
// Define default minimum POSIX Feature version
#define _POSIX_C_SOURCE 200809L
#endif//_POSIX_C_SOURCE

#ifndef _GNU_SOURCE
// Include GNU Sources
#define _GNU_SOURCE
#endif//_GNU_SOURCE

#include <pthread.h>
#define SI_PTHREAD_MUTEX_DEFAULT_TYPE (PTHREAD_MUTEX_ERRORCHECK)
#else
#warning Unknown/Unsupported OS
#endif // OS Specific Includes and Defines

// OS Specific feature flags must go before standard includes
#include <errno.h> // EBUSY, errno
#include <stdlib.h> // calloc(), free()


#ifndef SI_MUTEX_H
#define SI_MUTEX_H

#ifdef _WIN32
#define SI_COND_STATIC_INIT CONDITION_VARIABLE_INIT
typedef CONDITION_VARIABLE si_cond_t;
#define si_cond_init(c) InitializeConditionVariable(c)
#define si_cond_wait(c, m) SleepConditionVariableCS(c, m, INFINITE)
#define si_cond_signal(c) WakeConditionVariable(c)
#define si_cond_broadcast(c) WakeAllConditionVariable(c)
// Windows does not have an explicit DestroyConditionVariable function.
#define si_cond_destroy(c) // NOP


typedef CRITICAL_SECTION si_mutex_t;

#define si_mutex_lock(m) EnterCriticalSection(m)
#define si_mutex_unlock(m) LeaveCriticalSection(m)
#define si_mutex_free(m) DeleteCriticalSection(m)
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#define SI_COND_STATIC_INIT PTHREAD_COND_INITIALIZER
typedef pthread_cond_t si_cond_t;
#define si_cond_init(c) pthread_cond_init(c, NULL)
#define si_cond_wait(c, m) pthread_cond_wait(c, m)
#define si_cond_signal(c) pthread_cond_signal(c)
#define si_cond_broadcast(c) pthread_cond_broadcast(c)
#define si_cond_destroy(c) pthread_cond_destroy(c)


/** Doxygen
 * @brief Initializes and existing mutex attribute struct with optional type.
 * 
 * @param p_mutexattr Pointer to mutex attribute type to be initialized.
 * @param mutex_type Mutex type enum int value (Optional).
 */
void si_mutexattr_init_2(pthread_mutexattr_t* const p_mutexattr,
	const int mutex_type);
void si_mutexattr_init(pthread_mutexattr_t* const p_mutexattr);

/** Doxygen
 * @brief Generates and initializes a new heap attribute struct on the heap.
 * 
 * @param mutex_type Optional mutex type enumerator to specify the mutex type.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
pthread_mutexattr_t* si_mutexattr_new_1(const int mutex_type);
pthread_mutexattr_t* si_mutexattr_new();

#define si_mutexattr_free(m) (void)pthread_mutexattr_destroy(m)

/** Doxygen
 * @brief Destroys and frees mutex attribute struct from a heap pointer.
 * 
 * @param pp_attr Pointer to heap of pthread_mutexattr_t to be freed/destroyed.
 */
void si_mutexattr_destroy(pthread_mutexattr_t** const pp_attr);


typedef pthread_mutex_t si_mutex_t;

/** Doxygen
 * @brief Initializes value of an exisiting si_mutex_t of a specific type.
 * 
 * @param p_mutex Pointer to the si_mutex_t to be initialized.
 * @param mutex_type PThread mutex type enum type.
 * 
 * @return Returns int 0 on success. Returns int -1 otherwise.
 */
int si_mutex_init_2(si_mutex_t* const p_mutex, const int mutex_type);

/** Doxygen
 * @brief Allocates & initializes an exisiting si_mutex_t of a specific type.
 * 
 * @param mutex_type PThread mutex type enum type.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_mutex_t* si_mutex_new_1(const int mutex_type);

/** Doxygen
 * @brief Blocking mode locks a si_mutex_t by pointer.
 * 
 * @param p_mutex Pointer to the mutex to be locked.
 */
void si_mutex_lock(si_mutex_t* const p_mutex);

/** Doxygen
 * @brief Blocking mode unlocks a si_mutex_t by pointer.
 * 
 * @param p_mutex Pointer to the mutex to be unlocked.
 */
void si_mutex_unlock(si_mutex_t* const p_mutex);

/** Doxygen
 * @brief Frees the contents of an existing si_mutex_t (blocking)
 * 
 * @param p_mutex Pointer to the mutex to be freed.
 */
void si_mutex_free(si_mutex_t* const p_mutex);
#endif // OS Specific Defines and Prototypes


/** Doxygen
 * @brief Initializes an existing si_mutex_t at pointer to default values.
 * 
 * @param p_mutex Pointer to the si_mutex_t to be initialized.
 * 
 * @return Returns int 0 on success. Returns int -1 otherwise.
 */
int si_mutex_init(si_mutex_t* const p_mutex);

/** Doxygen
 * @brief Allocates and initializes a new si_mutex on the heap.
 * 
 * @return Returns heap pointer to allocated mutex on success or NULL otherwise
 */
si_mutex_t* si_mutex_new();

/** Doxygen
 * @brief Frees heap allocated mutex and any internal memory via free().
 * 
 * @param pp_mutex Pointer to mutex heap pointer to be destroyed.
 */
void si_mutex_destroy(si_mutex_t** const pp_mutex);

#endif//SI_MUTEX_H
