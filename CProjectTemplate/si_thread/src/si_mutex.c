// si_mutex.c
#include "si_mutex.h"

#ifdef _WIN32

int si_mutex_init(si_mutex_t* const p_mutex)
{
	int result = SI_PTHREAD_ERROR;
	if (NULL == p_mutex)
	{
		goto END;
	}
	__try
	{
		InitializeCriticalSection(p_result);
		result = SI_PTHREAD_SUCCESS;
	}
	__except
	(
		GetExceptionCode() == STATUS_NO_MEMORY ?
			EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH
	)
	{
		goto END;
	}
END:
	return result;
}

si_mutex_t* si_mutex_new()
{
	si_mutex_t* p_result = calloc(1u, sizeof(si_mutex_t));
	if (NULL == p_result)
	{
		goto END;
	}
	const int init_result = si_mutex_init(p_result);
	if (SI_PTHREAD_SUCCESS != init_result)
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
END:
	return p_result;
}

#elif SI_PTHREAD

void si_mutexattr_init_2(pthread_mutexattr_t* const p_mutexattr,
	const int mutex_type)
{
	if (NULL == p_mutexattr)
	{
		goto END;
	}
	const int init_result = pthread_mutexattr_init(p_mutexattr);
	if (SI_PTHREAD_SUCCESS != init_result)
	{
		goto END;
	}
	const int type_result = pthread_mutexattr_settype(p_mutexattr, mutex_type);
	if (SI_PTHREAD_SUCCESS != type_result)
	{
		si_mutexattr_free(p_mutexattr);
		goto END;
	}
END:
	return;
}
inline void si_mutexattr_init(pthread_mutexattr_t* const p_mutexattr)
{
	// Default value of mutex_type SI_PTHREAD_MUTEX_DEFAULT_TYPE (ERROR_CHECK)
	si_mutexattr_init_2(p_mutexattr, SI_PTHREAD_MUTEX_DEFAULT_TYPE);
}

pthread_mutexattr_t* si_mutexattr_new_1(const int mutex_type)
{
	pthread_mutexattr_t* p_result = calloc(1u, sizeof(pthread_mutexattr_t));
	if (NULL == p_result)
	{
		goto END;
	}
	si_mutexattr_init_2(p_result, mutex_type);
END:
	return p_result;
}
inline pthread_mutexattr_t* si_mutexattr_new()
{
	// Default value of mutex_type SI_PTHREAD_MUTEX_DEFAULT_TYPE (ERROR_CHECK)
	return si_mutexattr_new_1(SI_PTHREAD_MUTEX_DEFAULT_TYPE);
}

void si_mutexattr_destroy(pthread_mutexattr_t** const pp_attr)
{
	if (NULL == pp_attr)
	{
		goto END;
	}
	if (NULL == *pp_attr)
	{
		// Already Freed
		goto END;
	}
	si_mutexattr_free(*pp_attr);
	free(*pp_attr);
	*pp_attr = NULL;
END:
	return;
}


int si_mutex_init_2(si_mutex_t* const p_mutex, const int mutex_type)
{
	int result = SI_PTHREAD_ERROR;
	if (NULL == p_mutex)
	{
		goto END;
	}
	pthread_mutexattr_t* p_attr = si_mutexattr_new_1(mutex_type);
	if (NULL == p_attr)
	{
		goto END;
	}
	result = pthread_mutex_init(p_mutex, p_attr);
	si_mutexattr_destroy(&p_attr);
END:
	return result;
}
inline int si_mutex_init(si_mutex_t* const p_mutex)
{
	// Default value of mutex_type SI_PTHREAD_MUTEX_DEFAULT_TYPE (ERROR_CHECK)
	return si_mutex_init_2(p_mutex, SI_PTHREAD_MUTEX_DEFAULT_TYPE);
}

si_mutex_t* si_mutex_new_1(const int mutex_type)
{
	si_mutex_t* p_result = calloc(1u, sizeof(si_mutex_t));
	if (NULL == p_result)
	{
		goto END;
	}
	const int init_result = si_mutex_init_2(p_result, mutex_type);
	if (SI_PTHREAD_SUCCESS != init_result)
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
END:
	return p_result;
}
inline si_mutex_t* si_mutex_new()
{
	// Default value of mutex_type SI_PTHREAD_MUTEX_DEFAULT_TYPE (ERROR_CHECK)
	return si_mutex_new_1(SI_PTHREAD_MUTEX_DEFAULT_TYPE);
}

bool si_mutex_timedlock(si_mutex_t* const p_mutex, const uint32_t millisecs)
{
	bool result = false;
	if ((NULL == p_mutex) || (0u >= millisecs))
	{
		goto END;
	}
	struct timespec abs_time = {0};
	const int get_time = clock_gettime(CLOCK_REALTIME, &abs_time);
	if (SI_PTHREAD_SUCCESS != get_time)
	{
		goto END;
	}
	const long milli_to_nano = 1000000L;
	const long to_nanosecs = (((long)millisecs) * milli_to_nano);
	// Handle Overflows
	if (to_nanosecs / ((long)millisecs) != milli_to_nano)
	{
		goto END;
	}
	abs_time.tv_nsec += to_nanosecs;
	// Handle Overflows
	if (abs_time.tv_nsec < to_nanosecs)
	{
		goto END;
	}
	int timed_result = pthread_mutex_timedlock(p_mutex, &abs_time);
	result = (SI_PTHREAD_SUCCESS == timed_result);
END:
	return result;
}

void si_mutex_lock(si_mutex_t* const p_mutex)
{
	if (NULL == p_mutex)
	{
		goto END;
	}
	// Loop to handle spurious wakeups
	int lock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(p_mutex);
	}
END:
	return;
}

void si_mutex_unlock(si_mutex_t* const p_mutex)
{
	if (NULL == p_mutex)
	{
		goto END;
	}
	// Loop to handle spurious wakeups
	int unlock_result = SI_PTHREAD_ERROR;
	while (SI_PTHREAD_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(p_mutex);
	}
END:
	return;
}

void si_mutex_free(si_mutex_t* const p_mutex)
{
	if (NULL == p_mutex)
	{
		goto END;
	}
	int destroy_result = SI_PTHREAD_ERROR;
	// We could but intentionally don't try unlock here in order to destroy
	while (SI_PTHREAD_SUCCESS != destroy_result)
	{
		destroy_result = pthread_mutex_destroy(p_mutex);
	}
END:
	return;
}
#endif// SI_PTHREAD


void si_mutex_destroy(si_mutex_t** const pp_mutex)
{
	if (NULL == pp_mutex)
	{
		goto END;
	}
	if (NULL == *pp_mutex)
	{
		// Already Freed
		goto END;
	}
	si_mutex_free(*pp_mutex);
	free(*pp_mutex);
	*pp_mutex = NULL;
END:
	return;
}
