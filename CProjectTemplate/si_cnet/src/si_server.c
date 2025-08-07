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

void si_server_init_5(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue)
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
	struct pollfd initialSocket = (struct pollfd){};
	initialSocket.fd = SOCKET_ERROR;
	initialSocket.events = 0;
	initialSocket.revents = 0;
	si_array_new_3(&(p_server->sockets), sizeof(struct pollfd), max_client_queue);
	for(size_t iii = 0u; iii < max_client_queue; iii++)
	{
		si_array_set(&(p_server->sockets), iii, &initialSocket);
	}
	si_realloc_settings_new(&(p_server->settings));

	int server_fd = socket(
		p_server->family,
		type,
		0
	);
	if(SOCKET_ERROR >= server_fd)
	{
		fprintf(stderr, "server_socket() error: %s\n", strerror(errno));
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
		fprintf(stderr, "server_bind() error: %s\n", strerror(errno));
		close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Mark as passive port with listen
	if(SOCKET_SUCCESS != listen(server_fd, max_client_queue))
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

	// TODO Log to file(?)
	printf("Server bound to local port: %hu.\n", port);
END:
	return;
}

si_server_t* si_server_new_4(const unsigned short port, const int type,
	const int family, const int max_client_queue)
{
	si_server_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_server_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_server_init_5(p_new, port, type, family, max_client_queue);
END:
	return p_new;
}
inline si_server_t* si_server_new_3(const unsigned short port, const int type,
	const int family)
{
	return si_server_new_4(port, type, family, SOMAXCONN);
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
	if(true == si_realloc_settings_grow(&(p_server->settings), &(p_server->sockets)))
	{
		if(current_capacity >= p_server->sockets.capacity)
		{
			// Grow() completed but capacity is not larger.
			goto END;
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
	}
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
	if(NULL == p_server_fd)
	{
		// Invalid array
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const int server_fd = p_server_fd->fd;
	pthread_mutex_unlock(&(p_server->sockets_lock));
	if(SOCKET_SUCCESS > server_fd)
	{
		// Invalid server
		goto END;
	}

	socklen_t addr_size = (socklen_t)sockaddr_sizeof(p_server->family);
	struct sockaddr* client_addr = sockaddr_new(p_server->family);
	int client_fd = accept(
		server_fd,
		client_addr,
		&(addr_size)
	);
	bool is_denied = false;
	if((NULL != p_server->access_list) && (NULL != client_addr))
	{
		const bool has = si_accesslist_has(p_server->access_list, client_addr);
		if(((true  == has) && (true  == p_server->access_list->is_blacklist)) ||
		   ((false == has) && (false == p_server->access_list->is_blacklist)))
		{
			is_denied = true;
			errno = EACCES;
		}
	}

	if((SOCKET_SUCCESS > client_fd) || (NULL == client_addr) || (is_denied))
	{
		if(!((errno == EAGAIN) && (!is_blocking)))
		{
			fprintf(stderr, "server_accept() error: %s\n", strerror(errno));
		}
		if(NULL != client_addr)
		{
			free(client_addr);
		}
		client_addr = NULL;
		if(SOCKET_SUCCESS >= client_fd)
		{
			close(client_fd);
		}
		client_fd = SOCKET_ERROR;
		goto END;
	}
	if(false == si_server_add_socket(p_server, client_fd))
	{
		free(client_addr);
		if(SOCKET_SUCCESS >= client_fd)
		{
			close(client_fd);
		}
		client_fd = SOCKET_ERROR;
		client_addr = NULL;
		goto END;
	}
	// TODO Log connection to file?
	fprintf(stdout, "Client connected: ");
	sockaddr_fprint(stdout, client_addr, addr_size);
	fprintf(stdout, "\n");
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
