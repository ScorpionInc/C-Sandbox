// si_socket.c
#include "si_socket.h"

bool is_ipv6_supported(void)
{
	bool result = false;
#ifdef AF_INET6
#if defined(SI_UNIX)
	int tmp = socket(AF_INET6, SOCK_STREAM, 0);
	if (SOCKET_ERROR >= tmp)
	{
		result = (errno != EAFNOSUPPORT);
		goto END;
	}
	result = true;
	(void)close(tmp);
	tmp = SOCKET_ERROR;
#else
	// AF_INET6 is defined so we are just going to assume yes.
	result = true;
	goto END;
#endif // OS Specific implementation
#endif // AF_INET6
END:
	return result;
}


void si_socket_init_4(si_socket_t* const p_socket, const int family,
	const int type, const int protocol)
{
	if (NULL == p_socket)
	{
		goto END;
	}
#if (defined(__linux__) || defined(_WIN32))
	*p_socket = socket(family, type, protocol);
#else
#warning Unknown/Unsupported OS
#endif// OS Specific Implementation
END:
	return;
}
inline void si_socket_init(si_socket_t* const p_socket, const int family,
	const int type)
{
	// Default value of protocol is (0)
	si_socket_init_4(p_socket, family, type, 0);
}

si_socket_t* si_socket_new_3(const int family, const int type,
	const int protocol)
{
	si_socket_t* p_result = NULL;
	p_result = calloc(1u, sizeof(si_socket_t));
	if (NULL == p_result)
	{
		goto END;
	}
	si_socket_init_4(p_result, family, type, protocol);
END:
	return p_result;
}
si_socket_t* si_socket_new(const int family, const int type)
{
	// Default value of protocol is (0)
	return si_socket_new_3(family, type, 0);
}

bool si_socket_is_valid(const si_socket_t* const p_socket)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
#ifdef SI_UNIX
	result = (si_socket_invalid < (*p_socket));
#elif defined(_WIN32)
	result = (si_socket_invalid != (*p_socket));
#else
#warning Unknown/Unsupported OS
#endif// OS Specific implementation
END:
	return result;
}
inline bool si_socket_is_invalid(const si_socket_t* const p_socket)
{
	return(!si_socket_is_valid(p_socket));
}

#ifdef __linux__

bool si_socket_is_blocking(const si_socket_t* const p_socket)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(p_socket);
	if (true != is_valid)
	{
		// Socket is invalid :-(
		goto END;
	}

	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(*p_socket, F_GETFL, 0);
	if (si_socket_invalid >= flags)
	{
		// Failed to get current flags
		goto END;
	}
	result = (0 == (flags & O_NONBLOCK));
END:
	return result;
}

bool si_socket_set_blocking(const si_socket_t* const p_socket,
	const bool is_blocking)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(p_socket);
	if (true != is_valid)
	{
		// Socket is invalid :-(
		goto END;
	}

	// Get all current flags and check if change is needed.
	// in fcntl() the kernel ignores 3rd argument with F_GETFL.
	const int flags = fcntl(*p_socket, F_GETFL, 0);
	if (si_socket_invalid >= flags)
	{
		// Failed to get flags
		goto END;
	}
	if (is_blocking == (0 == (flags & O_NONBLOCK)))
	{
		// Already in the desired mode
		result = true;
		goto END;
	}

	// Mask/Set new flag while keeping any old flags.
	int new_flags = 0;
	if (true == is_blocking)
	{
		new_flags = (flags & (~O_NONBLOCK));
	}
	else
	{
		new_flags = (flags | O_NONBLOCK);
	}

	// Validate socket mode changed
	const int fcntl_result = fcntl(*p_socket, F_SETFL, new_flags);
	if (si_socket_invalid >= fcntl_result)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

#elif defined(_WIN32)

bool si_socket_is_blocking(const si_socket_t* const p_socket)
{
	// Windows can't check if SOCKET is blocking after WSAIsBlocking deprecated
	// Instead we will check if a timed function call would block or not.
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	DWORD initial_timeout = 0U;
	DWORD temporary_timeout = 1U;

	int timeout_size = 0;
	const size_t dword_size = sizeof(DWORD);
	// Should never happen but just in case.
	if (INT_MAX < dword_size)
	{
		goto END;
	}
	
	// Get the current timeout flag value.
	timeout_size = (int)dword_size;
	const int get_initial_result = getsockopt(
		*p_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&initial_timeout, &timeout_size
	);
	if (NO_ERROR != get_initial_result)
	{
		// Failed to get initial timeout state
		goto END;
	}

	// Set the timeout flag (if needed)
	timeout_size = (int)dword_size;
	if (initial_timeout != temporary_timeout)
	{
		const int set_temp_result = setsockopt(
			*p_socket, SOL_SOCKET, SO_RCVTIMEO,
			(const char*)&temporary_timeout, timeout_size
		);
		if (NO_ERROR != set_temp_result)
		{
			// Failed to set temporary timeout state
			goto END;
		}
	}

	// Run a recv call which may timeout.
	char chr = '\0';
	// Length parameter of 0 to prevent actually reading anything.
	const int recv_result = recv(*p_socket, &chr, 0, 0);

	// Restore initial timeout flag state (if needed)
	timeout_size = (int)dword_size;
	if (initial_timeout != temporary_timeout)
	{
		const int restore_result = setsockopt(
			*p_socket, SOL_SOCKET, SO_RCVTIMEO,
			(const char*)&initial_timeout, timeout_size
		);
		if (NO_ERROR != restore_result)
		{
			// Failed to set restore timeout state
			goto END;
		}
	}

	// Parse returned/error result(s).
	int error = NO_ERROR;
	if (error != recv_result)
	{
		error = WSAGetLastError();
	}
	result = (error != WSAEWOULDBLOCK);
END:
	return result;
}

