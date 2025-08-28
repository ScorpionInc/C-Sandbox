//si_tasker.c
#include "si_tasker.h"

size_t si_cpu_core_count()
{
	size_t count = 1u;
#ifdef _WIN32
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	count = sys_info.dwNumberOfProcessors;
#elif defined(__APPLE__) || defined(__linux__) || defined(__unix__)
	count = sysconf(_SC_NPROCESSORS_ONLN);
#else
	// Assumes this is a single-core processor.
#endif
	// End
END:
	return count;
}

static void* si_tasker_runner(void* const p_arg)
{
	if(NULL == p_arg)
	{
		goto END;
	}
	si_tasker_t* p_tasker = (si_tasker_t*)p_arg;
	int status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(status != 0)
	{
		// Failed to enable cancellation states.
		goto END;
	}
	pthread_t thisThread = pthread_self();
	// Start of main thread loop
	while(0u < p_tasker->running_signal)
	{
		// Valgrind may freeze without a sleep() or --fair-sched=yes
		pthread_testcancel();
		si_task_t* next_task = si_tasker_next_task(p_tasker);
		if(NULL == next_task)
		{
			pthread_testcancel();
			int standoff = (rand() % (SI_TASKER_MAX_STANDOFF + 1));
			usleep(standoff);
			pthread_testcancel();
			continue;
		}
		// Start Task
		const bool is_complete = si_task_is_complete(next_task);
		const bool is_running = si_task_is_running(next_task);
		if((is_complete) || (is_running))
		{
			si_task_free(next_task);
			free(next_task);
			next_task = NULL;
			continue;
		}
		next_task->field |= SI_TASK_RUNNING;
		si_task_start(next_task);
		next_task->field &= (~SI_TASK_RUNNING);
		const bool returns_value = si_task_returns_value(next_task);
		const bool is_looping = si_task_is_looping(next_task);
		if(returns_value)
		{
			//!TODO Move results into tasker for retreival
		}
		if(is_looping)
		{
			// Requeue task at current priority.
			si_tasker_enqueue_task(p_tasker, next_task);
		}
		else
		{
			// Handle Completed Task
			next_task->field |= SI_TASK_COMPLETE;
		}
		// Cleanup
		si_task_free(next_task);
		free(next_task);
		next_task = NULL;
		pthread_testcancel();
	}
	// End
END:
	pthread_exit(p_arg);
}

void si_tasker_init(si_tasker_t* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	const size_t priority_capacity = SI_TASK_PRIORITY_MAX + 1u;
	pthread_t_array_init_2(&(p_tasker->pool), 0u);
	// Initialize priority queue locks
	pthread_mutex_t_array_init_2(&(p_tasker->locks), priority_capacity);
	for(size_t iii = 0u; iii < priority_capacity; iii++)
	{
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(&(p_tasker->locks), iii);
		if(NULL == p_lock)
		{
			break;
		}
		*p_lock = (pthread_mutex_t){0};
		const int init_queue_lock = pthread_mutex_init(p_lock, NULL);
		if(0 != init_queue_lock)
		{
			break;
		}
	}
	// TODO Add a way to define a shared realloc_settings struct
	// Initialize task queues
	si_queue_t_array_init_2(&(p_tasker->tasks), priority_capacity);
	for(size_t iii = 0u; iii < priority_capacity; iii++)
	{
		si_queue_t* p_queue = si_queue_t_array_at(&(p_tasker->tasks), iii);
		if(NULL == p_queue)
		{
			break;
		}
		si_queue_init(p_queue, sizeof(si_task_t));
	}
	p_tasker->results_lock = (pthread_mutex_t){0};
	const int init_results_lock = pthread_mutex_init(
		&(p_tasker->results_lock), NULL
	);
	if(0 != init_results_lock)
	{
		goto END;
	}
	p_tasker->results = (si_map_t){0};
	si_map_init(&(p_tasker->results));
	// End
END:
	return;
}

si_tasker_t* si_tasker_new()
{
	si_tasker_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_tasker_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_tasker_init(p_new);
END:
	return p_new;
}

void si_tasker_enqueue_task(si_tasker_t* const p_tasker,
	si_task_t* const p_task)
{
	// Validate
	if((NULL == p_tasker) || (NULL == p_task))
	{
		goto END;
	}
	// Begin
	const uint8_t priority = si_task_priority(p_task);
	pthread_mutex_t* const p_lock = pthread_mutex_t_array_at(
		&(p_tasker->locks), priority
	);
	if(NULL == p_lock)
	{
		goto END;
	}
	si_queue_t* const p_queue = si_queue_t_array_at(
		&(p_tasker->tasks), priority
	);
	if(NULL == p_queue)
	{
		goto END;
	}
	pthread_mutex_lock(p_lock);
	si_queue_enqueue(p_queue, p_task);
	pthread_mutex_unlock(p_lock);
	// End
END:
	return;
}

