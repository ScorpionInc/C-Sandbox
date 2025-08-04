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
	const size_t addr_size = p_access->is_ipv4 ?
		sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
	si_array_new_3(&(p_access->entries), addr_size, 0u);
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
	const sa_family_t family = p_access->is_ipv4 ? AF_INET : AF_INET6;
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_access->entries_lock)))
	{
		goto END;
	}
	result = sockaddr_is_valid(si_array_at(&(p_access->entries), index), family);
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
	for(size_t i = 0u; i < p_access->entries.capacity; i++)
	{
			if(true == si_accesslist_is_valid_at(p_access, i))
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
	result = (p_access->entries.capacity == si_accesslist_count(p_access));
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
	const sa_family_t family = p_access->is_ipv4 ? AF_INET : AF_INET6;
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_access->entries_lock)))
	{
		goto END;
	}
	for(size_t i = 0u; i < p_access->entries.capacity; i++)
	{
		const int cmp = sockaddr_cmp(
			p_addr,
			(struct sockaddr*)si_array_at(&(p_access->entries), i),
			family
		);
		if(0 == cmp)
		{
			result = i;
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
	const size_t count = si_accesslist_count(p_access);
	if(count >= p_access->entries.capacity)
	{
		if(NEVER == p_access->settings.grow_mode)
		{
			goto END;
		}
		if(false == si_realloc_settings_grow(&(p_access->settings), &(p_access->entries)))
		{
			goto END;
		}
	}
	for(size_t i = 0u; i < p_access->entries.capacity; i++)
	{
		if(false == si_accesslist_is_valid_at(p_access, i))
		{
			// Append here
			si_array_set(&(p_access->entries), i, p_addr);
			result = true;
			break;
		}
	}
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
	if(index >= p_access->entries.capacity)
	{
		goto END;
	}
	const size_t esz = p_access->entries.element_size;
	memset(&(((uint8_t*)p_access->entries.p_data)[index * esz]), 0x00, esz);
	result = true;
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
	si_array_free(&(p_access->entries));
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