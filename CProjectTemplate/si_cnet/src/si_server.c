//si_server.c

#include "si_server.h"

bool is_ipv6_supported(void)
{
	bool result = false;
#if defined(AF_INET6) && defined(__linux__)
	int tmp = socket(AF_INET6, SOCK_STREAM, 0);
	if(SOCKET_ERROR >= tmp)
	{
		result = (errno != EAFNOSUPPORT);
		goto END;
	}
	result = true;
	close(tmp);
	tmp = SOCKET_ERROR;
#endif//AF_INET6 && __linux__
END:
	return result;
}

size_t get_client_queue_limit(void)
{
	unsigned long result = SIZE_MAX;
#ifdef __linux__
	struct rlimit nofile_limit = {0};
	const int get_limit_result = getrlimit(RLIMIT_NOFILE, &nofile_limit);
	if(SOCKET_SUCCESS != get_limit_result)
	{
		goto END;
	}
	const size_t soft_limit = (size_t)nofile_limit.rlim_cur;
	const size_t kernel_limit = (size_t)SOMAXCONN;
	result = soft_limit < kernel_limit ? soft_limit : kernel_limit;
#endif//__linux__
END:
	return result;
}

void si_server_init_7(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* p_settings, si_logger_t* p_logger)
{
	if(NULL == p_server)
	{
		goto END;
	}
	if((0 >= type) || (SOCK_PACKET <= type))
	{
		// Invalid type
		goto END;
	}
	if((0 > family) || (AF_MAX <= family))
	{
		// Invalid family
		goto END;
	}
	int mut_max_queue = max_client_queue;
	if(0 > mut_max_queue)
	{
		mut_max_queue = 0;
	}
	p_server->family = family;

	// Initialize the pthread mutex
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
		&(p_server->sockets_lock), &mutex_attributes))
	{
        goto END;
    }
	if(SOCKET_SUCCESS != pthread_mutexattr_destroy(&mutex_attributes))
	{
		goto END;
	}

	// Initialize sockets array with invalid sockets.
	struct pollfd initialSocket = (struct pollfd){0};
	initialSocket.fd = SOCKET_ERROR;
	initialSocket.events = 0;
	initialSocket.revents = 0;
	si_array_init_3(&(p_server->sockets), sizeof(struct pollfd), mut_max_queue);
	for(size_t iii = 0u; iii < mut_max_queue; iii++)
	{
		si_array_set(&(p_server->sockets), iii, &initialSocket);
	}
	p_server->p_settings = p_settings;
	p_server->p_logger = p_logger;

	int server_fd = socket(
		p_server->family,
		type,
		0
	);
	if(SOCKET_ERROR >= server_fd)
	{
		si_logger_error(p_server->p_logger,
			"server_socket() error: %s", strerror(errno)
		);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Set address reuse for server socket
	int enable_reuse = 1;
	if(SOCKET_ERROR >= setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
		&enable_reuse, sizeof(enable_reuse)))
	{
		// Failed to enable. NOP
	}

	// Configure and Bind address to socket
	socklen_t sock_len = (socklen_t)sockaddr_sizeof(p_server->family);
	struct sockaddr* p_addr = sockaddr_new(p_server->family);
	if(NULL == p_addr)
	{
		close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}
	switch(p_server->family)
	{
	case(AF_INET):
		((struct sockaddr_in*)p_addr)->sin_addr.s_addr = htonl(INADDR_ANY);
		((struct sockaddr_in*)p_addr)->sin_port = htons(port);
		break;
#ifdef AF_INET6
	case(AF_INET6):

		// Disable IPV6_only to also allow for IPv4 on the socket.
		int disable_only = 0;
		if(SOCKET_ERROR >= setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY,
			&disable_only, sizeof(int)))
		{
			// Failed to enable. NOP
		}

		((struct sockaddr_in6*)p_addr)->sin6_addr = in6addr_any;
		((struct sockaddr_in6*)p_addr)->sin6_port = htons(port);
		break;