bool si_tasker_enqueue_func(si_tasker_t* const p_tasker,
	si_task_f const p_func)
{
	bool result = false;
	if((NULL == p_tasker) || (NULL == p_func))
	{
		goto END;
	}
	si_task_t* p_task = NULL;
	p_task = calloc(1u, sizeof(si_task_t));
	if(NULL == p_task)
	{
		goto END;
	}
	const uint8_t PRIORITY_AVG = ((SI_TASK_PRIORITY_MAX - SI_TASK_PRIORITY_MIN) / 2);
	p_task->field = PRIORITY_AVG;
	p_task->p_function = p_func;
	si_tasker_enqueue_task(p_tasker, p_task);
	si_task_free(p_task);
	free(p_task);
	p_task = NULL;
	result = true;
END:
	return result;
}

si_task_t* si_tasker_next_task(si_tasker_t* const p_tasker)
{
	si_task_t* result = NULL;
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	result = calloc(1u, sizeof(si_task_t));
	if(NULL == result)
	{
		goto END;
	}
	// From high priority to low priority.
	for(size_t iii = SI_TASK_PRIORITY_MAX; iii >= 0u; iii--)
	{
		// Get the next queue's lock.
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(
			&(p_tasker->locks), iii
		);
		if(NULL == p_lock)
		{
			// Invalid lock.
			break;
		}
		// Get the priority queue.
		si_queue_t* p_queue = si_queue_t_array_at(&(p_tasker->tasks), iii);
		if(NULL == p_queue)
		{
			// Invalid queue.
			break;
		}
		// Determine if this queue has a new task to be done.
		pthread_mutex_lock(p_lock);
		if(!si_queue_is_empty(p_queue))
		{
			si_queue_dequeue(p_queue, result);
			pthread_mutex_unlock(p_lock);
			goto END;
		}
		// Release queue lock.
		pthread_mutex_unlock(p_lock);
		// Prevent Underflow
		if(0u == iii)
		{
			break;
		}
	}
	// Failed to find task. Cleanup.
	free(result);
	result = NULL;
	// End
END:
	return result;
}

size_t si_tasker_count(const si_tasker_t* const p_tasker)
{
	size_t count = 0u;
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	const size_t priority_capacity = SI_TASK_PRIORITY_MAX + 1u;
	for(size_t iii = 0u; iii < priority_capacity; iii++)
	{
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(
			&(p_tasker->locks), iii
		);
		if(NULL == p_lock)
		{
			continue;
		}
		si_queue_t* p_queue = si_queue_t_array_at(&(p_tasker->tasks), iii);
		if(NULL == p_queue)
		{
			continue;
		}
		pthread_mutex_lock(p_lock);
		const uint8_t next_count = si_queue_count(p_queue);
		pthread_mutex_unlock(p_lock);
		count += next_count;
	}
	// End
END:
	return count;
}

volatile bool si_tasker_is_running(const si_tasker_t* const p_tasker)
{
	volatile bool result = false;
	if(NULL == p_tasker)
	{
		goto END;
	}
	result = (0 != p_tasker->running_signal);
END:
	return result;
}

void si_tasker_clear_tasks(si_tasker_t* const p_tasker)
{
	if(NULL == p_tasker)
	{
		goto END;
	}
	const size_t priority_capacity = (SI_TASK_PRIORITY_MAX + 1u);
	for(size_t iii = 0u; iii < priority_capacity; iii++)
	{
		if(iii >= p_tasker->tasks.capacity)
		{
			break;
		}
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(
			&(p_tasker->locks), iii
		);
		if(NULL == p_lock)
		{
			break;
		}
		si_queue_t* p_queue = si_queue_t_array_at(&(p_tasker->tasks), iii);
		if(NULL == p_queue)
		{
			continue;
		}
		si_task_t* p_task = NULL;
		p_task = calloc(1u, sizeof(si_task_t));
		if(NULL == p_task)
		{
			break;
		}
		pthread_mutex_lock(p_lock);
		size_t new_size = si_queue_dequeue(p_queue, p_task);
		while(0 < new_size)
		{
			si_task_free_at(&p_task);
			new_size = si_queue_dequeue(p_queue, p_task);
		}
		si_task_free_at(&p_task);
		pthread_mutex_unlock(p_lock);
	}
END:
	return;
}

