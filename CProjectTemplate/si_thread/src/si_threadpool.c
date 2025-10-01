// si_threadpool.c
#include "si_threadpool.h"

size_t si_cpu_core_count()
{
	size_t count = 1u;
#ifdef _WIN32
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	count = sys_info.dwNumberOfProcessors;
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
	const long sysconf_result = sysconf(_SC_NPROCESSORS_ONLN);
	if(0L >= sysconf_result)
	{
		goto END;
	}
	count = (size_t)sysconf_result;
#else
	// Assumes this is a single-core processor.
#endif // OS Specific implementation(s)
END:
	return count;
}


// Local scope structure to hold task values in the queue.
typedef struct local_task_t
{
	bool one_shot;
	size_t priority;
	size_t task_id;
	void* p_param;
	p_task_f p_task;
	void* p_result;
} local_task_t;

/** Doxygen
 * @brief Allocates and initializes a new heap local_task_t struct on the heap.
 * 
 * @param task_id auto-incremented task UID used to reference task results.
 * @param p_task Function reference of format: void* (*p_task_f)(void*);
 * @param p_param Optional parameter pointer passed to function on execution.
 * @param one_shot Optional stdbool flag to allow automatic looping of tasks.
 * @param priority Optional QoS priority level of the task. Used for looping.
 * 
 * @return Returns a heap pointer on success. Returns NULL otherwise.
 */
static local_task_t* local_task_new_5(const size_t task_id,
	const p_task_f p_task, void* const p_param, const bool one_shot,
	const size_t priority)
{
	local_task_t* p_result = NULL;
	if ((NULL == p_task) || (SI_THREADPOOL_TASK_ID_INVALID == task_id))
	{
		goto END;
	}
	p_result = calloc(1u, sizeof(local_task_t));
	if (NULL == p_result)
	{
		goto END;
	}
	p_result->one_shot = one_shot;
	p_result->priority = priority;
	p_result->task_id = task_id;
	p_result->p_param = p_param;
	p_result->p_task = p_task;
	p_result->p_result = NULL;
END:
	return p_result;
}
static inline local_task_t* local_task_new_4(const size_t task_id,
	const p_task_f p_task, void* const p_param, const bool one_shot)
{
	// Default value of priority is SI_THREADPOOL_PRIORITY_MIN(0u)
	return local_task_new_5(
		task_id, p_task, p_param, one_shot, SI_THREADPOOL_PRIORITY_MIN
	);
}
static inline local_task_t* local_task_new_3(const size_t task_id,
	const p_task_f p_task, void* const p_param)
{
	// Default value of one_shot is true.
	return local_task_new_4(task_id, p_task, p_param, true);
}
static inline local_task_t* local_task_new(const size_t task_id,
	const p_task_f p_task)
{
	// Default value of p_param is NULL.
	return local_task_new_3(task_id, p_task, NULL);
}

/** Doxygen
 * @brief Returns a heap clone of a local_task_t struct.
 * 
 * @param p_local Pointer to local_task_t struct to be cloned.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
static local_task_t* local_task_clone(local_task_t* p_local)
{
	local_task_t* p_result = NULL;
	if (NULL == p_local)
	{
		goto END;
	}
	if ((NULL == p_local->p_task) ||
	    (SI_THREADPOOL_TASK_ID_INVALID == p_local->task_id))
	{
		goto END;
	}
	p_result = calloc(1u, sizeof(local_task_t));
	memcpy(p_result, p_local, sizeof(local_task_t));
END:
	return p_result;
}


/** Doxygen
 * @brief Main thread task worker loop.
 * 
 * @param p_param Pointer to si_threadpool_t that started this worker thread.
 * 
 * @return Returns the same pointer received.
 */
