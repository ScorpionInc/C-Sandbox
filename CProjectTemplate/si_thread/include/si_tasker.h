//si_tasker.h
//Tasker is given a thread pool and allocates space to receive tasks.
//Tasker will attempt to keep each thread busy running tasks with the highest priority in a fifo order.

#ifdef _WIN32

#include <processthreadsapi.h> // CreateThread
#include <sysinfoapi.h> // GetSystemInfo

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
#include <signal.h> // pthread_kill()
#include <unistd.h> // sysconf, usleep

#endif // OS Test

#include <errno.h> // ETIMEDOUT
#include <time.h> // timespec, clock_gettime()

#include "si_map.h"
#include "si_queue.h"
#include "si_signals.h"
#include "si_task.h"

#define SI_TASKER_MAX_STANDOFF (10)
#define SI_TASKER_THREAD_TIMEOUT (60)

#ifndef SI_TASKER_H
#define SI_TASKER_H

#define SI_TEMPLATE_TYPE pthread_t
#include "si_array.template"

#define SI_TEMPLATE_TYPE pthread_mutex_t
#include "si_array.template"

#define SI_TEMPLATE_TYPE si_queue_t
#include "si_array.template"

size_t si_cpu_core_count();

typedef struct si_tasker_t
{
	volatile sig_atomic_t exit_signal;
	pthread_t_array_t pool;
	pthread_mutex_t_array_t locks;
	si_queue_t_array_t tasks;
	pthread_mutex_t results_lock;
	si_map_t results;
} si_tasker_t;

/** Doxygen
 * @brief Initializes an already allocated tasker structs values.
 * 
 * @param p_tasker Pointer to si_tasker_t struct to be initialized.
 */
void si_tasker_init(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Allocated/Initializes a new si_tasker_t struct on the heap.
 * 
 * @return On success, returns heap pointer. Otherwise returns NULL.
 */
si_tasker_t* si_tasker_new();

/** Doxygen
 * @brief Adds an existing task to the appropriate priority queue in tasker.
 * 
 * @param p_tasker Pointer to the si_tasker_t struct to be added to.
 * @param p_task Pointer to the si_task to be enqueued in the tasker.
 */
void si_tasker_enqueue_task(si_tasker_t* const p_tasker,
	si_task_t* const p_task);

/** Doxygen
 * @brief Allocates and initializes a new task from a function reference that
 *        is then enqueued into the provided si_taskers priority queue.
 * 
 * @param p_tasker Pointer to the si_tasker_t struct to be added to.
 * @param p_func Pointer to task function to create a si_task_t from.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
*/
bool si_tasker_enqueue_func(si_tasker_t* const p_tasker,
	si_task_f const p_func);

/** Doxygen
 * @brief Dequeues task from a queue based upon priority levels for thread pool
 * 
 * @param p_tasker Pointer to the si_tasker_t to pull task from.
 * 
 * @return Returns next heap task to be executed/handled on success. Otherwise
 *         returns NULL.
 */
si_task_t* si_tasker_next_task(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Returns the number of pending tasks in the queues.
 * 
 * @return Returns size_t count on success. SIZE_MAX on error.
 */
size_t si_tasker_count(const si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Checks the volatile atomic for the exit signal and returns state.
 * 
 * @return Returns stdbool true if pool is started. Returns false otherwise.
 */
volatile bool si_tasker_is_running(const si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Removes all waiting tasks from the priority queues in a tasker.
 * 
 * @param p_tasker Pointer to a si_tasker_t struct to have tasks cleared.
 */
void si_tasker_clear_tasks(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Starts worker threads in the thread pool to handle queued tasks.
 * 
 * @param p_tasker Pointer to the si_tasker_t to be started.
 * @param thread_count How many threads to startup in the tasker's thread pool.
 */
void si_tasker_start_2(si_tasker_t* const p_tasker, const size_t thread_count);
void si_tasker_start(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Waits for a started tasker struct to finish executing/be stopped.
 * 
 * @param p_tasker Pointer to the si_tasker_t to be waited upon.
 */
void si_tasker_await(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Sets the state of the tasker to stopped and joins worker threads.
 * 
 * @param p_tasker Pointer to the si_tasker_t to be stopped.
 */
void si_tasker_stop(si_tasker_t* const p_tasker);

/** Doxygen
 * @brief Frees allocated values inside of an existing si_tasker_t object.
 * 
 * @param p_tasker Pointer to the si_tasker_t struct to free contents of.
 */
void si_tasker_free(si_tasker_t* const p_tasker);

#endif//SI_TASKER_H
