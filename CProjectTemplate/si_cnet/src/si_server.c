// si_server.c
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
	(void)close(tmp);
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

/** Doxygen
 * @brief Local function uses provided socket descriptor to set the keepalive
 *        option to the provided desired value.
 * 
 * @param server_fd Socket descriptor used to set options on socket
 * @param keepalive Desired keep alive state as a stdbool value.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_socket_set_keepalive(const int server_fd, const bool keepalive)
{
	bool result = false;
	if(SOCKET_ERROR >= server_fd)
	{
		goto END;
	}
	int i_keepalive = keepalive ? 1 : 0;
	const int set_result = setsockopt(
		server_fd, SOL_SOCKET, SO_KEEPALIVE,
		&i_keepalive, (socklen_t)sizeof(i_keepalive)
	);
	result = (SOCKET_SUCCESS <= set_result);
END:
	return result;
}

/** Doxygen
 * @brief Local function initializes a pthread mutex, and assigns it's
 *        default attributes.
 * 
 * @param p_mutex Pointer to pthread mutex to be initialized.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_server_init_rmutex(pthread_mutex_t* const p_mutex)
{
	bool result = false;
	if(NULL == p_mutex)
	{
		goto END;
	}
	int func_result = 0;
	pthread_mutexattr_t mutex_attributes = {0};
	func_result = pthread_mutexattr_init(&mutex_attributes);
	if(SOCKET_SUCCESS != func_result)
	{
		goto END;
	}
	func_result = pthread_mutexattr_settype(
		&mutex_attributes, PTHREAD_MUTEX_RECURSIVE
	);
	if(SOCKET_SUCCESS != func_result)
	{
		goto END;
	}
	func_result = pthread_mutex_init(
		p_mutex, &mutex_attributes
	);
	if(SOCKET_SUCCESS != func_result)
	{
		goto END;
	}
	func_result = pthread_mutexattr_destroy(&mutex_attributes);
	result = (SOCKET_SUCCESS == func_result);
END:
	return result;
}

/** Doxygen
 * @brief Local function binds an existing server socket 2 all local interfaces
 * 
 * @param server_fd Server socket int file descriptor
 * @param family Socket family enum/id to be initialized as
 * @param port Local port number to bind server socket to
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on sucess. Returns false otherwise.
*/
static bool si_server_socket_bind(const int server_fd,
	const sa_family_t family, const uint16_t port,
	si_logger_t* const p_logger)
{
	bool result = false;
	if((SOCKET_ERROR >= server_fd) || (0u == port))
	{
		goto END;
	}
	if((0u == family) || (AF_MAX <= family))
	{
		goto END;
	}

	// Create a local address used in the bind function call.
	struct sockaddr* p_addr = sockaddr_new(family);
	if(NULL == p_addr)
	{
		si_logger_error(p_logger,
			"Failed to allocate socket address of family type: %hu.", family
		);
		goto END;
	}
	switch(family)
	{
	case(AF_INET):
	{
		((struct sockaddr_in*)p_addr)->sin_addr.s_addr = htonl(INADDR_ANY);
		((struct sockaddr_in*)p_addr)->sin_port = htons(port);
		break;
	}
#ifdef AF_INET6
	case(AF_INET6):
	{
		// Disable IPV6_only to also allow for IPv4 on the socket.
		int disable_only = 0;
		if(SOCKET_ERROR >= setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY,
			&disable_only, sizeof(int)))
		{
			// Failed to enable. NOP
			si_logger_warning(p_logger,
				"Failed to enable IPv4 over a IPv6 server socket with id: %d.", server_fd
			);
		}

		((struct sockaddr_in6*)p_addr)->sin6_addr = in6addr_any;
		((struct sockaddr_in6*)p_addr)->sin6_port = htons(port);
		break;
	}
#endif//AF_INET6
	default:
		// Unsupported/Unknown socket family ID/enum
		si_logger_error(p_logger,
			"Use of an unknown or unsupported family type of: %hu.", family
		);
		goto END;
		break;
	}

	// Bind server socket to local address at target port number
	const socklen_t sock_len = (socklen_t)sockaddr_sizeof(family);
	const int bind_result = bind(server_fd, p_addr, sock_len);
	free(p_addr);
	p_addr = NULL;
	if(SOCKET_SUCCESS != bind_result)
	{
		si_logger_error(p_logger,
			"Server socket failed to bind with error: %s.", strerror(errno)
		);
		goto END;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Local function sets an existing server socket to it's default
 *        options, binds to local interfaces, and listens in passive mode.
 * 
 * @param server_fd Server socket int file descriptor
 * @param family Socket family enum/id to be initialized as
 * @param port Local port number to bind server socket to
 * @param max_client_queue Maximum number(int) of client connections to queue.
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on sucess. Returns false otherwise.
 */
static bool si_server_socket_defaults(const int server_fd,
	const sa_family_t family, const uint16_t port, const int max_client_queue,
	si_logger_t* const p_logger)
{
	bool result = false;
	if((SOCKET_ERROR >= server_fd) || (0u == port))
	{
		goto END;
	}
	if((0u == family) || (AF_MAX <= family))
	{
		goto END;
	}

	int mut_max_queue = max_client_queue;
	// Prevents invalid values being passed to listen()
	if(0 > mut_max_queue)
	{
		mut_max_queue = 0;
	}

	// Set address reuse for faster restart of server socket
	int enable_reuse = 1;
	const int set_reuse_result = setsockopt(
		server_fd, SOL_SOCKET, SO_REUSEADDR,
		&enable_reuse, sizeof(enable_reuse)
	);
	if(SOCKET_ERROR >= set_reuse_result)
	{
		// Failed, log in order to enable for future debugging
		si_logger_warning(p_logger,
			"Failed to enable address reuse for server socket: %d.", server_fd
		);
	}

	// Bind server socket to all local addresses by default
	const bool is_bound = si_server_socket_bind(
		server_fd, family, port, p_logger
	);
	if(true != is_bound)
	{
		goto END;
	}

	// Mark as passive port with listen to enable client socket accept
	const int listen_result = listen(server_fd, mut_max_queue);
	if(SOCKET_SUCCESS != listen_result)
	{
		si_logger_error(p_logger,
			"Server socket failed to set socket %d to passive mode.", server_fd
		);
		goto END;
	}

	// Enable keep alive by default for more consistent socket cleanup.
	const bool set_keepalive = si_socket_set_keepalive(server_fd, true);
	if(true != set_keepalive)
	{
		// Failed, log to enable future debugging
		si_logger_warning(p_logger,
			"Failed to enable keepalive on server socket id: %d.", server_fd
		);
	}
	result = true;
END:
	return result;
}

void si_server_init_7(si_server_t* const p_server, const uint16_t port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings, si_logger_t* const p_logger)
{
	if((NULL == p_server) || (0u == port))
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
	// Prevents invalid values being passed to si_array_init()
	if(0 > mut_max_queue)
	{
		mut_max_queue = 0;
	}

	p_server->family = family;
	p_server->p_access_list = NULL;
	p_server->p_settings = p_settings;
	p_server->p_logger = p_logger;

	// Initialize the pthread mutex for multithreading support
	const bool init_mutex = si_server_init_rmutex(&(p_server->sockets_lock));
	if(true != init_mutex)
	{
		si_logger_error(p_server->p_logger,
			"Failed to initialize server mutex to it's default value."
		);
		goto END;
	}

	// Initializes sockets array with invalid sockets for poll.
	struct pollfd initialSocket = (struct pollfd){0};
	initialSocket.fd = SOCKET_ERROR;
	initialSocket.events = 0;
	initialSocket.revents = 0;
	si_array_init_3(&(p_server->sockets), sizeof(struct pollfd), mut_max_queue);
	for(size_t iii = 0u; iii < mut_max_queue; iii++)
	{
		si_array_set(&(p_server->sockets), iii, &initialSocket);
	}

	// Create and configure server socket
	int server_fd = socket(
		p_server->family,
		type,
		0
	);
	const bool set_socket_options = si_server_socket_defaults(
		server_fd, p_server->family, port, mut_max_queue, p_server->p_logger
	);
	if(true != set_socket_options)
	{
		(void)close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Add server socket to sockets array
	const bool added_socket = si_server_add_socket(p_server, server_fd);
	if(false == added_socket)
	{
		(void)close(server_fd);
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
void si_server_init_6(si_server_t* const p_server, const uint16_t port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger = NULL
	si_server_init_7(
		p_server, port, type, family, max_client_queue, p_settings, NULL
	);
}
inline void si_server_init_5(si_server_t* const p_server,
	const uint16_t port, const int type, const int family,
	const int max_client_queue)
{
	// Default value of p_settings = NULL
	si_server_init_6(p_server, port, type, family, max_client_queue, NULL);
}

si_server_t* si_server_new_6(const uint16_t port, const int type,
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
inline si_server_t* si_server_new_5(const uint16_t port, const int type,
	const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	return si_server_new_6(
		port, type, family, max_client_queue, p_settings, NULL
	);
}
inline si_server_t* si_server_new_4(const uint16_t port, const int type,
	const int family, const int max_client_queue)
{
	// Default value of p_settings is NULL
	return si_server_new_5(port, type, family, max_client_queue, NULL);
}
si_server_t* si_server_new_3(const uint16_t port, const int type,
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
si_server_t* si_server_new_2(const uint16_t port, const int type)
{
	si_server_t* p_new = NULL;
#ifdef AF_INET6
	const bool is_v6_supported = is_ipv6_supported();
	if(true == is_v6_supported)
	{
		p_new = si_server_new_3(port, type, AF_INET6);
		goto END;
	}
#endif//AF_INET6
	p_new = si_server_new_3(port, type, DEFAULT_FAMILY);
END:
	return p_new;
}
inline si_server_t* si_server_new(const uint16_t port)
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
	int lock_result = SOCKET_ERROR;
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	struct pollfd* p_server_struct = (struct pollfd*)(si_array_at(
		&(p_server->sockets), 0u
	));
	if(NULL == p_server_struct)
	{
		while(SOCKET_SUCCESS != unlock_result)
		{
			unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
		}
		goto END;
	}
	const int server_fd = p_server_struct->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		while(SOCKET_SUCCESS != unlock_result)
		{
			unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
		}
		goto END;
	}

	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(server_fd, F_GETFL, 0);
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
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
	int lock_results = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_results)
	{
		lock_results = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		goto UNLOCK;
	}

	// Get all current flags and check if change is needed.
	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(server_fd, F_GETFL, 0);
	if(SOCKET_ERROR >= flags)
	{
		goto UNLOCK;
	}
	if(blocking == (0 == (flags & O_NONBLOCK)))
	{
		goto UNLOCK;
	}

	// Mask/Set new flag while keeping any old flags.
	int new_flags = 0;
	if(true == blocking)
	{
		new_flags = (flags & (~O_NONBLOCK));
	}
	else
	{
		new_flags = (flags | O_NONBLOCK);
	}

	// Validate socket mode changed
	const int fcntl_result = fcntl(server_fd, F_SETFL, new_flags);
	if(SOCKET_ERROR >= fcntl_result)
	{
		goto UNLOCK;
	}
	result = true;
UNLOCK:
	int unlock_results = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_results)
	{
		unlock_results = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
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
	int lock_result = -1;
	int unlock_result = -1;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	if(SOCKET_SUCCESS > server_fd)
	{
		while(SOCKET_SUCCESS != unlock_result)
		{
			unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
		}
		goto END;
	}

	int value = 0;
	const socklen_t c_value_size = (socklen_t)sizeof(value);
	socklen_t value_size = c_value_size;
	int get_result = getsockopt(
		server_fd, SOL_SOCKET, SO_KEEPALIVE,
		&value, &value_size
	);
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
	if((SOCKET_SUCCESS > get_result) || (value_size != c_value_size))
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
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const int server_fd =
		((struct pollfd*)si_array_at(&(p_server->sockets), 0u))->fd;
	result = si_socket_set_keepalive(server_fd, keepalive);
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
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
	// Attempt assign to open slot
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const size_t current_capacity = p_server->sockets.capacity;
	for(size_t iii = 0u; iii < current_capacity; iii++)
	{
		struct pollfd* p_fd = si_array_at(&(p_server->sockets), iii);
		if(NULL == p_fd)
		{
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
		goto UNLOCK;
	}
	// Append instead as there was no open slot to assign
	if(NULL == p_server->p_settings)
	{
		const bool did_grow = si_array_resize(
			&(p_server->sockets), p_server->sockets.capacity + 1u
		);
		if(true != did_grow)
		{
			si_logger_error(p_server->p_logger,
				"Failed to grow socket array with default method"
			);
			goto UNLOCK;
		}
	}
	else
	{
		const bool did_grow = si_realloc_settings_grow(
			p_server->p_settings, &(p_server->sockets)
		);
		if(true == did_grow)
		{
			if(current_capacity >= p_server->sockets.capacity)
			{
				// Grow() completed but capacity is not larger.
				si_logger_error(p_server->p_logger,
					"Failed to grow socket array enough with p_settings method"
				);
				goto UNLOCK;
			}
		}
		else
		{
			// Failed to grow by settings.
			si_logger_error(p_server->p_logger,
				"Failed to grow socket array with p_settings method"
			);
			goto UNLOCK;
		}
	}
	// Initialize new values to invalid sockets.
	const size_t grow_amount = (p_server->sockets.capacity - current_capacity);
	struct pollfd initialValue = (struct pollfd){0};
	initialValue.fd = SOCKET_ERROR;
	initialValue.events = 0;
	initialValue.revents = 0;
	for(size_t iii = 0u; iii < grow_amount; iii++)
	{
		si_array_set(
			&(p_server->sockets), current_capacity + iii, &initialValue
		);
	}
	result = si_server_add_socket(p_server, socket_fd);
UNLOCK:
	int unlock_results = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_results)
	{
		unlock_results = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
END:
	return result;
}

static void si_server_drop_socket_at(si_server_t* const p_server,
	const size_t index)
{
	if(NULL == p_server)
	{
		goto END;
	}
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	if(index >= p_server->sockets.capacity)
	{
		goto END;
	}
	struct pollfd* p_poll = si_array_at(&(p_server->sockets), index);
	if(NULL == p_poll)
	{
		goto END;
	}
	p_poll->events = 0;
	if(SOCKET_SUCCESS <= p_poll->fd)
	{
		(void)close(p_poll->fd);
	}
	p_poll->fd = SOCKET_ERROR;
	p_poll->revents = 0;
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
	}
END:
	return;
}

void si_server_drop_socket(si_server_t* const p_server, const int socket_fd)
{
	if((NULL == p_server) || (SOCKET_ERROR >= socket_fd))
	{
		goto END;
	}
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	for(size_t iii = 0u; iii < p_server->sockets.capacity; iii++)
	{
		struct pollfd* p_poll = (struct pollfd*)(si_array_at(
			&(p_server->sockets), iii
		));
		if(NULL == p_poll)
		{
			// Array is invalid or index is out of bounds
			break;
		}
		int next_fd = p_poll->fd;
		if(SOCKET_ERROR >= next_fd)
		{
			continue;
		}
		if(next_fd == socket_fd)
		{
			si_logger_info(p_server->p_logger,
				"Client at index: %lu with id: %d is disconnecting.",
				iii,
				next_fd
			);
			if(NULL != p_server->p_on_leave)
			{
				p_server->p_on_leave(p_server, next_fd);
			}
			si_server_drop_socket_at(p_server, iii);
		}
	}
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
END:
	return;
}

void si_server_accept(si_server_t* const p_server)
{
	if(NULL == p_server)
	{
		goto END;
	}
	const bool is_blocking = si_server_is_blocking(p_server);
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const struct pollfd* p_server_fd = si_array_at(&(p_server->sockets), 0u);
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
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
	struct sockaddr_storage client_addr = {0};
	int client_fd = accept(
		server_fd,
		(struct sockaddr*)&client_addr,
		&addr_size
	);
	if(SOCKET_SUCCESS > client_fd)
	{
		goto ERROR;
	}

	// Validate access permission(s)
	if(NULL != p_server->p_access_list)
	{
		const bool has = si_accesslist_has(
			p_server->p_access_list, (struct sockaddr*)&client_addr
		);
		if((( true == has) && (true  == p_server->p_access_list->is_blacklist)) ||
		   ((false == has) && (false == p_server->p_access_list->is_blacklist)))
		{
			si_logger_custom(
				p_server->p_logger, SI_LOGGER_INFO,
				NULL, &client_addr, " - Access Denied.",
				(void(*)(FILE* const,  const void* const))sockaddr_fprint
			);
			errno = EACCES;
			goto ERROR;
		}
	}

	// Connected & permitted
	const bool added_socket = si_server_add_socket(p_server, client_fd);
	if(false == added_socket)
	{
		goto ERROR;
	}
	si_logger_custom(
		p_server->p_logger, SI_LOGGER_INFO,
		"Client connected: ", &client_addr, NULL,
		(void(*)(FILE* const,  const void* const))sockaddr_fprint
	);
	if(NULL != p_server->p_on_connect)
	{
		const bool event_result = p_server->p_on_connect(p_server, client_fd);
		if(true != event_result)
		{
			si_server_drop_socket(p_server, client_fd);
		}
	}
	goto END;
ERROR:
	if(!((EAGAIN == errno) && (false == is_blocking)))
	{
		si_logger_error(p_server->p_logger,
			"Server accept() failed with the error: %s", strerror(errno)
		);
	}
	if(SOCKET_SUCCESS <= client_fd)
	{
		// Intentionally ignoring socket close errors.
		(void)close(client_fd);
	}
	client_fd = SOCKET_ERROR;
END:
	return;
}

void si_server_broadcast(si_server_t* const p_server,
	const uint8_t* const p_buffer, const size_t buffer_size,
	const int sender_fd)
{
	if((NULL == p_server) || (NULL == p_buffer) || (0u >= buffer_size))
	{
		goto END;
	}
	int lock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_result)
	{
		lock_result = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	for(size_t iii = 1u; iii < p_server->sockets.capacity; iii++)
	{
		struct pollfd* p_next_poll = si_array_at(&(p_server->sockets), iii);
		if(NULL == p_next_poll)
		{
			continue;
		}
		const int next_fd = p_next_poll->fd;
		if((SOCKET_SUCCESS > next_fd) || (sender_fd == next_fd))
		{
			continue;
		}
		ssize_t send_result = send(next_fd, p_buffer, buffer_size, 0);
		if(SOCKET_ERROR >= send_result)
		{
			(void)close(next_fd);
			p_next_poll->fd = SOCKET_ERROR;
			si_server_drop_socket(p_server, next_fd);
			continue;
		}
	}
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
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
static void si_server_handle_socket(si_server_t* const p_server,
	struct pollfd* const p_fd)
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
		if(NULL == p_server->p_on_read)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLIN);
		}
		else
		{
			const bool event_result = p_server->p_on_read(p_server, p_fd->fd);
			if(true != event_result)
			{
				p_fd->revents |= POLLHUP;
				goto CLOSE;
			}
		}
	}
	if(p_fd->revents & POLLOUT)
	{
		if(NULL == p_server->p_on_write)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLOUT);
		}
		else
		{
			const bool event_result = p_server->p_on_write(p_server, p_fd->fd);
			if(true != event_result)
			{
				p_fd->revents |= POLLHUP;
				goto CLOSE;
			}
		}
	}