#endif//AF_INET6
	default:
		// Unsupported family
		break;
	}

	// Bind new socket to port
	const int bind_result = bind(
		server_fd,
		p_addr,
		sock_len
	);
	if(SOCKET_SUCCESS != bind_result)
	{
		si_logger_error(p_server->p_logger,
			"server_bind() error: %s", strerror(errno)
		);
		close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Mark as passive port with listen
	if(SOCKET_SUCCESS != listen(server_fd, mut_max_queue))
	{
		close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Add server socket to sockets array
	if(false == si_server_add_socket(p_server, server_fd))
	{
		close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}
	si_logger_info(
		p_server->p_logger,
		"Server bound to local port: %hu.", port
	);
END:
	return;
}
void si_server_init_6(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger = NULL
	si_server_init_7(
		p_server, port, type, family, max_client_queue, p_settings, NULL
	);
}
inline void si_server_init_5(si_server_t* const p_server,
	const unsigned short port, const int type, const int family,
	const int max_client_queue)
{
	// Default value of p_settings = NULL
	si_server_init_6(p_server, port, type, family, max_client_queue, NULL);
}

si_server_t* si_server_new_6(const unsigned short port, const int type,
	const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings, si_logger_t* const p_logger)
{
	si_server_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_server_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_server_init_7(p_new,
		port, type, family, max_client_queue,
		p_settings, p_logger
	);
END:
	return p_new;
}
inline si_server_t* si_server_new_5(const unsigned short port, const int type,
	const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	return si_server_new_6(
		port, type, family, max_client_queue, p_settings, NULL
	);
}
inline si_server_t* si_server_new_4(const unsigned short port, const int type,
	const int family, const int max_client_queue)
{
	// Default value of p_settings is NULL
	return si_server_new_5(port, type, family, max_client_queue, NULL);
}
si_server_t* si_server_new_3(const unsigned short port, const int type,
	const int family)
{
	si_server_t* p_server = NULL;
	size_t max_clients = get_client_queue_limit();
	if(max_clients > INT_MAX)
	{
		p_server = si_server_new_4(port, type, family, 0);
		goto END;
	}
	p_server = si_server_new_4(port, type, family, (int)max_clients);
END:
	return p_server;
}
si_server_t* si_server_new_2(const unsigned short port, const int type)
{
	si_server_t* p_new = NULL;
#ifdef AF_INET6
	if(is_ipv6_supported())
	{
		p_new = si_server_new_3(port, type, AF_INET6);
		goto END;
	}
#endif//AF_INET6
	p_new = si_server_new_3(port, type, DEFAULT_FAMILY);
END:
	return p_new;
}
inline si_server_t* si_server_new(const unsigned short port)
{
	return si_server_new_2(port, DEFAULT_TYPE);
}

bool si_server_is_blocking(si_server_t* const p_server)
{
	bool result = false;
	if(NULL == p_server)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}

	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(server_fd, F_GETFL, 0);
	pthread_mutex_unlock(&(p_server->sockets_lock));
	if(SOCKET_ERROR >= flags)
	{
		goto END;
	}
	result = (0 == (flags & O_NONBLOCK));
END:
	return result;
}

bool si_server_set_blocking(si_server_t* const p_server,
	const bool blocking)
{
	bool result = false;
	if(NULL == p_server)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}

	// Get all current flags and check if change is needed.
	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(server_fd, F_GETFL, 0);
	if(SOCKET_ERROR >= flags)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	if(blocking == (0 == (flags & O_NONBLOCK)))
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}

	// Mask/Set new flag while keeping any old flags.
	int new_flags = 0;
	if(blocking)
	{
		new_flags = (flags & (~O_NONBLOCK));
	}
	else
	{
		new_flags = (flags | O_NONBLOCK);
	}

	// Validate socket mode changed
	if(SOCKET_ERROR >= fcntl(server_fd, F_SETFL, new_flags))
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	result = true;
	pthread_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_is_keepalive(si_server_t* const p_server)
{
	bool result = false;
	if(NULL == p_server)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}

	int value = 0;
	socklen_t value_size = (socklen_t)sizeof(value);
	int get_result = getsockopt(
		server_fd, SOL_SOCKET, SO_KEEPALIVE,
		&value, &value_size
	);
	pthread_mutex_unlock(&(p_server->sockets_lock));
	if((SOCKET_SUCCESS > get_result) || (value_size != (socklen_t)sizeof(value)))
	{
		goto END;
	}
	result = (0 < value);
