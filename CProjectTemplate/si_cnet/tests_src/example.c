#include <stdio.h> // printf()
#include <ctype.h> // toupper()

#ifdef __linux__
#include <poll.h> // poll()
#else
#error Unsupported Operating System.
#endif//__linux__

#include "si_strings.h"
#include "si_server.h"
#include "si_paddr.h"

#define PORT (1234u)
#define BUF_SIZE (1024u)

/** Doxygen
 * @brief Handles a single client's input event
 *
 * @param p_unused Pointer to server who owns the socket.
 * @param p_fd Pointer to pollfd struct
 */
static void handle_input(struct si_server_t* const p_unused, struct pollfd* const p_fd)
{
	if(NULL == p_fd)
	{
		goto END;
	}
	// Handle input
	char buffer[BUF_SIZE] = {0};
	ssize_t r_result = read(p_fd->fd, buffer, BUF_SIZE - 1u);
	if(0 >= r_result)
	{
		p_fd->revents |= POLLHUP;
		goto END;
	}
	buffer[r_result] = '\0';

	char* p_message = NULL;
	size_t message_size = 0u;
	
	struct sockaddr_storage peer_address = {0};
	socklen_t peer_address_len = sizeof(peer_address);
	const int gpn_result = getpeername(
		p_fd->fd, (struct sockaddr*)&peer_address, &peer_address_len
	);
	if(SOCKET_SUCCESS != gpn_result)
	{
		goto END;
	}

	char* p_tmp = sockaddr_as_str((struct sockaddr*)&peer_address);
	p_message = strv_clone_join(4u, NULL, "[", p_tmp, "]: ", buffer);
	message_size = strnlen(p_message, INT64_MAX);
	free(p_tmp);

	si_server_broadcast(p_unused, (uint8_t*)p_message, message_size, p_fd->fd);
	free(p_message);
END:
	return;
}

int main(int argc, char** pp_argv)
{
	si_logger_t logger = {0};
	logger.logging_level = SI_LOGGER_ALL;
	logger.p_file = stdout;
	
	si_server_t* p_server = si_server_new_6(
		PORT, DEFAULT_TYPE, DEFAULT_FAMILY,
		get_client_queue_limit(), NULL, &logger
	);
	if(NULL == p_server)
	{
		goto END;
	}
	p_server->p_handle_read = handle_input;

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
		si_server_handle_events(p_server);
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
