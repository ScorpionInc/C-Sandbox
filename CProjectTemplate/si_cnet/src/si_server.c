// si_server.c
#include "si_server.h"

size_t get_client_queue_limit(void)
{
	unsigned long result = SIZE_MAX;
#ifdef __linux__
	struct rlimit nofile_limit = {0};
	const int get_limit_result = getrlimit(RLIMIT_NOFILE, &nofile_limit);
	if (SOCKET_SUCCESS != get_limit_result)
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
 * @brief Function binds a server socket to all local interfaces on same port
 * 
 * @param server_fd Server socket int file descriptor
 * @param family Socket family enum/id to be initialized as
 * @param port Local port number to bind server socket to
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_server_socket_bind_port(const int server_fd,
	const sa_family_t family, const uint16_t port,
	si_logger_t* const p_logger)
{
	bool result = false;
	if ((SOCKET_ERROR >= server_fd) || (0u == port))
	{
		goto END;
	}
	if ((0u == family) || (AF_MAX <= family))
	{
		goto END;
	}

	// Create a local address used in the bind function call.
	struct sockaddr* p_addr = sockaddr_new(family);
	if (NULL == p_addr)
	{
		si_logger_error(p_logger,
			"Failed to allocate socket address of family type: %hu.", family
		);
		goto END;
	}
	switch (family)
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
		if (SOCKET_ERROR >= setsockopt(server_fd, IPPROTO_IPV6, IPV6_V6ONLY,
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
		free(p_addr);
		p_addr = NULL;
		goto END;
	break;
	}

	// Bind server socket to local address at target port number
	const socklen_t sock_len = (socklen_t)sockaddr_sizeof(family);
	const int bind_result = bind(server_fd, p_addr, sock_len);
	free(p_addr);
	p_addr = NULL;
	if (SOCKET_SUCCESS != bind_result)
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

#ifdef AF_UNIX
/** Doxygen
 * @brief Local function binds an existing server socket to a unix socket file.
 * 
 * @param server_fd Server socket int file descriptor
 * @param p_path Path of the socket to bind to.
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_server_socket_bind_unix(const int server_fd,
	const char* const p_path, si_logger_t* const p_logger)
{
	bool result = false;
	if ((SOCKET_ERROR >= server_fd) || (NULL == p_path))
	{
		goto END;
	}

	// Create a local address used in the bind function call.
	struct sockaddr_un* p_addr = (struct sockaddr_un*)sockaddr_new(AF_UNIX);
	if (NULL == p_addr)
	{
		si_logger_error(p_logger,
			"Failed to allocate socket address of family type: AF_UNIX(%hu).",
			AF_UNIX
		);
		goto END;
	}
	p_addr->sun_family = AF_UNIX;

	// Sets the sun_path value from the p_path C string value.
	const size_t data_size = sizeof(p_addr->sun_path);
	memset(p_addr->sun_path, 0x00, data_size);
	const size_t path_size = strnlen(p_path, data_size + 1u);
	if (path_size >= data_size)
	{
		si_logger_error(p_logger,
			"Requested AF_UNIX(%hu) path '%s' overflows the address buffer.",
			AF_UNIX, p_path
		);
		free(p_addr);
		p_addr = NULL;
		goto END;
	}
	strncpy(p_addr->sun_path, p_path, data_size - 1u);
	// Should already be NULL but just in case.
	(p_addr->sun_path)[data_size - 1u] = '\0';

	// Actually does the socket binding.
	const int bind_result = bind(
		server_fd, (struct sockaddr*)p_addr, sizeof(struct sockaddr_un)
	);
	free(p_addr);
	p_addr = NULL;
	if (SI_PTHREAD_SUCCESS != bind_result)
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
#endif//AF_UNIX

/** Doxygen
 * @brief Local function binds an existing server socket based upon family
 * 
 * @param server_fd Server socket int file descriptor
 * @param family Socket family enum/id to be initialized as
 * @param p_bind_target Pointer to value to be bound/bind to.
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_server_socket_bind(const int server_fd,
	const sa_family_t family, const void* const p_bind_target,
	si_logger_t* const p_logger)
{
	bool result = false;
	if ((SOCKET_ERROR >= server_fd) || (NULL == p_bind_target))
	{
		goto END;
	}
	// Switch case fall through is intended behavior.
	switch (family)
	{
	case AF_INET:
#ifdef AF_INET6
	case AF_INET6:
#endif//AF_INET6
		uint16_t* p_port_address = (uint16_t*)p_bind_target;
		result = si_server_socket_bind_port(
			server_fd, family, *p_port_address, p_logger
		);
	break;
#ifdef AF_UNIX
	case AF_UNIX:
		const char* const p_path = (const char*)p_bind_target;
		result = si_server_socket_bind_unix(server_fd, p_path, p_logger);
	break;
#endif//AF_UNIX
	default:
		si_logger_error(p_logger,
			"Use of an unknown or unsupported family type of: %hu.", family
		);
	break;
	}
END:
	return result;
}

/** Doxygen
 * @brief Local function sets an existing server socket to it's default
 *        options, binds to local interfaces, and listens in passive mode.
 * 
 * @param server_fd Server socket int file descriptor
 * @param family Socket family enum/id to be initialized as
 * @param p_bind_target Pointer to the value to bind the socket to.
 * @param max_client_queue Maximum number(int) of client connections to queue.
 * @param p_logger Optional pointer to si_logger_t for error messages
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool si_server_socket_defaults(const int server_fd,
	const sa_family_t family, const void* const p_bind_target,
	const int max_client_queue, si_logger_t* const p_logger)
{
	bool result = false;
	if ((SOCKET_ERROR >= server_fd) || (NULL == p_bind_target))
	{
		goto END;
	}
	if ((0u == family) || (AF_MAX <= family))
	{
		goto END;
	}

	int mut_max_queue = max_client_queue;
	// Prevents invalid values being passed to listen()
	if (0 > mut_max_queue)
	{
		mut_max_queue = 0;
	}

	// Set address reuse for faster restart of server socket
	int enable_reuse = 1;
	const int set_reuse_result = setsockopt(
		server_fd, SOL_SOCKET, SO_REUSEADDR,
		&enable_reuse, sizeof(enable_reuse)
	);
	if (SOCKET_ERROR >= set_reuse_result)
	{
		// Failed, log in order to enable for future debugging
		si_logger_warning(p_logger,
			"Failed to enable address reuse for server socket: %d.", server_fd
		);
	}

	// Bind the server socket based upon socket family.
	const bool is_bound = si_server_socket_bind(
		server_fd, family, p_bind_target, p_logger
	);
	if (true != is_bound)
	{
		goto END;
	}

	// Mark as passive port with listen to enable client socket accept
	const int listen_result = listen(server_fd, mut_max_queue);
	if (SOCKET_SUCCESS != listen_result)
	{
		si_logger_error(p_logger,
			"Server socket failed to set socket %d to passive mode.", server_fd
		);
		goto END;
	}

	// Enable keep alive by default for more consistent socket cleanup.
	const bool set_keepalive = si_socket_set_keepalive(&server_fd, true);
	if (true != set_keepalive)
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


// Generic family socket server initializers
void si_server_init_7(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue, const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	if ((NULL == p_server) || (NULL == p_bind_target))
	{
		goto END;
	}
	if ((0 >= type) || (SOCK_PACKET <= type))
	{
		// Invalid type
		goto END;
	}
	if ((0 > family) || (AF_MAX <= family))
	{
		// Invalid family
		goto END;
	}
	int mut_max_queue = max_client_queue;
	// Prevents invalid values being passed to initializer
	if (0 > mut_max_queue)
	{
		mut_max_queue = 0;
	}

	p_server->family = family;
	if(AF_INET == family)
	{
		// Prefer IPv6 over IPv4
#ifdef AF_INET6
		const bool is_v6_supported = is_ipv6_supported();
		if (true == is_v6_supported)
		{
			p_server->family = AF_INET6;
		}
#endif//AF_INET6
	}

	p_server->p_access_list = NULL;
	p_server->p_settings = (si_realloc_settings_t*)p_settings;
	p_server->p_logger = p_logger;

	// Initialize the mutex for multithreading support
	const bool init_mutex = (
		SI_PTHREAD_SUCCESS == si_mutex_init_2(
			&(p_server->sockets_lock), PTHREAD_MUTEX_RECURSIVE
		)
	);
	if (true != init_mutex)
	{
		si_logger_error(p_server->p_logger,
			"Failed to initialize server mutex to it's default value."
		);
		goto END;
	}
	si_poll_init(&(p_server->sockets), mut_max_queue);

	// Create and configure server socket
	int server_fd = socket(
		p_server->family,
		type,
		0
	);
	const bool set_socket_options = si_server_socket_defaults(
		server_fd, p_server->family, p_bind_target, mut_max_queue, p_server->p_logger
	);
	if (true != set_socket_options)
	{
		(void)close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}

	// Add server socket to sockets array
	const bool added_socket = si_server_add_socket(p_server, server_fd);
	if (false == added_socket)
	{
		(void)close(server_fd);
		server_fd = SOCKET_ERROR;
		goto END;
	}
END:
	return;
}
inline void si_server_init_6(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue, const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	si_server_init_7(
		p_server, p_bind_target, type, family,
		max_client_queue, p_settings, NULL
	);
}
inline void si_server_init_5(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue)
{
	// Default value of p_settings is NULL
	si_server_init_6(
		p_server, p_bind_target, type, family, max_client_queue, NULL
	);
}
void si_server_init_4(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family)
{
	// Default value of max_client_queue = get_client_queue_limit()
	int max_client_queue = 0;
	const size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_client_queue = (int)queue_limit;
	}
	si_server_init_5(p_server, p_bind_target, type, family, max_client_queue);
}
inline void si_server_init_3(si_server_t* const p_server,
	const void* const p_bind_target, const int type)
{
	// Default value of family is DEFAULT_FAMILY(AF_INET)
	si_server_init_4(p_server, p_bind_target, type, DEFAULT_FAMILY);
}
inline void si_server_init(si_server_t* const p_server,
	const void* const p_bind_target)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	si_server_init_3(p_server, p_bind_target, DEFAULT_TYPE);
}

// Generic family socket server allocators
si_server_t* si_server_new_6(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	si_server_t* p_server = NULL;
	if (NULL == p_bind_target)
	{
		goto END;
	}
	if ((0 >= type) || (SOCK_PACKET <= type))
	{
		// Invalid type
		goto END;
	}
	if ((0 > family) || (AF_MAX <= family))
	{
		// Invalid family
		goto END;
	}
	p_server = calloc(1u, sizeof(si_server_t));
	if (NULL == p_server)
	{
		// Failed to allocate
		goto END;
	}
	si_server_init_7(
		p_server, p_bind_target, type, family,
		max_client_queue, p_settings, p_logger
	);
END:
	return p_server;
}
inline si_server_t* si_server_new_5(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue,
	const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	return si_server_new_6(
		p_bind_target, type, family, max_client_queue, p_settings, NULL
	);
}
inline si_server_t* si_server_new_4(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue)
{
	// Default value of p_settings is NULL
	return si_server_new_5(
		p_bind_target, type, family, max_client_queue, NULL
	);
}
si_server_t* si_server_new_3(const void* const p_bind_target, const int type,
	const int family)
{
	// Default value of max_client_queue is get_client_queue_limit()
	int max_clients = 0;
	size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_clients = (int)queue_limit;
	}
	return si_server_new_4(p_bind_target, type, family, max_clients);
}
inline si_server_t* si_server_new_2(const void* const p_bind_target, const int type)
{
	// Default value of family is DEFAULT_FAMILY(AF_INET)
	return si_server_new_3(p_bind_target, type, DEFAULT_FAMILY);
}
inline si_server_t* si_server_new(const void* const p_bind_target)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	return si_server_new_2(p_bind_target, DEFAULT_TYPE);
}

// IPv4/IPv6 socket server initializers
inline void si_server_init_inet_6(si_server_t* const p_server, const uint16_t port,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	si_server_init_7(
		p_server, &port, type, AF_INET, max_client_queue, p_settings, p_logger
	);
}
inline void si_server_init_inet_5(si_server_t* const p_server, const uint16_t port,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger = NULL
	si_server_init_inet_6(
		p_server, port, type, max_client_queue, p_settings, NULL
	);
}
inline void si_server_init_inet_4(si_server_t* const p_server,
	const uint16_t port, const int type, const int max_client_queue)
{
	// Default value of p_settings = NULL
	si_server_init_inet_5(p_server, port, type, max_client_queue, NULL);
}
void si_server_init_inet_3(si_server_t* const p_server,
	const uint16_t port, const int type)
{
	// Default value of max_client_queue = get_client_queue_limit()
	int max_client_queue = 0;
	const size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_client_queue = (int)queue_limit;
	}
	si_server_init_inet_4(p_server, port, type, max_client_queue);
}
inline void si_server_init_inet(si_server_t* const p_server,
	const uint16_t port)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	si_server_init_inet_3(p_server, port, DEFAULT_TYPE);
}

// IPv4/IPv6 socket server allocators
inline si_server_t* si_server_new_inet_5(const uint16_t port, const int type,
	const int max_client_queue, const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	return si_server_new_6(
		&port, type, AF_INET, max_client_queue, p_settings, p_logger
	);
}
inline si_server_t* si_server_new_inet_4(const uint16_t port, const int type,
	const int max_client_queue, const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	return si_server_new_inet_5(
		port, type, max_client_queue, p_settings, NULL
	);
}
inline si_server_t* si_server_new_inet_3(const uint16_t port, const int type,
	const int max_client_queue)
{
	// Default value of p_settings is NULL
	return si_server_new_inet_4(port, type, max_client_queue, NULL);
}
si_server_t* si_server_new_inet_2(const uint16_t port, const int type)
{
	// Default value of max_client_queue is get_client_queue_limit()
	int max_clients = 0;
	size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_clients = (int)queue_limit;
	}
	return si_server_new_inet_3(port, type, max_clients);
}
inline si_server_t* si_server_new_inet(const uint16_t port)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	return si_server_new_inet_2(port, DEFAULT_TYPE);
}

// Unix socket server initializers
inline void si_server_init_unix_6(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	si_server_init_7(
		p_server, p_path, type, AF_UNIX, max_client_queue, p_settings, p_logger
	);
}
inline void si_server_init_unix_5(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	si_server_init_unix_6(
		p_server, p_path, type, max_client_queue, p_settings, NULL
	);
}
inline void si_server_init_unix_4(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue)
{
	// Default value of p_settings is NULL
	si_server_init_unix_5(p_server, p_path, type, max_client_queue, NULL);
}
void si_server_init_unix_3(si_server_t* const p_server,
	const char* const p_path, const int type)
{
	// Default value of max_client_queue = get_client_queue_limit()
	int max_client_queue = 0;
	const size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_client_queue = (int)queue_limit;
	}
	si_server_init_unix_4(p_server, p_path, type, max_client_queue);
}
inline void si_server_init_unix(si_server_t* const p_server,
	const char* const p_path)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	si_server_init_unix_3(p_server, p_path, DEFAULT_TYPE);
}

// Unix socket server allocators
inline si_server_t* si_server_new_unix_5(const char* const p_path,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger)
{
	return si_server_new_6(
		p_path, type, AF_UNIX, max_client_queue, p_settings, p_logger
	);
}
inline si_server_t* si_server_new_unix_4(const char* const p_path,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings)
{
	// Default value of p_logger is NULL
	return si_server_new_unix_5(p_path, type, max_client_queue, p_settings, NULL);
}
inline si_server_t* si_server_new_unix_3(const char* const p_path,
	const int type, const int max_client_queue)
{
	// Default value p_settings is NULL
	return si_server_new_unix_4(p_path, type, max_client_queue, NULL);
}
si_server_t* si_server_new_unix_2(const char* const p_path,
	const int type)
{
	// Default value of p_settings is get_client_queue_limit()
	int max_clients = 0;
	size_t queue_limit = get_client_queue_limit();
	if (INT_MAX >= queue_limit)
	{
		max_clients = (int)queue_limit;
	}
	return si_server_new_unix_3(p_path, type, max_clients);
}
inline si_server_t* si_server_new_unix(const char* const p_path)
{
	// Default value of type is DEFAULT_TYPE(SOCK_STREAM)
	return si_server_new_unix_2(p_path, DEFAULT_TYPE);
}

size_t si_server_count_clients(si_server_t* const p_server)
{
	size_t result = 0u;
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	result = si_poll_count_3(&(p_server->sockets), 0, 1u);
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_is_blocking(si_server_t* const p_server)
{
	bool result = false;
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const si_poll_info* const p_server_info = si_poll_at(
		&(p_server->sockets), 0u
	);
	if (NULL == p_server_info)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const si_socket_t server_fd = p_server_info->fd;
	result = si_socket_is_blocking(&server_fd);
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_set_blocking(si_server_t* const p_server,
	const bool blocking)
{
	bool result = false;
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const si_poll_info* const p_server_info = si_poll_at(
		&(p_server->sockets), 0u
	);
	if (NULL == p_server_info)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const si_socket_t server_fd = p_server_info->fd;
	result = si_socket_set_blocking(&server_fd, blocking);
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_is_keepalive(si_server_t* const p_server)
{
	bool result = false;
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const si_poll_info* const p_server_info = si_poll_at(
		&(p_server->sockets), 0u
	);
	if (NULL == p_server_info)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const si_socket_t server_fd = p_server_info->fd;
	result = si_socket_is_keepalive(&server_fd);
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_set_keepalive(si_server_t* const p_server, const bool keepalive)
{
	bool result = false;
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const si_poll_info* const p_server_info = si_poll_at(
		&(p_server->sockets), 0u
	);
	if (NULL == p_server_info)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	const si_socket_t server_fd = p_server_info->fd;
	result = si_socket_set_keepalive(&server_fd, keepalive);
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

bool si_server_add_socket(si_server_t* const p_server, const si_socket_t socket_fd)
{
	bool result = false;
	if (SOCKET_SUCCESS > socket_fd)
	{
		goto END;
	}
	// Attempt assign to open slot
	si_mutex_lock(&(p_server->sockets_lock));
	result = si_poll_append_4(
		&(p_server->sockets),
		&socket_fd, (POLLIN | POLLOUT | POLLHUP),
		p_server->p_settings
	);
	if (true != result)
	{
		si_logger_error(p_server->p_logger, "Failed to add socket: %d.", socket_fd);
	}
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return result;
}

void si_server_drop_socket(si_server_t* const p_server, const si_socket_t socket_fd)
{
	if (NULL == p_server)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(&socket_fd);
	if (true != is_valid)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const bool closed = si_poll_close(&(p_server->sockets), socket_fd);
	if (true == closed)
	{
		si_logger_info(p_server->p_logger,
			"Client socket with id: %d was disconnected.", socket_fd
		);
		if (NULL != p_server->p_on_leave)
		{
			p_server->p_on_leave(p_server, socket_fd);
		}
	}
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

void si_server_accept(si_server_t* const p_server)
{
	if (NULL == p_server)
	{
		goto END;
	}
	const bool is_blocking = si_server_is_blocking(p_server);
	si_mutex_lock(&(p_server->sockets_lock));
	const si_poll_info* p_server_fd = si_poll_at(&(p_server->sockets), 0u);
	si_mutex_unlock(&(p_server->sockets_lock));
	if (NULL == p_server_fd)
	{
		// Invalid array
		goto END;
	}
	const int server_fd = p_server_fd->fd;
	if (SOCKET_SUCCESS > server_fd)
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
	if (SOCKET_SUCCESS > client_fd)
	{
		goto ERROR;
	}

	// Validate access permission(s)
	if (NULL != p_server->p_access_list)
	{
		const bool is_allowed = si_accesslist_is_allowed(
			p_server->p_access_list, (struct sockaddr*)&client_addr
		);
		if (true != is_allowed)
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
	if (false == added_socket)
	{
		goto ERROR;
	}
	si_logger_custom(
		p_server->p_logger, SI_LOGGER_INFO,
		"Client connected: ", &client_addr, "\n",
		(void(*)(FILE* const,  const void* const))sockaddr_fprint
	);
	if (NULL != p_server->p_on_connect)
	{
		const bool event_result = p_server->p_on_connect(p_server, client_fd);
		if (true != event_result)
		{
			si_server_drop_socket(p_server, client_fd);
		}
	}
	goto END;
ERROR:
	if (!((EAGAIN == errno) && (false == is_blocking)))
	{
		si_logger_error(p_server->p_logger,
			"Server accept() failed with the error: %s", strerror(errno)
		);
	}
	if (SOCKET_SUCCESS <= client_fd)
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
	if ((NULL == p_server) || (NULL == p_buffer) || (0u >= buffer_size))
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	for (size_t iii = 1u; iii < p_server->sockets.capacity; iii++)
	{
		si_poll_info* p_next_poll = si_poll_at(&(p_server->sockets), iii);
		if (NULL == p_next_poll)
		{
			continue;
		}
		const int next_fd = p_next_poll->fd;
		if ((SOCKET_SUCCESS > next_fd) || (sender_fd == next_fd))
		{
			continue;
		}
		ssize_t send_result = send(next_fd, p_buffer, buffer_size, 0);
		if (SOCKET_ERROR >= send_result)
		{
			(void)close(next_fd);
			p_next_poll->fd = SOCKET_ERROR;
			si_server_drop_socket(p_server, next_fd);
			continue;
		}
	}
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

/** Doxygen
 * @brief Calls the server's handle_read/handle_write functions based upon the
 *        poll event results for a single socket. Handles disconnects / errors.
 * 
 * @param p_server Pointer to the si_server struct to call functions from.
 * @param p_fd Pointer to pollfd struct with socket descriptor & event flags
 */
static void si_server_handle_socket(si_server_t* const p_server,
	si_poll_info* const p_fd)
{
	if (NULL == p_fd)
	{
		goto END;
	}
	if ((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		goto CLOSE;
	}
	if (p_fd->revents & POLLIN)
	{
		if (NULL == p_server->p_on_read)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLIN);
		}
		else
		{
			const bool event_result = p_server->p_on_read(p_server, p_fd->fd);
			if (true != event_result)
			{
				p_fd->revents |= POLLHUP;
				goto CLOSE;
			}
		}
	}
	if (p_fd->revents & POLLOUT)
	{
		if (NULL == p_server->p_on_write)
		{
			// We don't handle it so turn it off.
			p_fd->revents &= (~POLLOUT);
		}
		else
		{
			const bool event_result = p_server->p_on_write(p_server, p_fd->fd);
			if (true != event_result)
			{
				p_fd->revents |= POLLHUP;
				goto CLOSE;
			}
		}
	}
CLOSE:
	if ((p_fd->revents & POLLHUP) || (p_fd->revents & POLLERR))
	{
		si_server_drop_socket(p_server, p_fd->fd);
	}
END:
	return;
}

void si_server_handle_events(si_server_t* const p_server)
{
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	const bool is_blocking = si_server_is_blocking(p_server);
	const int timeout_mills = is_blocking ? -1 : DEFAULT_POLL_TIMEOUT;
	const size_t capacity = p_server->sockets.capacity;
	const int poll_result = poll(
		p_server->sockets.p_data, capacity, timeout_mills
	);
	// Poll client events
	if (SOCKET_SUCCESS > poll_result)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	// Handle poll event results
	for (size_t iii = 1u; iii < capacity; iii++)
	{
		si_poll_info* p_fd = NULL;
		p_fd = si_poll_at(&(p_server->sockets), iii);
		if (NULL == p_fd)
		{
			break;
		}
		const bool is_valid = si_socket_is_valid(&(p_fd->fd));
		if ((true == is_valid) && (0 != p_fd->revents))
		{
			si_server_handle_socket(p_server, p_fd);
		}
	}
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

/** Doxygen
 * @brief Local function closes server socket if open/defined.
 * 
 * @param p_server Pointer to the si_server_t to close the server socket of.
 */
static void si_server_close(si_server_t* const p_server)
{
	if (NULL == p_server)
	{
		goto END;
	}
	si_mutex_lock(&(p_server->sockets_lock));
	si_poll_info* const p_server_info = si_poll_at(
		&(p_server->sockets), 0u
	);
	if (NULL == p_server_info)
	{
		si_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	si_socket_close(&(p_server_info->fd));
	si_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

void si_server_free(si_server_t* const p_server)
{
	if (NULL == p_server)
	{
		goto END;
	}
	si_server_close(p_server);
	p_server->family = AF_UNSPEC;
	p_server->p_access_list = NULL;
	si_mutex_lock(&(p_server->sockets_lock));
	si_poll_free(&(p_server->sockets));
	si_mutex_unlock(&(p_server->sockets_lock));
	si_mutex_free(&(p_server->sockets_lock));
	p_server->p_settings = NULL;
	p_server->p_on_connect = NULL;
	p_server->p_on_read = NULL;
	p_server->p_on_write = NULL;
	p_server->p_on_leave = NULL;
	p_server->p_logger = NULL;
END:
	return;
}

void si_server_destroy(si_server_t** const pp_server)
{
	if (NULL == pp_server)
	{
		goto END;
	}
	if (NULL == *pp_server)
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