static void* si_threadpool_worker(si_threadpool_t* const p_param)
{
	si_threadpool_t* const p_pool = p_param;
	if (NULL == p_pool)
	{
		goto END;
	}
	local_task_t* p_task = NULL;
	bool is_running = atomic_load(&(p_pool->is_running));
	while (true == is_running)
	{
		p_task = si_priority_queue_dequeue(&(p_pool->queue));
		if (NULL == p_task)
		{
			sleep(1);
			goto CONTINUE;
		}
		if (NULL == p_task->p_task)
		{
			goto CONTINUE;
		}
		p_task->p_result = p_task->p_task(p_task->p_param);
		if (true != p_task->one_shot)
		{
			// Handles looping task(s)
			const size_t new_id = si_threadpool_enqueue_5(
				p_pool, p_task->p_task, p_task->p_param, 0, p_task->priority
			);
			if (SI_THREADPOOL_TASK_ID_INVALID == new_id)
			{
				// NOP ignore failed loop.
			}
		}
		else
		{
			// Parameter free is a delegated responsibility of caller.
			//free(p_task->p_param);
		}
		// Handle Results
		if (NULL != p_task->p_result)
		{
			local_task_t* p_results = local_task_clone(p_task);
			if (NULL == p_results)
			{
				goto CONTINUE;
			}
			// NULL out function to prevent unintentional re-execution
			p_results->p_task = NULL;
			p_results->p_param = NULL;
			si_mutex_lock(&(p_pool->results_lock));
			const size_t append_index = si_parray_append(
				&(p_pool->results), p_results
			);
			if (SIZE_MAX == append_index)
			{
				// Append failed.
				free(p_results);
				goto CONTINUE;
			}
			else
			{
				si_cond_signal(&(p_pool->results_appended_signal));
			}
			si_mutex_unlock(&(p_pool->results_lock));
		}
		si_cond_signal(&(p_pool->task_completed_signal));
CONTINUE:
		// Free and test for cancel/running
		if (NULL != p_task)
		{
			free(p_task);
			p_task = NULL;
		}
		is_running = atomic_load(&(p_pool->is_running));

		int cancel_result = pthread_setcancelstate(
			PTHREAD_CANCEL_ENABLE, NULL
		);
		if (0 != cancel_result)
		{
			// Failed to enable the cancellation state(s).
			goto END;
		}
		pthread_testcancel();
		cancel_result = pthread_setcancelstate(
			PTHREAD_CANCEL_DISABLE, NULL
		);
		if (0 != cancel_result)
		{
			// Failed to disable the cancellation state(s).
			goto END;
		}
	}
END:
	pthread_exit(p_param);
	return p_param;
}


void si_threadpool_init_2(si_threadpool_t* const p_pool,
	const size_t priority_count)
{
	if (NULL == p_pool)
	{
		goto END;
	}
	atomic_store(&(p_pool->is_running), 0);
	
	const int task_counter_results = si_mutex_init(
		&(p_pool->task_counter_lock)
	);
	if (SI_PTHREAD_SUCCESS != task_counter_results)
	{
		goto END;
	}
	const int pool_init_results = si_mutex_init(
		&(p_pool->pool_lock)
	);
	if (SI_PTHREAD_SUCCESS != pool_init_results)
	{
		goto END;
	}
	const int results_init_results = si_mutex_init(
		&(p_pool->results_lock)
	);
	if (SI_PTHREAD_SUCCESS != results_init_results)
	{
		goto END;
	}

	atomic_store(&(p_pool->task_counter), 0u);
	si_cond_init(&(p_pool->results_appended_signal));
	si_cond_init(&(p_pool->task_completed_signal));
	si_array_init_3(&(p_pool->pool), sizeof(pthread_t), 0u);
	si_parray_init_2(&(p_pool->results), 0u);
	p_pool->results.p_free_value = free;

	si_priority_queue_init(&(p_pool->queue), priority_count);
	p_pool->queue.p_free_value = free;
END:
	return;
}
inline void si_threadpool_init(si_threadpool_t* const p_pool)
{
	// Default value of priority_count(1u)
	si_threadpool_init_2(p_pool, SI_THREADPOOL_DEFAULT_PRIORITY_COUNT);
}

si_threadpool_t* si_threadpool_new_1(const size_t priority_count)
{
	void* p_results = NULL;
	if (0u >= priority_count)
	{
		goto END;
	}
	p_results = calloc(1u, sizeof(si_threadpool_t));
	if (NULL == p_results)
	{
		goto END;
	}
	si_threadpool_init_2(p_results, priority_count);
END:
	return p_results;
}
inline si_threadpool_t* si_threadpool_new()
{
	// Default value of priority_count(1u)
	return si_threadpool_new_1(SI_THREADPOOL_DEFAULT_PRIORITY_COUNT);
}

