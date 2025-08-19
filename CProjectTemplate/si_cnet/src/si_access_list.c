//si_access_list.c

#include "si_access_list.h"

void si_accesslist_init_4(si_accesslist_t* const p_access,
	const bool is_blacklist, const bool is_ipv4,
	const si_realloc_settings_t* const p_settings)
{
	if(NULL == p_access)
	{
		goto END;
	}

	pthread_mutexattr_t mutex_attributes;
	if(SOCKET_SUCCESS != pthread_mutexattr_init(&mutex_attributes))
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutexattr_settype(
		&mutex_attributes, PTHREAD_MUTEX_RECURSIVE))
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_init(
		&(p_access->entries_lock), &mutex_attributes))
	{
        goto END;
    }
	if(SOCKET_SUCCESS != pthread_mutexattr_destroy(&mutex_attributes))
	{
		goto END;
	}

	p_access->is_blacklist = is_blacklist;
	p_access->is_ipv4 = is_ipv4;
	si_parray_init_2(&(p_access->entries), 0u);
	if(NULL != p_settings)
	{
		memcpy(&(p_access->settings), p_settings,
			sizeof(si_realloc_settings_t));
	}
END:
	return;
}
inline void si_accesslist_init(si_accesslist_t* const p_access,
	const bool is_blacklist, const bool is_ipv4)
{
	// Default value for p_settings = NULL(Default realloc settings)
	return si_accesslist_init_4(p_access, is_blacklist, is_ipv4, NULL);
}

si_accesslist_t* si_accesslist_new_3(const bool is_blacklist,
	const bool is_ipv4, const si_realloc_settings_t* const p_settings)
{
	si_accesslist_t* p_result = calloc(1u, sizeof(si_accesslist_t));
	if(NULL == p_result)
	{
		goto END;
	}
	si_accesslist_init_4(p_result, is_blacklist, is_ipv4, p_settings);
END:
	return p_result;
}
inline si_accesslist_t* si_accesslist_new(const bool is_blacklist,
	const bool is_ipv4)
{
	// Default value for p_settings = NULL(Default realloc settings)
	return si_accesslist_new_3(is_blacklist, is_ipv4, NULL);
}

bool si_accesslist_is_valid_at(si_accesslist_t* const p_access,
	const size_t index)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_access->entries_lock)))
	{
		goto END;
	}
	void* p_addr = si_parray_at(&(p_access->entries), index);
	result = sockaddr_is_valid(p_addr);
	pthread_mutex_unlock(&(p_access->entries_lock));
END:
	return result;
}

size_t si_accesslist_count(si_accesslist_t* const p_access)
{
	size_t result = SIZE_MAX;
	if(NULL == p_access)
	{
		goto END;
	}
	result++;
	for(size_t iii = 0u; iii < p_access->entries.array.capacity; iii++)
	{
			if(true == si_accesslist_is_valid_at(p_access, iii))
			{
				result++;
			}
	}
END:
	return result;
}

bool si_accesslist_is_empty(si_accesslist_t* const p_access)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	result = (0u == si_accesslist_count(p_access));
END:
	return result;
}

bool si_accesslist_is_full(si_accesslist_t* const p_access)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	result = (p_access->entries.array.capacity == si_accesslist_count(p_access));
END:
	return result;
}

size_t si_accesslist_index_of(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr)
{
	size_t result = SIZE_MAX;
	if(NULL == p_access)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_access->entries_lock)))
	{
		goto END;
	}
	for(size_t iii = 0u; iii < p_access->entries.array.capacity; iii++)
	{
		const struct sockaddr* p_nxt = si_parray_at(&(p_access->entries), iii);
		const int cmp = sockaddr_cmp(
			p_addr,
			p_nxt
		);
		if(0 == cmp)
		{
			result = iii;
			break;
		}
	}
	pthread_mutex_unlock(&(p_access->entries_lock));
END:
	return result;
}

bool si_accesslist_has(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	result = (SIZE_MAX != si_accesslist_index_of(p_access, p_addr));
END:
	return result;
}

bool si_accesslist_append(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr)
{
	bool result = false;
	if((NULL == p_access) || (NULL == p_addr))
	{
		goto END;
	}
	const size_t new_index = si_parray_append(&(p_access->entries), p_addr);
	result = (SIZE_MAX != new_index);
END:
	return result;
}

bool si_accesslist_remove_at(si_accesslist_t* const p_access,
	const size_t index)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	result = si_parray_remove_at(&(p_access->entries), index);
END:
	return result;
}

bool si_accesslist_remove(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr)
{
	bool result = false;
	if(NULL == p_access)
	{
		goto END;
	}
	const size_t index = si_accesslist_index_of(p_access, p_addr);
	if(index >= SIZE_MAX)
	{
		goto END;
	}
	result = si_accesslist_remove_at(p_access, index);
END:
	return result;
}

void si_accesslist_free(si_accesslist_t* const p_access)
{
	if(NULL == p_access)
	{
		goto END;
	}
	si_parray_free(&(p_access->entries));
	pthread_mutex_destroy(&(p_access->entries_lock));
END:
	return;
}

void si_accesslist_destroy(si_accesslist_t** pp_access)
{
	if(NULL == pp_access)
	{
		goto END;
	}
	if(NULL == *pp_access)
	{
		// Already freed
		goto END;
	}
	si_accesslist_free(*pp_access);
	free(*pp_access);
	*pp_access = NULL;
END:
	return;
}