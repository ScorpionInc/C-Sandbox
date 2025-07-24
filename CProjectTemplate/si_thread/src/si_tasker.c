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

static void* si_tasker_runner(void* const p_tasker)
{
	// Begin
	int status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if(status != 0)
	{
		// Failed to enable cancellation states.
		goto END;
	}
	pthread_t thisThread = pthread_self();
	printf("Thread #%lu is running.\n", (unsigned long)thisThread);//!Debugging
	// Start of main thread loop
	while(1 == 1)
	{
		// Valgrind may freeze without a sleep() or --fair-sched=yes
		pthread_testcancel();
		usleep(1u);
		si_task* const next_task = si_tasker_next_task(p_tasker);
		if(NULL == next_task)
		{
			pthread_testcancel();
			usleep(999u);
			pthread_testcancel();
			continue;
		}
		// Start Task
		if(!si_task_is_complete(next_task))
		{
			next_task->field |= SI_TASK_RUNNING_MASK;
			si_task_start(next_task);
		}
		// Handle Completed Task
		next_task->field |= SI_TASK_COMPLETE_MASK;
		if(si_task_returns_value(next_task))
		{
			//!TODO Move results into tasker for retreival
		}
		if(si_task_is_looping(next_task))
		{
			// Requeue task at current priority.
			si_tasker_enqueue_task(p_tasker, next_task);
		}
		// Cleanup
		si_task_free(next_task);
	}
	// End
END:
	pthread_exit(p_tasker);
}

void si_tasker_new_2(si_tasker* const p_tasker, const size_t thread_count)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	const size_t priority_capacity = SI_TASK_PRIORITY_MAX + 1u;
	// Initialize thread pool
	pthread_t_array_new_2(&(p_tasker->pool), thread_count);
	for(size_t i = 0u; i < thread_count; i++)
	{
		pthread_t* p_thread = pthread_t_array_at(&(p_tasker->pool), i);
		if(NULL == p_thread)
		{
			break;
		}
		pthread_create(p_thread, NULL, si_tasker_runner, (void*)p_tasker);
	}
	// Initialize priority queue locks
	pthread_mutex_t_array_new_2(&(p_tasker->locks), priority_capacity);
	for(size_t i = 0u; i < priority_capacity; i++)
	{
		pthread_mutex_t* lock = pthread_mutex_t_array_at(&(p_tasker->locks), i);
		if(NULL == lock)
		{
			break;
		}
		*lock = (pthread_mutex_t){};
		pthread_mutex_init(lock, NULL);
	}
	// TODO Add a way to define a shared realloc_settings struct
	// Initialize task queues
	si_queue_array_new_2(&(p_tasker->tasks), priority_capacity);
	for(size_t i = 0u; i < priority_capacity; i++)
	{
		si_queue* p_queue = si_queue_array_at(&(p_tasker->tasks), i);
		if(NULL == p_queue)
		{
			break;
		}
		si_queue_new(p_queue, sizeof(si_task));
	}
	p_tasker->results_lock = (pthread_mutex_t){};
	p_tasker->results = (si_map_t){};
	si_map_new(&(p_tasker->results));
	// End
END:
	return;
}
void si_tasker_new(si_tasker* const p_tasker)
{
	// Default thread count = # of CPU Cores
	const size_t cpu_core_count = si_cpu_core_count();
	si_tasker_new_2(p_tasker, cpu_core_count);
}

void si_tasker_enqueue_task(si_tasker* const p_tasker, si_task* const p_task)
{
	//!TODO
	// Validate
	if((NULL == p_tasker) || (NULL == p_task))
	{
		goto END;
	}
	// Begin
	// End
END:
	return;
}

si_task* si_tasker_next_task(si_tasker* const p_tasker)
{
	si_task* result = NULL;
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	// Create new task struct on heap.
	result = calloc(1u, sizeof(si_task));
	if(NULL == result)
	{
		goto END;
	}
	// From high priority to low priority.
	for(size_t i = SI_TASK_PRIORITY_MAX; i >= 0u; i--)
	{
		// Get the next queue's lock.
		pthread_mutex_t* lock = pthread_mutex_t_array_at(&(p_tasker->locks),i);
		if(NULL == lock)
		{
			// Invalid lock.
			break;
		}
		// Wait for the queue to become available.
		pthread_mutex_lock(lock);
		// Get the priority queue.
		si_queue* p_queue = si_queue_array_at(&(p_tasker->tasks), i);
		if(NULL == p_queue)
		{
			// Invalid queue.
			pthread_mutex_unlock(lock);
			break;
		}
		// Determine if this queue has a new task to be done.
		if(!si_queue_is_empty(p_queue))
		{
			si_queue_dequeue(p_queue, result);
			pthread_mutex_unlock(lock);
			goto END;
		}
		// Release queue lock.
		pthread_mutex_unlock(lock);
		// Prevent Underflow
		if(0u == i)
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

inline size_t si_tasker_count(const si_tasker* const p_tasker)
{
	size_t count = 0u;
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	const size_t priority_capacity = SI_TASK_PRIORITY_MAX + 1u;
	for(size_t i = 0u; i < priority_capacity; i++)
	{
		si_queue* p_queue = si_queue_array_at(&(p_tasker->tasks), i);
		if(NULL == p_queue)
		{
			continue;
		}
		pthread_mutex_t* p_lock = pthread_mutex_t_array_at(
			&(p_tasker->locks), i
		);
		if(NULL == p_lock)
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

void si_tasker_start(si_tasker* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	// si_tasker_stop calls cancel on all threads going from 0->count together.
	// Thus joining any one thread should work. But the last seems most logical
	const size_t last_thread_index = p_tasker->pool.capacity - 1u;
	const pthread_t* p_last_thread = pthread_t_array_at(
		&(p_tasker->pool), last_thread_index
	);
	if(NULL == p_last_thread)
	{
		// Invalid thread.
		goto END;
	}
	pthread_join(*p_last_thread, NULL);
	// End
END:
	return;
}

void si_tasker_stop(si_tasker* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	const size_t thread_count = p_tasker->pool.capacity;
	for(size_t i = 0u; i < thread_count; i++)
	{
		pthread_t* p_thread = pthread_t_array_at(&(p_tasker->pool), i);
		if(NULL == p_thread)
		{
			continue;
		}
		pthread_cancel(*p_thread);
		pthread_join(*p_thread, NULL);
	}
	// End
END:
	return;
}

void si_tasker_free(si_tasker* const p_tasker)
{
	// Validate
	if(NULL == p_tasker)
	{
		goto END;
	}
	// Begin
	si_tasker_stop(p_tasker);
	pthread_t_array_free(&(p_tasker->pool));
	pthread_mutex_t_array_free(&(p_tasker->locks));
	const size_t priority_capacity = SI_TASK_PRIORITY_MAX + 1u;
	for(size_t i = 0u; i < priority_capacity; i++)
	{
		si_queue* p_queue = si_queue_array_at(&(p_tasker->tasks), i);
		if(NULL == p_queue)
		{
			continue;
		}
		si_queue_free(p_queue);
	}
	si_queue_array_free(&(p_tasker->tasks));
	si_map_free(&(p_tasker->results));
	// End
END:
	return;
}