bool si_threadpool_has_result(si_threadpool_t* const p_pool,
	const size_t task_id)
{
	bool result = false;
	if ((NULL == p_pool) || (SI_THREADPOOL_TASK_ID_INVALID == task_id))
	{
		goto END;
	}

	si_mutex_lock(&(p_pool->results_lock));

	const size_t results_capacity = p_pool->results.array.capacity;
	for (size_t iii = 0u; iii < results_capacity; iii++)
	{
		local_task_t* p_task = si_parray_at(&(p_pool->results), iii);
		if (NULL == p_task)
		{
			continue;
		}
		if (p_task->task_id == task_id)
		{
			result = true;
			break;
		}
	}

	si_mutex_unlock(&(p_pool->results_lock));
END:
	return result;
}

/** Doxygen
 * @brief Local function to generate(increment) next UID for a new task.
 * 
 * @param p_pool Pointer to the si_threadpool_t struct to generate for.
 * 
 * @return Returns size_t on success. SI_THREADPOOL_TASK_ID_INVALID otherwise.
 */
static size_t si_threadpool_next_task_id(si_threadpool_t* const p_pool)
{
	size_t result = SI_THREADPOOL_TASK_ID_INVALID;
	if (NULL == p_pool)
	{
		goto END;
	}
	si_mutex_t* const p_lock = &(p_pool->task_counter_lock);
	si_mutex_lock(p_lock);
	result = atomic_fetch_add(&(p_pool->task_counter), 1u);
	si_mutex_unlock(p_lock);
	if (SI_THREADPOOL_TASK_ID_INVALID == result)
	{
		result = 0u;
	}
END:
	return result;
}

size_t si_threadpool_enqueue_5(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot,
	const size_t priority)
{
	size_t result = SI_THREADPOOL_TASK_ID_INVALID;
	if ((NULL == p_pool) || (NULL == p_task))
	{
		goto END;
	}
	const size_t priority_count = si_priority_queue_priority_count(
		&(p_pool->queue)
	);
	if (priority >= priority_count)
	{
		goto END;
	}
	// Get next task from threadpool UID
	const size_t task_id = si_threadpool_next_task_id(p_pool);
	if (SI_THREADPOOL_TASK_ID_INVALID == task_id)
	{
		goto END;
	}
	// Generate new local task struct to hold the required task information.
	local_task_t* p_local = local_task_new_4(
		task_id, p_task, p_parameter, one_shot
	);
	// Enqueue new local task struct by priority level.
	const bool did_enqueue = si_priority_queue_enqueue(
		&(p_pool->queue), p_local, priority
	);
	if (true != did_enqueue)
	{
		free(p_local);
		p_local = NULL;
		goto END;
	}
	result = task_id;
END:
	return result;
}
inline size_t si_threadpool_enqueue_4(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter, const bool one_shot)
{
	// Default value of priority is SI_THREADPOOL_PRIORITY_MIN (0u)
	return si_threadpool_enqueue_5(
		p_pool, p_task, p_parameter, one_shot, SI_THREADPOOL_PRIORITY_MIN
	);
}
inline size_t si_threadpool_enqueue_3(si_threadpool_t* const p_pool,
	p_task_f const p_task, void* const p_parameter)
{
	// Default value of one_shot is true.
	return si_threadpool_enqueue_4(
		p_pool, p_task, p_parameter, true
	);
}
inline size_t si_threadpool_enqueue(si_threadpool_t* const p_pool,
	p_task_f const p_task)
{
	// Default value of p_parameter is NULL
	return si_threadpool_enqueue_3(p_pool, p_task, NULL);
}

/** Doxygen
 * @brief Version of pop_results that doesn't lock for use of an await signal.
 * 
 * @param p_pool Pointer to si_threadpool_t to pop results value from.
 * @param task_id ID of the task to be found and removed/popped.
 * 
 * @return Returns value pointer on success. Returns NULL otherwise.
 */
static void* local_si_threadpool_pop_results(si_threadpool_t* const p_pool,
	const size_t task_id)
{
	void* p_result = NULL;
	if ((NULL == p_pool) || (SI_THREADPOOL_TASK_ID_INVALID == task_id))
	{
		goto END;
	}

	// *DANGER* This local function intentionally doesn't lock the mutex.
	const size_t results_capacity = p_pool->results.array.capacity;
	for (size_t iii = 0u; iii < results_capacity; iii++)
	{
		local_task_t* p_task = si_parray_at(&(p_pool->results), iii);
		if (NULL == p_task)
		{
			continue;
		}
		if (p_task->task_id == task_id)
		{
			p_result = p_task->p_result;
			si_parray_remove_at(&(p_pool->results), iii);
			break;
		}
	}
END:
	return p_result;
}

