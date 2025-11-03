#include <stdbool.h> // bool, true, false
#include <stdio.h> // printf()
#include <ctype.h> // toupper()

#ifdef __linux__

#include <poll.h> // poll()

#else
#error Unsupported Operating System.
#endif//__linux__

#include "si_signals.h"
#include "si_strings.h"
#include "si_server.h"
#include "si_paddr.h"

#define PORT (1234u)
#define BUF_SIZE (1024u)

/** Doxygen
 * @brief Local wrapper for si_server broadcast that prepends a clients address
 * 
 * @param p_server Pointer to si_server_t to broadcast message.
 * @param socket_fd To ID source socket and generate address string from.
 * @param p_msg Pointer to C String payload to be sent.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool send_message(struct si_server_t* const p_server,
	const int socket_fd, const char* const p_msg)
{
	bool result = false;
	if ((NULL == p_server) || (SOCKET_ERROR >= socket_fd) || (NULL == p_msg))
	{
		goto END;
	}
	char* p_message = NULL;
	size_t message_size = 0u;
	
	struct sockaddr_storage peer_address = {0};
	socklen_t peer_address_len = sizeof(peer_address);
	const int gpn_result = getpeername(
		socket_fd, (struct sockaddr*)&peer_address, &peer_address_len
	);
	char* p_tmp = NULL;
	if (SOCKET_SUCCESS != gpn_result)
	{
		p_tmp = strdup("*");
	}
	else
	{
		p_tmp = sockaddr_as_str((struct sockaddr*)&peer_address);
		if (NULL == p_tmp)
		{
			goto END;
		}
	}

	p_message = strv_clone_join(4u, NULL, "[", p_tmp, "]: ", p_msg);
	free(p_tmp);
	if (NULL == p_message)
	{
		goto END;
	}
	message_size = strnlen(p_message, INT64_MAX);

	si_server_broadcast(p_server, (uint8_t*)p_message, message_size, socket_fd);
	free(p_message);
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Handles a single client's connect event
 *
 * @param p_unused Pointer to server who owns the socket.
 * @param p_fd Pointer to pollfd struct
 * 
 * @return Returns true on success. Returns false otherwise.
 */
static bool handle_connect(struct si_server_t* const p_server,
	const int socket_fd)
{
	bool result = false;
	if ((NULL == p_server) || (SOCKET_ERROR >= socket_fd))
	{
		goto END;
	}
	const char* const p_message = "Has joined the server.\n";
	send_message(p_server, socket_fd, p_message);
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Handles a single client's input event
 *
 * @param p_unused Pointer to server who owns the socket.
 * @param p_fd Pointer to pollfd struct
 * 
 * @return Returns true on success. Returns false otherwise.
 */
static bool handle_input(struct si_server_t* const p_server,
	const int socket_fd)
{
	bool result = false;
	if ((NULL == p_server) || (SOCKET_ERROR >= socket_fd))
	{
		goto END;
	}
	// Handle input
	char buffer[BUF_SIZE] = {0};
	ssize_t r_result = read(socket_fd, buffer, BUF_SIZE - 1u);
	if (0 >= r_result)
	{
		goto END;
	}
	buffer[r_result] = '\0';

	send_message(p_server, socket_fd, buffer);
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Handles a single client's leave event
 *
 * @param p_unused Pointer to server who owns the socket.
 * @param p_fd Pointer to pollfd struct
 * 
 * @return Returns true on success. Returns false otherwise.
 */
static bool handle_leave(struct si_server_t* const p_server,
	const int socket_fd)
{
	bool result = false;
	if ((NULL == p_server) || (SOCKET_ERROR >= socket_fd))
	{
		goto END;
	}
	const char* const p_message = "Has left the server.\n";
	send_message(p_server, socket_fd, p_message);
	result = true;
END:
	return result;
}

volatile sig_atomic_t g_exit_signal = 0;

/** Doxygen
 * @brief Handles graceful exit from an exit signal
 * 
 * @param signal Signal code value received.
 */
static void handle_exit(const int signal)
{
	g_exit_signal++;
}

int main(int argc, char** pp_argv)
{
	set_exit_signal_handler(handle_exit);

	si_logger_t logger = {0};
	si_logger_init(&logger);
	logger.logging_level = SI_LOGGER_ALL;
	
	const uint16_t port = PORT;
	si_server_t* p_server = si_server_new_6(
		&port, DEFAULT_TYPE, DEFAULT_FAMILY,
		get_client_queue_limit(), NULL, &logger
	);
	if (NULL == p_server)
	{
		goto END;
	}
	p_server->p_on_connect = handle_connect;
	p_server->p_on_read = handle_input;
	p_server->p_on_leave = handle_leave;

	si_accesslist_t* p_access = si_accesslist_new(false, true);
	if (NULL == p_access)
	{
		free(p_server);
		p_server = NULL;
		goto END;
	}
	struct sockaddr_in* p_loopback = (struct sockaddr_in*)sockaddr_new(AF_INET);
	if (NULL == p_loopback)
	{
		free(p_access);
		free(p_server);
		p_access = NULL;
		p_server = NULL;
		goto END;
	}
	p_loopback->sin_addr.s_addr = htonl(0x7F000001);
	si_accesslist_append(p_access, (struct sockaddr*)p_loopback);
	p_server->p_access_list = p_access;

	si_server_set_blocking(p_server, false);
	while (0 == g_exit_signal)
	{
		si_server_accept(p_server);
		si_server_handle_events(p_server);
	}

	// Handle cleanup
	if (NULL != p_access)
	{
		si_accesslist_free(p_access);
		free(p_access);
		p_access = NULL;
	}
	if (NULL != p_loopback)
	{
		free(p_loopback);
		p_loopback = NULL;
	}
	si_server_destroy(&p_server);
	p_server = NULL;
	si_logger_free(&logger);
END:
	return 0;
}
