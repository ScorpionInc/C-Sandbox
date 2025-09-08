// si_pthread_mutex.c
#include "si_pthread_mutex.h"

#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)

pthread_mutexattr_t* si_mutexattr_new()
{
	pthread_mutexattr_t* p_new = calloc(1u, sizeof(pthread_mutexattr_t));
	if(NULL == p_new)
	{
		goto END;
	}
	const int attr_init_result = pthread_mutexattr_init(p_new);
	if(0 != attr_init_result)
	{
		goto CLEAN;
	}
	const int settype_result = pthread_mutexattr_settype(
		p_new, PTHREAD_MUTEX_NORMAL
	);
	if(0 != settype_result)
	{
		pthread_mutexattr_destroy(p_new);
		goto CLEAN;
	}
	goto END;
CLEAN:
	if(NULL != p_new)
	{
		free(p_new);
		p_new = NULL;
	}
END:
	return p_new;
}

void si_mutexattr_destroy(pthread_mutexattr_t** const pp_attr)
{
	if(NULL == pp_attr)
	{
		goto END;
	}
	if(NULL == *pp_attr)
	{
		// Already Freed
		goto END;
	}
	pthread_mutexattr_destroy(*pp_attr);
	free(*pp_attr);
	*pp_attr = NULL;
END:
	return;
}

pthread_mutex_t* si_mutex_new_1(const pthread_mutexattr_t* const p_attr)
{
	pthread_mutex_t* p_new = NULL;
	if(NULL == p_attr)
	{
		goto END;
	}
	p_new = calloc(1u, sizeof(pthread_mutex_t));
	if(NULL == p_new)
	{
		goto END;
	}
	const int init_results = pthread_mutex_init(p_new, p_attr);
	if(0 != init_results)
	{
		free(p_new);
		p_new = NULL;
	}
END:
	return p_new;
}
pthread_mutex_t* si_mutex_new()
{
	pthread_mutex_t* p_result = NULL;
	pthread_mutexattr_t* p_attr = si_mutexattr_new();
	if(NULL == p_attr)
	{
		goto END;
	}
	p_result = si_mutex_new_1(p_attr);
	si_mutexattr_destroy(&p_attr);
END:
	return p_result;
}

void si_mutex_destroy(pthread_mutex_t** const pp_mutex)
{
	if(NULL == pp_mutex)
	{
		goto END;
	}
	if(NULL == *pp_mutex)
	{
		// Already freed
		goto END;
	}
	free(*pp_mutex);
	*pp_mutex = NULL;
END:
	return;
}

#endif // defined(__APPLE__) || defined(__linux__) || defined(__unix__)