void* si_threadpool_pop_results(si_threadpool_t* const p_pool,
	const size_t task_id)
{
	void* p_result = NULL;
	if ((NULL == p_pool) || (SI_THREADPOOL_TASK_ID_INVALID == task_id))
	{
		goto END;
	}
	si_mutex_lock(&(p_pool->results_lock));
	p_result = local_si_threadpool_pop_results(p_pool, task_id);
	si_mutex_unlock(&(p_pool->results_lock));
END:
	return p_result;
}

void* si_threadpool_await_results(si_threadpool_t* const p_pool,
	const size_t task_id)
{
	if((NULL == p_pool) || (SI_THREADPOOL_TASK_ID_INVALID == task_id))
	{
		goto END;
	}
	void* p_results = NULL;
	while (NULL == p_results)
	{
		const bool is_running = atomic_load(&(p_pool->is_running));
		if (true != is_running)
		{
			break;
		}
		si_mutex_lock(&(p_pool->results_lock));
		p_results = local_si_threadpool_pop_results(p_pool, task_id);
		if (NULL == p_results)
		{
			si_cond_wait(&(p_pool->results_appended_signal), &(p_pool->results_lock));
		}
		si_mutex_unlock(&(p_pool->results_lock));
	}
END:
	return p_results;
}

void si_threadpool_await(si_threadpool_t** const pp_pool)
{
	if (NULL == pp_pool)
	{
		goto END;
	}
	if (NULL == *pp_pool)
	{
		goto END;
	}
	bool is_running = atomic_load(&((*pp_pool)->is_running));
	while (true == is_running)
	{
		if (NULL == pp_pool)
		{
			break;
		}
		if (NULL == *pp_pool)
		{
			break;
		}
		is_running = atomic_load(&((*pp_pool)->is_running));
		sleep(1);
	}
END:
	return;
}

void si_threadpool_start_2(si_threadpool_t* const p_pool,
	const size_t thread_count)
{
	if ((NULL == p_pool) || (0u >= thread_count))
	{
		goto END;
	}
	const bool is_running = atomic_load(&(p_pool->is_running));
	if (true == is_running)
	{
		goto END;
	}
	atomic_store(&(p_pool->is_running), true);

	si_mutex_lock(&(p_pool->pool_lock));
	
	if (0u < p_pool->pool.capacity)
	{
		// Already has threads!
		goto UNLOCK;
	}
	if (NULL != p_pool->pool.p_data)
	{
		si_array_free(&(p_pool->pool));
	}
	si_array_init_3(&(p_pool->pool), sizeof(pthread_t), thread_count);

	for (size_t iii = 0u; iii < thread_count; iii++)
	{
		pthread_t* p_thread = si_array_at(&(p_pool->pool), iii);
		if (NULL == p_thread)
		{
			break;
		}
		const int create_result = pthread_create(
			p_thread, NULL,
			(void* (*)(void*))si_threadpool_worker, (void*)p_pool
		);
		if (0 != create_result)
		{
			break;
		}
	}

UNLOCK:
	si_mutex_unlock(&(p_pool->pool_lock));
END:
	return;
}
inline void si_threadpool_start(si_threadpool_t* const p_pool)
{
	// Default value of thread_count is si_cpu_core_count()(Machine/OS dependent).
	const size_t core_count = si_cpu_core_count();
	si_threadpool_start_2(p_pool, core_count);
}

