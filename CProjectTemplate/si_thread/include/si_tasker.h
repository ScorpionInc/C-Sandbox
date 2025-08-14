//si_tasker.h
//Tasker is given a thread pool and allocates space to receive tasks.
//Tasker will attempt to keep each thread busy running tasks with the highest priority in a fifo order.

#ifdef _WIN32
#include <processthreadsapi.h> // CreateThread
#include <sysinfoapi.h> // GetSystemInfo
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#include <pthread.h>
#include <unistd.h> // sysconf, usleep
#endif

#include "si_map.h"
#include "si_queue.h"
#include "si_task.h"

#ifndef SI_TASKER_H
#define SI_TASKER_H

#define SI_TEMPLATE_TYPE pthread_t
#include "si_array.template"

#define SI_TEMPLATE_TYPE pthread_mutex_t
#include "si_array.template"

#define SI_TEMPLATE_TYPE si_queue_t
#include "si_array.template"

size_t si_cpu_core_count();

typedef struct si_tasker
{
	pthread_t_array_t pool;
	pthread_mutex_t_array_t locks;
	si_queue_t_array_t tasks;
	//!TODO
	pthread_mutex_t results_lock;
	si_map_t results;
} si_tasker;

void si_tasker_new_2(si_tasker* const p_tasker, const size_t thread_count);
void si_tasker_new(si_tasker* const p_tasker);

void si_tasker_enqueue_task(si_tasker* const p_tasker, si_task* const p_task);

si_task* si_tasker_next_task(si_tasker* const p_tasker);

size_t si_tasker_count(const si_tasker* const p_tasker);

void si_tasker_start(si_tasker* const p_tasker);

void si_tasker_stop(si_tasker* const p_tasker);

void si_tasker_free(si_tasker* const p_tasker);

#endif//SI_TASKER_H