bool si_socket_set_blocking(const si_socket_t* const p_socket,
	const bool is_blocking)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	u_long flag = is_blocking ? 0UL : 1UL;
	const int ioctls = ioctlsocket(socket, FIONBIO, &flag);
	result = (NO_ERROR == ioctls);
END:
	return result;
}

#else
#warning Unknown/Unsupported OS

bool si_socket_is_blocking(const si_socket_t* const p_socket)
{
	fprintf(stderr, "Call to OS specific implementation on an unknown OS.\n");
	return false;
}
bool si_socket_set_blocking(const si_socket_t* const p_socket,
	const bool is_blocking)
{
	fprintf(stderr, "Call to OS specific implementation on an unknown OS.\n");
	return false;
}

#endif // OS Specific implementation

#if (defined(__linux__) || defined(_WIN32))

bool si_socket_is_keepalive(const si_socket_t* const p_socket)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(p_socket);
	if (true != is_valid)
	{
		// Socket is invalid :-(
		goto END;
	}

	int value = 0;
	const socklen_t c_value_size = (socklen_t)sizeof(value);
	socklen_t value_size = c_value_size;
	const int get_result = getsockopt(
		*p_socket, SOL_SOCKET, SO_KEEPALIVE,
		&value, &value_size
	);
	if ((si_socket_invalid >= get_result) || (value_size != c_value_size))
	{
		goto END;
	}
	result = (0 < value);
END:
	return result;
}

bool si_socket_set_keepalive(const si_socket_t* const p_socket,
	const bool is_keepalive)
{
	bool result = false;
	if (NULL == p_socket)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(p_socket);
	if (true != is_valid)
	{
		// Socket is invalid :-(
		goto END;
	}

	int keepalive = is_keepalive ? 1 : 0;
	const socklen_t keepalive_size = (socklen_t)sizeof(keepalive);
	const int set_result = setsockopt(
		*p_socket, SOL_SOCKET, SO_KEEPALIVE,
		&keepalive, keepalive_size
	);
	result = (si_socket_invalid < set_result);
END:
	return result;
}

#else
#warning Unknown/Unsupported OS

bool si_socket_is_keepalive(const si_socket_t* const p_socket)
{
	fprintf(stderr, "Call to OS specific implementation on an unknown OS.\n");
	return false;
}
bool si_socket_set_keepalive(const si_socket_t* const p_socket,
	const bool is_keepalive)
{
	fprintf(stderr, "Call to OS specific implementation on an unknown OS.\n");
	return false;
}

#endif // OS Specific implementation

void si_socket_close(si_socket_t* const p_socket)
{
	if (NULL == p_socket)
	{
		goto END;
	}
	const bool is_valid = si_socket_is_valid(p_socket);
	if (true != is_valid)
	{
		goto END;
	}

#ifdef AF_UNIX
	// Handles the removing of created file(s) after a UNIX socket closes.
	const char* p_path = NULL;
	struct sockaddr_storage address = {0};
	socklen_t address_size = sizeof(address);
	const int get_address_result = getsockname(
		*p_socket, (struct sockaddr*)&address, &address_size
	);
	const sa_family_t family = address.ss_family;
	if (EXIT_SUCCESS == get_address_result)
	{
		const sa_family_t family = address.ss_family;
		if (AF_UNIX == family)
		{
			struct sockaddr_un* p_sun_addr = (struct sockaddr_un*)&address;
			const size_t sun_path_size = sizeof(p_sun_addr->sun_path);
			// Just in case ensure NULL terminated
			p_sun_addr->sun_path[sun_path_size - 1u] = '\0';
			p_path = p_sun_addr->sun_path;
		}
	}
#endif//AF_UNIX

#ifdef SI_UNIX
	close(*p_socket);
	*p_socket = si_socket_invalid;
#elif defined(_WIN32)
	closesocket(*p_socket);
	*p_socket = si_socket_invalid;
#else
#warning Unknown/Unsupported OS
#endif// OS Specific implementation

#ifdef AF_UNIX
	// Cleanup any created UNIX socket file(s)
	if (NULL != p_path)
	{
		unlink(p_path);
	}
#endif//AF_UNIX
END:
	return;
}