void si_tasker_start_2(si_tasker_t* const p_tasker, const size_t thread_count)
{
	if(NULL == p_tasker)
	{
		goto END;
	}
	const volatile bool is_running = si_tasker_is_running(p_tasker);
	if(true == is_running)
	{
		goto END;
	}
	// Initialize thread pool
	p_tasker->running_signal++;
	pthread_t_array_resize(&(p_tasker->pool), thread_count);
	for(size_t iii = 0u; iii < thread_count; iii++)
	{
		pthread_t* p_thread = pthread_t_array_at(&(p_tasker->pool), iii);
		if(NULL == p_thread)
		{
			break;
		}
		pthread_create(p_thread, NULL, si_tasker_runner, (void*)p_tasker);
	}
	// End
END:
	return;
}
inline void si_tasker_start(si_tasker_t* const p_tasker)
{
	const size_t cpu_core_count = si_cpu_core_count();
	si_tasker_start_2(p_tasker, cpu_core_count);
}

void si_tasker_await(si_tasker_t* const p_tasker)
{
	if(NULL == p_tasker)
	{
		goto END;
	}
	volatile bool is_running = si_tasker_is_running(p_tasker);
	while(is_running)
	{
		is_running = si_tasker_is_running(p_tasker);
		sleep(3);
	}
	si_tasker_stop(p_tasker);
END:
	return;
}

void si_tasker_stop(si_tasker_t* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	if(0 >= p_tasker->running_signal)
	{
		// Already stopped.
		goto END;
	}
	p_tasker->running_signal--;
	const size_t thread_count = p_tasker->pool.capacity;
	bool timeout_created = true;
	struct timespec timeout = {0};
	const int gettime_result = clock_gettime(CLOCK_REALTIME, &timeout);
	if(0 != gettime_result)
	{
		timeout_created = false;
	}
	else
	{
		timeout.tv_sec += SI_TASKER_THREAD_TIMEOUT;
	}
	for(size_t iii = 0u; iii < thread_count; iii++)
	{
		pthread_t* p_thread = pthread_t_array_at(&(p_tasker->pool), iii);
		if(NULL == p_thread)
		{
			continue;
		}
		pthread_cancel(*p_thread);
		int timed_join_result = EXIT_SUCCESS;
		if(timeout_created)
		{
			timed_join_result = pthread_timedjoin_np(*p_thread, NULL, &timeout);
		}
		else
		{
			pthread_join(*p_thread, NULL);
		}
		if(ETIMEDOUT == timed_join_result)
		{
			pthread_kill(*p_thread, SIGKILL);
			pthread_join(*p_thread, NULL);
		}
	}
	pthread_t_array_resize(&(p_tasker->pool), 0u);
	// End
END:
	return;
}

void si_tasker_free(si_tasker_t* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	si_tasker_stop(p_tasker);
	pthread_t_array_free(&(p_tasker->pool));
	si_tasker_clear_tasks(p_tasker);
	for(size_t iii = 0u; iii < p_tasker->tasks.capacity; iii++)
	{
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(
			&(p_tasker->locks), iii
		);
		if(NULL == p_lock)
		{
			break;
		}
		si_queue_t* p_queue = si_queue_t_array_at(&(p_tasker->tasks), iii);
		if(NULL == p_queue)
		{
			continue;
		}
		pthread_mutex_lock(p_lock);
		si_queue_free(p_queue);
		pthread_mutex_unlock(p_lock);
		pthread_mutex_destroy(p_lock);
	}
	si_queue_t_array_free(&(p_tasker->tasks));
	pthread_mutex_t_array_free(&(p_tasker->locks));
	si_queue_t_array_free(&(p_tasker->tasks));
	si_map_free(&(p_tasker->results));
	// End
END:
	return;
}

void si_tasker_destroy(si_tasker_t** pp_tasker)
{
	if(NULL == pp_tasker)
	{
		goto END;
	}
	if(NULL == *pp_tasker)
	{
		// Already freed
		goto END;
	}
	si_tasker_free(*pp_tasker);
	free(*pp_tasker);
	pp_tasker = NULL;
END:
	return;
}

/** Doxygen
 * @brief Prints thread IDs from a thread ID array to a FILE stream.
 * 
 * @param p_file Pointer to the FILE to print to.
 * @param p_pool Pointer to the array of thread IDs to be read from.
 */
static void si_tasker_fprint_pool(FILE* const p_file,
	const pthread_t_array_t* const p_pool)
{
	if((NULL == p_file) || (NULL == p_pool))
	{
		goto END;
	}
	fprintf(p_file, "[\n");
	for(size_t iii = 0u; iii < p_pool->capacity; iii++)
	{
		pthread_t* p_thread = si_array_at(p_pool, iii);
		if(NULL == p_thread)
		{
			fprintf(p_file, "NULL");
		}
		else
		{
			fprintf(p_file, "%lu", *p_thread);
		}
		if(iii + 1u < p_pool->capacity)
		{
			fprintf(p_file, ", ");
		}
		if(5 == (iii % 6))
		{
			fprintf(p_file, "\n\t\t");
		}
	}
	fprintf(p_file, "]:%lu@%p", p_pool->capacity, p_pool);
END:
	return;
}