CLOSE:
	if((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		si_server_drop_socket(p_server, p_fd->fd);
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
	int lock_results = SOCKET_ERROR;
	while(SOCKET_SUCCESS != lock_results)
	{
		lock_results = pthread_mutex_lock(&(p_server->sockets_lock));
	}
	const bool is_blocking = si_server_is_blocking(p_server);
	const int timeout_mills = is_blocking ? -1 : DEFAULT_POLL_TIMEOUT;
	const size_t capacity = p_server->sockets.capacity;
	const int poll_result = poll(
		p_server->sockets.p_data, capacity, timeout_mills
	);
	// Poll client events
	if(SOCKET_SUCCESS > poll_result)
	{
		goto UNLOCK;
	}
	// Handle poll event results
	for(size_t iii = 1u; iii < capacity; iii++)
	{
		struct pollfd* p_fd = NULL;
		p_fd = si_array_at(&(p_server->sockets), iii);
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
UNLOCK:
	int unlock_result = SOCKET_ERROR;
	while(SOCKET_SUCCESS != unlock_result)
	{
		unlock_result = pthread_mutex_unlock(&(p_server->sockets_lock));
	}
END:
	return;
}

void si_server_free(si_server_t* p_server)
{
	if(NULL == p_server)
	{
		goto END;
	}
	p_server->family = AF_UNSPEC;
	p_server->p_access_list = NULL;
	pthread_mutex_destroy(&(p_server->sockets_lock));
	si_array_free(&(p_server->sockets));
	p_server->p_settings = NULL;
	p_server->p_on_connect = NULL;
	p_server->p_on_read = NULL;
	p_server->p_on_write = NULL;
	p_server->p_on_leave = NULL;
	p_server->p_logger = NULL;
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
