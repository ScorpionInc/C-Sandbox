#include <stdio.h> // printf()
#include <ctype.h> // toupper()

#ifdef __linux__
#include <poll.h> // poll()
#else
#error Unsupported Operating System.
#endif//__linux__

#include "si_server.h"

#define PORT (1234u)
#define BUF_SIZE (1024u)

/** Doxygen
 * @brief Modifies C-String at p_input_str to UPPER CASE.
 *
 * @param p_input_str Pointer to first char of C-String to be UPPER CASED.
 */
static void to_uppercase (char* const p_input_str)
{
	if(NULL == p_input_str)
	{
		goto END;
	}
	size_t offset = 0u;
	while(SIZE_MAX > offset)
	{
		char next_char = p_input_str[offset];
		if('\0' == next_char)
		{
			break;
		}
		p_input_str[offset] = toupper(next_char);
		offset++;
	}
END:
	return;
}

/** Doxygen
 * @brief Handles a single client's events
 *
 * @param p_fd Pointer to pollfd struct
 */
static void handle_events(struct pollfd* const p_fd)
{
	if(NULL == p_fd)
	{
		goto END;
	}
	// Handle input
	char buffer[BUF_SIZE];
	if(p_fd->revents & POLLIN)
	{
		memset(&buffer, 0x00, BUF_SIZE);
		ssize_t r_result = read(p_fd->fd, buffer, BUF_SIZE - 1u);
		if(0 >= r_result)
		{
			p_fd->revents |= POLLHUP;
			goto CLOSE;
		}
		buffer[r_result] = '\0';
		to_uppercase(buffer);
		ssize_t w_result = write(p_fd->fd, buffer, BUF_SIZE - 1u);
		if(0 >= w_result)
		{
			p_fd->revents |= POLLHUP;
			goto CLOSE;
		}
	}
CLOSE:
	// Handle close
	if(p_fd->revents & POLLHUP)
	{
		p_fd->events = 0;
		if(SOCKET_SUCCESS <= p_fd->fd)
		{
			close(p_fd->fd);
		}
		p_fd->fd = SOCKET_ERROR;
		goto END;
	}
END:
	return;
}

/** Doxygen
 * @brief Checks the clients for a si_server's poll events.
 *
 * @param p_server Pointer to si_server struct
 */
void handle_server(si_server_t* const p_server)
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
	// Poll client events
	if(SOCKET_SUCCESS >
		poll(p_server->sockets.p_data, capacity, -1))
	{
		pthread_mutex_unlock(&(p_server->sockets_lock));
		goto END;
	}
	// Handle clients
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
		handle_events(p_fd);
	}
	pthread_mutex_unlock(&(p_server->sockets_lock));
END:
	return;
}

int main(int argc, char** pp_argv)
{
	si_server_t* p_server = si_server_new(PORT);
	if(NULL == p_server)
	{
		goto END;
	}
	si_accesslist_t* p_access = si_accesslist_new(true, true);
	if(NULL == p_access)
	{
		free(p_server);
		p_server = NULL;
		goto END;
	}
	struct sockaddr_in* p_loopback = (struct sockaddr_in*)sockaddr_new(AF_INET);
	if(NULL == p_loopback)
	{
		free(p_access);
		free(p_server);
		p_access = NULL;
		p_server = NULL;
		goto END;
	}
	p_loopback->sin_addr.s_addr = htonl(0x7F000001);
	si_accesslist_append(p_access, (struct sockaddr*)p_loopback);
	p_server->access_list = p_access;
	si_server_set_blocking(p_server, false);
	while(NULL != p_server)
	{
		si_server_accept(p_server);
		handle_server(p_server);
	}
	if(NULL != p_access)
	{
		free(p_access);
		p_access = NULL;
	}
	si_server_free_at(&p_server);
END:
	return 0;
}
