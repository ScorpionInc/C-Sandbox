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

void si_tasker_init(si_tasker_t* const p_tasker);

si_tasker_t* si_tasker_new();

void si_tasker_enqueue_task(si_tasker_t* const p_tasker,
	si_task_t* const p_task);

bool si_tasker_enqueue_func(si_tasker_t* const p_tasker,
	si_task_f const p_func);

si_task_t* si_tasker_next_task(si_tasker_t* const p_tasker);

size_t si_tasker_count(const si_tasker_t* const p_tasker);

volatile bool si_tasker_is_running(const si_tasker_t* const p_tasker);

void si_tasker_clear_tasks(si_tasker_t* const p_tasker);

void si_tasker_start_2(si_tasker_t* const p_tasker, const size_t thread_count);
void si_tasker_start(si_tasker_t* const p_tasker);

void si_tasker_await(si_tasker_t* const p_tasker);

void si_tasker_stop(si_tasker_t* const p_tasker);

void si_tasker_free(si_tasker_t* const p_tasker);

#endif//SI_TASKER_H
