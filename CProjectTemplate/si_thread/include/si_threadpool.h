/* si_threadpool.h
 * Language: C
 * Created : 20250904
 * Purpose : Structure for passing work functions to a thread pool of workers.
 */

#include "si_array.h" // si_array_t
#include "si_parray.h" // si_parray_t
#include "si_priority_queue.h" // si_priority_queue_t
#include "si_thread.h" // si_thread_t
#include "si_mutex.h" // si_mutex_new(), si_mutex_lock(), si_mutex_unlock()

#include <errno.h> // ETIMEDOUT
#include <stdatomic.h> // atomic_bool
#include <stdbool.h> //bool, false, true
#include <time.h> // timespec, time_t, clock_gettime()

#define SI_THREADPOOL_TASK_ID_INVALID (SIZE_MAX)
#define SI_THREADPOOL_PRIORITY_MIN (0u)
#define SI_THREADPOOL_DEFAULT_PRIORITY_COUNT (1u)

#ifdef _GNU_SOURCE
#define SI_THREADPOOL_DEFAULT_JOIN_TIMEOUT (100)
#endif//_GNU_SOURCE

#ifndef SI_THREADPOOL_H
#define SI_THREADPOOL_H

typedef void* (*p_task_f)(void*);

typedef struct si_threadpool_t
{
	si_mutex_t task_counter_lock;
	si_mutex_t pool_lock;
	si_mutex_t results_lock;
	volatile atomic_bool is_running;
	volatile _Atomic size_t task_counter;
	si_cond_t results_appended_signal;
	si_cond_t task_completed_signal;
	si_array_t pool;
	si_parray_t results;
	si_priority_queue_t queue;
} si_threadpool_t;

/** Doxygen
 * @brief Initializes an existing si_threadpool at pointer address.
 * 
 * @param p_pool Pointer to si_threadpool_t to be initialized.
 * @param priority_count Number of priority levels to allow.
 */
void si_threadpool_init_2(si_threadpool_t* const p_pool,
	const size_t priority_count);
void si_threadpool_init  (si_threadpool_t* const p_pool);

/** Doxygen
 * @brief Allocates and initializes a new si_threadpool_t on the heap.
 * 
 * @param priority_count Number of priority levels to allow.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_threadpool_t* si_threadpool_new_1(const size_t priority_count);
si_threadpool_t* si_threadpool_new  ();

/** Doxygen
 * @brief Determines if a task result is returned in a threadpool.
 * 
 * @param p_pool Pointer to the thread pool struct to check.
 * @param task_id UID of the task to check for result of.
 * 
 * @return Returns stdbool true if found. Returns false otherwise.
 */
bool si_threadpool_has_result(si_threadpool_t* const p_pool,
	const size_t task_id);

/** Doxygen
 * @brief Enqueues a new task into the priority queue of a thread pool struct.
 * 
 * @param p_pool Pointer to the thread pool struct to add task to.
 * @param p_task Function of the task to be executed. void* func(void* p_arg);
 * @param p_parameter Pointer parameter to pass to the task function on run.
 * @param one_shot Flag determines if automatically restarted. (new id ignored)
 * @param priority QoS size_t priority level of the task. 0->(priority_count-1)
 * 
 * @return Returns task UID on success. Otherwise SI_THREADPOOL_TASK_ID_INVALID
 */
size_t si_threadpool_enqueue_5(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot,
	const size_t priority);
size_t si_threadpool_enqueue_4(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot);
size_t si_threadpool_enqueue_3(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter);
size_t si_threadpool_enqueue  (si_threadpool_t* const p_pool,
	p_task_f const p_task);

/** Doxygen
 * @brief Pops a task result by UID from threadpool.
 * 
 * @param p_pool Pointer to the thread pool struct to pop from.
 * @param task_id UID of size_t to id the results to be returned.
 * 
 * @return Returns result pointer on Success. Returns NULL otherwise.
 */
void* si_threadpool_pop_results(si_threadpool_t* const p_pool,
	const size_t task_id);

/** Doxygen
 * @brief Blocks waiting for a task result to become ready. (Pops task result)
 * 
 * @param p_pool Pointer to the thread pool struct to read from.
 * @param task_id UID of size_t to id the results to be waited on.
 * 
 * @return Returns result pointer on Success. Returns NULL otherwise.
 */
void* si_threadpool_await_results(si_threadpool_t* const p_pool,
	const size_t task_id);

/** Doxygen
 * @brief Blocks waiting for the pool to be stopped or freed.
 * 
 * @param pp_pool Pointer to the heap si_threadpool struct to watch.
 */
void si_threadpool_await(si_threadpool_t** const pp_pool);

/** Doxygen
 * @brief Starts the threadpool's worker threads of a specified count.
 * 
 * @param p_pool Pointer to the thread pool struct to start threads in.
 * @param thread_count Target number of worker threads to start.
 */
void si_threadpool_start_2(si_threadpool_t* const p_pool,
	const size_t thread_count);
void si_threadpool_start  (si_threadpool_t* const p_pool);

/** Doxygen
 * @brief Stops a started/running threadpool at address with a timeout.
 * 
 * @param p_pool Pointer to the thread pool struct to stop threads of.
 * @param timeout millisec offset before kill is signaled. 0 = no timeout
 */
void si_threadpool_stop_2(si_threadpool_t* const p_pool,
	const uint32_t timeout);

/** Doxygen
 * @brief Blocking stops/joins a started/running threadpool at address.
 * 
 * @param p_pool Pointer to the thread pool struct to stop threads of.
 */
void si_threadpool_stop  (si_threadpool_t* const p_pool);

/** Doxygen
 * @brief Stops and frees the contents of an existing si_threadpool_t struct.
 * 
 * @param p_pool Pointer to the thread pool struct to have it's contents freed.
 */
void si_threadpool_free(si_threadpool_t* const p_pool);

/** Doxygen
 * @brief Stops & frees a heap si_threadpool_t struct by it's pointers address.
 * 
 * @param pp_pool Pointer to the thread pool struct's heap pointer to destroy.
 */
void si_threadpool_destroy(si_threadpool_t** const pp_pool);

#endif // SI_THREADPOOL_H
