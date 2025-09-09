/* threadpool.h - 20250904
*/

#ifdef _WIN32

#include <processthreadsapi.h> // CreateThread
#include <sysinfoapi.h> // GetSystemInfo(), SYSTEM_INFO

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
#include <stdatomic.h> // atomic_bool
#include <stdbool.h> //bool, false, true
#include <time.h> // timespec, time_t, clock_gettime()

#include "si_parray.h" // si_parray_t
#include "si_priority_queue.h" // si_priority_queue_t
#include "si_pthread_mutex.h" // si_mutexattr_new(), si_mutex_new_1()

#define SI_THREADPOOL_TASK_ID_INVALID (SIZE_MAX)
#define SI_THREADPOOL_PRIORITY_MIN (0u)
#define SI_THREADPOOL_DEFAULT_PRIORITY_COUNT (1u)
#define SI_THREADPOOL_DEFAULT_JOIN_TIMEOUT (100)

#ifndef SI_THREADPOOL_H
#define SI_THREADPOOL_H

size_t si_cpu_core_count();

typedef void* (*p_task_f)(void*);

typedef struct si_threadpool_t
{
	pthread_mutex_t task_counter_lock;
	pthread_mutex_t pool_lock;
	pthread_mutex_t results_lock;
	volatile atomic_bool is_running;
	volatile _Atomic size_t task_counter;
	si_parray_t pool;
	si_parray_t results;
	si_priority_queue_t queue;
} si_threadpool_t;

void si_threadpool_init_2(si_threadpool_t* const p_pool,
	const size_t priority_count);
void si_threadpool_init  (si_threadpool_t* const p_pool);

si_threadpool_t* si_threadpool_new_1(const size_t priority_count);
si_threadpool_t* si_threadpool_new  ();

bool si_threadpool_has_result(si_threadpool_t* const p_pool,
	const size_t task_id);

size_t si_threadpool_enqueue_5(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot,
	const size_t priority);
size_t si_threadpool_enqueue_4(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot);
size_t si_threadpool_enqueue_3(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter);
size_t si_threadpool_enqueue  (si_threadpool_t* const p_pool,
	p_task_f const p_task);

void* si_threadpool_pop_results(si_threadpool_t* const p_pool,
	const size_t task_id);

void* si_threadpool_await_results(si_threadpool_t* const p_pool,
	const size_t task_id);

void si_threadpool_await(si_threadpool_t** const pp_pool);

void si_threadpool_start_2(si_threadpool_t* const p_pool,
	const size_t thread_count);
void si_threadpool_start  (si_threadpool_t* const p_pool);

void si_threadpool_stop_2(si_threadpool_t* const p_pool,
	const time_t timeout);
void si_threadpool_stop  (si_threadpool_t* const p_pool);

void si_threadpool_free(si_threadpool_t* const p_pool);

void si_threadpool_destroy(si_threadpool_t** const pp_pool);

#endif // SI_THREADPOOL_H
