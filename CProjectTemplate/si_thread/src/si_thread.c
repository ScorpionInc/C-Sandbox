// si_thread.c
#include "si_thread.h"

#if defined(__APPLE__)
// Adds support for Mac OS pthreads timedjoin

typedef struct timedjoin_t
{
	bool            was_joined;
	pthread_t       thread;
	pthread_mutex_t mutex;
	pthread_cond_t  signal;
	void**          pp_result;
} timedjoin_t;

static void* timed_joiner(void* p_arg)
{
	if (NULL == p_arg)
	{
		goto END;
	}
	timedjoin_t* p_data = (timedjoin_t*)p_arg;
	(void)pthread_join(p_data->thread, p_data->pp_result);
	int lock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_data->mutex));
	}
	p_data->was_joined = true;
	int unlock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_lock(&(p_data->mutex));
	}
	pthread_cond_signal(&(p_data->signal));
END:
	return p_arg;
}

int pthread_timedjoin(pthread_t thread, void** pp_result,
	struct timespec* p_timeout)
{
	int result = SI_PTHREAD_ERROR;
	if ((NULL == pp_result) || (NULL == p_timeout))
	{
		goto END;
	}
	pthread_t   thread = 0UL;
	timedjoin_t data   = {0};

	pthread_mutex_init(&(data.mutex) , NULL);
	pthread_cond_init (&(data.signal), NULL);
	
	int lock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(data.mutex));
	}

	result = pthread_create(&thread, NULL, timed_joiner, &data);
	if (SI_PTHREAD_SUCCESS != result)
	{
		goto END;
	}
	result = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	if (SI_PTHREAD_SUCCESS != cancelable)
	{
		goto END;
	}

	do
	{
		result = pthread_cond_timedwait(
			&(data.signal), &(data.mutex), p_timeout
		);
	} while ((result != ETIMEDOUT) && (true != data.was_joined));

	int unlock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(data.mutex));
	}
	pthread_cancel(thread);
	pthread_join(thread, NULL);

	pthread_cond_destroy(&(data.signal));
	pthread_mutex_destroy(&(data.mutex));
	result = (data.was_joined ? 0 : result);
END:
	return result;
}
#endif// __APPLE__

size_t si_cpu_core_count()
{
	size_t count = 1u;
#ifdef _WIN32
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	count = sys_info.dwNumberOfProcessors;
#elif SI_PTHREAD
	const long sysconf_result = sysconf(_SC_NPROCESSORS_ONLN);
	if (0L >= sysconf_result)
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

si_thread_func_return_t si_thread_join(si_thread_t* const p_thread)
{
	si_thread_func_return_t result = (si_thread_func_return_t)0;
	if (NULL == p_thread)
	{
		goto END;
	}
	const bool is_valid = si_thread_is_valid(*p_thread);
	if (true != is_valid)
	{
		goto END;
	}
#ifdef _WIN32
	(void)WaitForSingleObject(*p_thread, INFINITE);
	(void)GetExitCodeThread(*p_thread, &result);
#elif SI_PTHREAD
	(void)pthread_join(*p_thread, &result);
#endif// OS Specific implementation(s)
END:
	return result;
}

si_thread_func_return_t si_thread_timedjoin(si_thread_t* const p_thread,
	const uint32_t millisecs, const bool kill)
{
	si_thread_func_return_t result = (si_thread_func_return_t)0;
	if (NULL == p_thread)
	{
		goto END;
	}
	const bool is_valid = si_thread_is_valid(*p_thread);
	if (true != is_valid)
	{
		goto END;
	}
	if (0u == millisecs)
	{
		result = si_thread_join(p_thread);
		goto END;
	}
#ifdef _WIN32
	result = WaitForSingleObject(*p_thread, millisecs);
	if (WAIT_OBJECT_0 != result)
	{
		goto END;
	}
	(void)GetExitCodeThread(*p_thread, &result);
#elif SI_PTHREAD
	struct timespec abs_time = {0};
	const int get_time = clock_gettime(CLOCK_REALTIME, &abs_time);
	if (SI_PTHREAD_SUCCESS != get_time)
	{
		result = si_thread_join(p_thread);
		goto END;
	}
	const long milli_to_nano = 1000000L;
	const long to_nanosecs = (((long)millisecs) * milli_to_nano);
	// Handle Overflows
	if (to_nanosecs / ((long)millisecs) != milli_to_nano)
	{
		result = si_thread_join(p_thread);
		goto END;
	}
	abs_time.tv_nsec += to_nanosecs;
	const int ptj_result = pthread_timedjoin(*p_thread, &result, &abs_time);
	if(true != kill)
	{
		goto END;
	}
	if (ETIMEDOUT == ptj_result)
	{
		// Patience has expired. Time for thread murder :)
		const bool sent_kill = si_thread_kill(*p_thread);
		if (true == sent_kill)
		{
			result = si_thread_join(p_thread);
		}
	}
#endif// OS Specific implementation(s)
END:
	return result;
}
inline si_thread_func_return_t si_thread_timedjoin(si_thread_t* const p_thread,
	const uint32_t millisecs)
{
	// Default value of kill is false.
	return si_thread_timedjoin_3(p_thread, millisecs, false);
}