END:
	return result;
}

bool si_server_set_keepalive(si_server_t* const p_server, const bool keepalive)
{
	bool result = false;
	if(NULL == p_server)
	{
		goto END;
	}
	int i_keepalive = keepalive ? 1 : 0;
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const int set_result = setsockopt(
		server_fd, SOL_SOCKET, SO_KEEPALIVE,
		&i_keepalive, (socklen_t)sizeof(i_keepalive)
	);

	pthread_mutex_unlock(&(p_server->sockets_lock));
	result = (SOCKET_SUCCESS <= set_result);
END:
	return result;
}

bool si_server_add_socket(si_server_t* const p_server, const int socket_fd)
{
	bool result = false;
	if(SOCKET_SUCCESS > socket_fd)
	{
		goto END;
	}
	// Attempt assign
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const size_t current_capacity = p_server->sockets.capacity;
	for(size_t iii = 0u; iii < current_capacity; iii++)
	{
		struct pollfd* p_fd = si_array_at(&(p_server->sockets), iii);
		if(NULL == p_fd)
		{
			// Error: should never be NULL.
			continue;
		}
		if(SOCKET_SUCCESS <= p_fd->fd)
		{
			// Already a valid file descriptor
			continue;
		}
		p_fd->fd = socket_fd;
		p_fd->events = (POLLIN | POLLOUT | POLLHUP);
		p_fd->revents = 0;
		result = true;
		goto END;
	}
	// Append
	if(NULL == p_server->p_settings)
	{
		const bool did_grow = si_array_resize(&(p_server->sockets), p_server->sockets.capacity + 1u);
		if(true != did_grow)
		{
			pthread_mutex_unlock(&(p_server->sockets_lock));
			goto END;
		}
	}
	else
	{
		if(true == si_realloc_settings_grow(p_server->p_settings, &(p_server->sockets)))
		{
			if(current_capacity >= p_server->sockets.capacity)
			{
				// Grow() completed but capacity is not larger.
				pthread_mutex_unlock(&(p_server->sockets_lock));
				goto END;
			}
		}
		else
		{
			// Failed to grow by settings.
			pthread_mutex_unlock(&(p_server->sockets_lock));
			goto END;
		}
	}
	// Initialize new values to invalid sockets.
	const size_t grow_amount = (p_server->sockets.capacity - current_capacity);
	struct pollfd initialValue = (struct pollfd){};
	initialValue.fd = SOCKET_ERROR;
	initialValue.events = 0;
	initialValue.revents = 0;
	for(size_t iii = 0u; iii < grow_amount; iii++)
	{
		si_array_set(&(p_server->sockets), current_capacity + iii, &initialValue);
	}
	result = si_server_add_socket(p_server, socket_fd);
	pthread_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

void si_server_accept(si_server_t* const p_server)
{
	if(NULL == p_server)
	{
		goto END;
	}
	const bool is_blocking = si_server_is_blocking(p_server);
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const struct pollfd* p_server_fd = si_array_at(&(p_server->sockets), 0u);
	pthread_mutex_unlock(&(p_server->sockets_lock));
	if(NULL == p_server_fd)
	{
		// Invalid array
		goto END;
	}
	const int server_fd = p_server_fd->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		// Invalid server
		goto END;
	}

	// Connect a client and validate socket
	socklen_t addr_size = (socklen_t)sockaddr_sizeof(p_server->family);
	struct sockaddr* client_addr = sockaddr_new(p_server->family);
	int client_fd = accept(
		server_fd,
		client_addr,
		&(addr_size)
	);
	if((SOCKET_SUCCESS > client_fd) || (NULL == client_addr))
	{
		goto ERROR;
	}

	// Validate access permission(s)
	if((NULL != p_server->access_list) && (NULL != client_addr))
	{
		const bool has = si_accesslist_has(p_server->access_list, client_addr);
		if((( true == has) && (true  == p_server->access_list->is_blacklist)) ||
		   ((false == has) && (false == p_server->access_list->is_blacklist)))
		{
			si_logger_custom(
				p_server->p_logger, SI_LOGGER_INFO,
				NULL, client_addr, " - Access Denied.",
				(void(*)(FILE* const,  const void* const))sockaddr_fprint
			);
			errno = EACCES;
			goto ERROR;
		}
	}

	// Connected & permitted
	if(false == si_server_add_socket(p_server, client_fd))
	{
		goto ERROR;
	}
	si_logger_custom(
		p_server->p_logger, SI_LOGGER_INFO,
		"Client connected: ", client_addr, NULL,
		(void(*)(FILE* const,  const void* const))sockaddr_fprint
	);
	goto END;
ERROR:
	if(!((errno == EAGAIN) && (!is_blocking)))
	{
		si_logger_error(p_server->p_logger,
			"server_accept() error: %s", strerror(errno)
		);
	}
	if(NULL != client_addr)
	{
		free(client_addr);
	}
	client_addr = NULL;
	if(SOCKET_SUCCESS <= client_fd)
	{
		close(client_fd);
	}
	client_fd = SOCKET_ERROR;
END:
	return;
}