/** Doxygen
 * @brief Prints the mutex lock state of each lock in the array to FILE.
 * 
 * @param p_file Pointer to FILE stream to write to.
 * @param p_locks Pointer to the mutex array to read mutexs from.
 */
static void si_tasker_fprint_locks(FILE* const p_file,
	pthread_mutex_t_array_t* const p_locks)
{
	if((NULL == p_file) || (NULL == p_locks))
	{
		goto END;
	}
	fprintf(p_file, "[\n\t\t");
	for(size_t iii = 0u; iii < p_locks->capacity; iii++)
	{
		pthread_mutex_t* p_lock = si_array_at(p_locks, iii);
		if(NULL == p_lock)
		{
			fprintf(p_file, "NULL");
		}
		else
		{
			const int lock_status = pthread_mutex_trylock(p_lock);
			if(0 == lock_status)
			{
				pthread_mutex_unlock(p_lock);
			}
			fprintf(p_file, "%s",
				lock_status == EBUSY ? "locked" : "unlock");
		}
		if(iii + 1u < p_locks->capacity)
		{
			fprintf(p_file, ", ");
		}
		if(5 == (iii % 6))
		{
			fprintf(p_file, "\n\t\t");
		}
	}
	fprintf(p_file, "]:%lu@%p", p_locks->capacity, p_locks);
END:
	return;
}

/** Doxygen
 * @brief Prints the state of the task queues to FILE. *WARNING* ignores mutex.
 * 
 * @param p_file Pointer to the FILE stream to be written to.
 * @param p_tasks Pointer to the task queues array to read from.
 */
static void si_tasker_fprint_tasks(FILE* const p_file,
	const si_queue_t_array_t* const p_tasks)
{
	if((NULL == p_file) || (NULL == p_tasks))
	{
		goto END;
	}
	fprintf(p_file, "[\n\t\t");
	for(size_t iii = 0u; iii < p_tasks->capacity; iii++)
	{
		si_queue_t* p_queue = si_array_at(p_tasks, iii);
		if(NULL == p_queue)
		{
			fprintf(p_file, "NULL");
		}
		else
		{
			si_queue_fprint(p_file, p_queue);
		}
		if(iii + 1u < p_tasks->capacity)
		{
			fprintf(p_file, ", ");
		}
		if(1 == (iii % 2))
		{
			fprintf(p_file, "\n\t\t");
		}
	}
	fprintf(p_file, "]:%lu@%p", p_tasks->capacity, p_tasks);
END:
	return;
}

void si_tasker_fprint(FILE* const p_file, si_tasker_t* const p_tasker)
{
	if(NULL == p_file)
	{
		goto END;
	}
	if(NULL == p_tasker)
	{
		fprintf(p_file, "NULL");
		goto END;
	}
	fprintf(p_file, "{\n\trunning_signal: %s;\n\tpool : ",
		p_tasker->running_signal ? "true" : "false");
	si_tasker_fprint_pool(p_file, &(p_tasker->pool));
	fprintf(p_file, ";\n\tlocks: ");
	si_tasker_fprint_locks(p_file, &(p_tasker->locks));
	fprintf(p_file, ";\n\ttasks: ");
	for(size_t iii = 0u; iii < p_tasker->locks.capacity; iii++)
	{
		pthread_mutex_t* p_lock = NULL;
		p_lock = si_array_at(&(p_tasker->locks), iii);
		if(NULL == p_lock)
		{
			continue;
		}
		pthread_mutex_lock(p_lock);
	}
	si_tasker_fprint_tasks(p_file, &(p_tasker->tasks));
	for(size_t iii = 0u; iii < p_tasker->locks.capacity; iii++)
	{
		pthread_mutex_t* p_lock = NULL;
		p_lock = si_array_at(&(p_tasker->locks), iii);
		if(NULL == p_lock)
		{
			continue;
		}
		pthread_mutex_unlock(p_lock);
	}
	const int results_status = pthread_mutex_trylock(&(p_tasker->results_lock));
	if(0 == results_status)
	{
		pthread_mutex_unlock(&(p_tasker->results_lock));
	}
	fprintf(p_file, ";\n\tresults_lock: %s",
		results_status == EBUSY ? "locked" : "unlock");
	fprintf(p_file, ";\n\tresults     : ");
	pthread_mutex_lock(&(p_tasker->results_lock));
	si_map_fprint(p_file, &(p_tasker->results));
	pthread_mutex_unlock(&(p_tasker->results_lock));
	fprintf(p_file, ";\n}@%p", p_tasker);
	fflush(p_file);
END:
	return;
}