#ifdef _GNU_SOURCE
void si_threadpool_stop_2(si_threadpool_t* const p_pool,
	const time_t timeout_offset)
{
	if (NULL == p_pool)
	{
		goto END;
	}
	atomic_store(&(p_pool->is_running), false);

	// Wake up anything waiting on these signals so they can check the
	// current/new run state of the threadpool.
	si_cond_broadcast(&(p_pool->results_appended_signal));
	si_cond_broadcast(&(p_pool->task_completed_signal));

	bool timeout_created = true;
	struct timespec timeout = {0};
	const int gettime_result = clock_gettime(CLOCK_REALTIME, &timeout);
	if (0 != gettime_result)
	{
		timeout_created = false;
	}
	else
	{
		timeout.tv_sec += timeout_offset;
	}

	si_mutex_lock(&(p_pool->pool_lock));

	const size_t thread_count = p_pool->pool.capacity;
	for (size_t iii = 0u; iii < thread_count; iii++)
	{
		pthread_t* p_thread = si_array_at(&(p_pool->pool), iii);
		if (NULL == p_thread)
		{
			continue;
		}
		// We verify thread is cancelable when it was created.
		(void)pthread_cancel(*p_thread);

		int join_result = EXIT_FAILURE;
		if ((true == timeout_created) && (0 < timeout_offset))
		{
			join_result = pthread_timedjoin_np(*p_thread, NULL, &timeout);
		}
		else
		{
			join_result = pthread_join(*p_thread, NULL);
		}

		if (ETIMEDOUT == join_result)
		{
			// Join has timed out. Time for the murder signal :)
			const int sent_kill = pthread_kill(*p_thread, SIGKILL);
			if (SI_PTHREAD_SUCCESS == sent_kill)
			{
				(void)pthread_join(*p_thread, NULL);
			}
			else
			{
				// Couldn't send thread a kill signal so don't retry the join
				// as the thread's state was/remains unchanged.
			}
		}
		else
		{
			// Thread is already being joined (EBUSY, EDEADLK, EDEADLOCK).
			// Or an unknown fault has occurred (EFAULT).
		}
	}
	si_array_free(&(p_pool->pool));

	si_mutex_unlock(&(p_pool->pool_lock));
END:
	return;
}
inline void si_threadpool_stop(si_threadpool_t* const p_pool)
{
	// Default value of timeout is 0(blocks forever)
	si_threadpool_stop_2(p_pool, 0);
}
#else
void si_threadpool_stop(si_threadpool_t* const p_pool)
{
	if (NULL == p_pool)
	{
		goto END;
	}
	atomic_store(&(p_pool->is_running), false);

	// Wake up anything waiting on these signals so they can check the
	// current/new run state of the threadpool.
	si_cond_broadcast(&(p_pool->results_appended_signal));
	si_cond_broadcast(&(p_pool->task_completed_signal));

	si_mutex_lock(&(p_pool->pool_lock));

	const size_t thread_count = p_pool->pool.capacity;
	for (size_t iii = 0u; iii < thread_count; iii++)
	{
		pthread_t* p_thread = si_array_at(&(p_pool->pool), iii);
		if (NULL == p_thread)
		{
			continue;
		}
		// We verify thread is cancelable when it was created.
		(void)pthread_cancel(*p_thread);
		// If thread is invalid or already we joining continue.
		(void)pthread_join(*p_thread, NULL);
	}
	si_array_free(&(p_pool->pool));

	si_mutex_unlock(&(p_pool->pool_lock));
END:
	return;
}
#endif//_GNU_SOURCE

void si_threadpool_free(si_threadpool_t* const p_pool)
{
	if (NULL == p_pool)
	{
		goto END;
	}
	si_threadpool_stop_2(p_pool, SI_THREADPOOL_DEFAULT_JOIN_TIMEOUT);

	si_mutex_lock(&(p_pool->task_counter_lock));
	si_mutex_lock(&(p_pool->pool_lock));
	si_mutex_lock(&(p_pool->results_lock));

	si_parray_free(&(p_pool->results));
	si_priority_queue_free(&(p_pool->queue));
	si_cond_free(&(p_pool->results_appended_signal));
	si_cond_free(&(p_pool->task_completed_signal));

	si_mutex_unlock(&(p_pool->task_counter_lock));
	si_mutex_free(&(p_pool->task_counter_lock));
	si_mutex_unlock(&(p_pool->pool_lock));
	si_mutex_free(&(p_pool->pool_lock));
	si_mutex_unlock(&(p_pool->results_lock));
	si_mutex_free(&(p_pool->results_lock));
END:
	return;
}

void si_threadpool_destroy(si_threadpool_t** const pp_pool)
{
	if (NULL == pp_pool)
	{
		goto END;
	}
	if (NULL == *pp_pool)
	{
		// Already freed
		goto END;
	}
	si_threadpool_free(*pp_pool);
	free(*pp_pool);
	*pp_pool = NULL;
END:
	return;
}