/** Doxygen
 * @brief Calls the server's handle_read/heandle_write functions based upon the
 *        poll event results for a single socket. Handles disconnects / errors.
 * 
 * @param p_server Pointer to the si_server struct to call functions from.
 * @param p_fd Pointer to pollfd struct with socket descriptor & event flags
 */
static void si_server_handle_socket(si_server_t* const p_server, struct pollfd* const p_fd)
{
	if(NULL == p_fd)
	{
		goto END;
	}
	if((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		goto CLOSE;
	}
	if(p_fd->revents & POLLIN)
	{
		if(NULL == p_server->p_handle_read)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLIN);
		}
		else
		{
			p_server->p_handle_read(p_fd);
		}
	}
	if((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		goto CLOSE;
	}
	if(p_fd->revents & POLLOUT)
	{
		if(NULL == p_server->p_handle_write)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLOUT);
		}
		else
		{
			p_server->p_handle_write(p_fd);
		}
	}
CLOSE:
	if((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		p_fd->events = 0;
		if(SOCKET_SUCCESS <= p_fd->fd)
		{
			close(p_fd->fd);
		}
		p_fd->fd = SOCKET_ERROR;
	}
END:
	return;
}

void si_server_handle_events(si_server_t* const p_server)
{
	if(NULL == p_server)
	{
		goto END;
	}
	if(SOCKET_SUCCESS != pthread_mutex_lock(&(p_server->sockets_lock)))
	{
		goto END;
	}
	const size_t capacity = p_server->sockets.capacity;
	const int poll_result = poll(p_server->sockets.p_data, capacity, -1);
	// Poll client events
	if(SOCKET_SUCCESS > poll_result)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	// Handle poll event results
	for(size_t i = 1u; i < capacity; i++)
	{
		struct pollfd* p_fd = NULL;
		p_fd = si_array_at(&(p_server->sockets), i);
		if(NULL == p_fd)
		{
			continue;
		}
		if((SOCKET_SUCCESS > p_fd->fd) || (0 == p_fd->revents))
		{
			continue;
		}
		si_server_handle_socket(p_server, p_fd);
	}
	pthread_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

void si_server_free(si_server_t* p_server)
{
	if(NULL == p_server)
	{
		goto END;
	}
	pthread_mutex_destroy(&(p_server->sockets_lock));
	si_array_free(&(p_server->sockets));
END:
	return;
}

void si_server_free_at(si_server_t** pp_server)
{
	if(NULL == pp_server)
	{
		goto END;
	}
	if(NULL == *pp_server)
	{
		// Already Freed
		goto END;
	}
	si_server_free(*pp_server);
	free(*pp_server);
	*pp_server = NULL;
END:
	return;
}
