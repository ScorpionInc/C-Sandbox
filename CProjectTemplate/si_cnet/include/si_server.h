/* si_server.h
 * Author: Scorpioninc
 * Created: 20250718
 * Updated: 20250729
 * Purpose: Protoype functions and structs for server-side networking.
 */

#ifndef SI_SERVER_H
#define SI_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <errno.h> // errno, strerror()
#include <limits.h> // ULONG_MAX
#include <stdbool.h> // true false
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // calloc()
#include <string.h> // memset()

#ifdef __linux__

#ifndef _POSIX_C_SOURCE
// Defines our minimum target POSIX standard
#define _POSIX_C_SOURCE 200809L
#endif//_POSIX_C_SOURCE

#include <arpa/inet.h> // inet_addr() htonl()

#include <fcntl.h> // fcntl(), O_NONBLOCK
#include <netdb.h> // addrinfo
#include <poll.h> // poll(), pollfd
#include <pthread.h> // pthread_mutex_t

#include <sys/resource.h> // getrlimit(), rlimit
#include <sys/socket.h> // socket(), listen(), SOL_SOCKET, SOMAXCONN, SO_KEEPALIVE
#include <sys/stat.h> // stat()
#include <sys/types.h> // ssize_t

#include <unistd.h> // read() write()

#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)

// Default is: TCP/IPv4
#define DEFAULT_FAMILY AF_INET
#define DEFAULT_TYPE SOCK_STREAM

#else
#error Unsupported Operating System.
#endif//__linux__

#include "si_access_list.h"
#include "si_array.h"
#include "si_realloc_settings.h"
#include "si_logger.h"

/** Doxygen
 * @brief Uses error code from linux socket() to determine if IPv6 is supported
          on this machine.
 *
 * @return Returns stdbool true if ipv6 is supported. Returns false otherwise.
 */
bool is_ipv6_supported(void);

/** Doxygen
 * @brief Gets current system client queue limit
 * 
 * @return Returns size_t client queue limit. SIZE_MAX on error.
 */
size_t get_client_queue_limit(void);

typedef struct si_server_t
{
	int family;
	si_accesslist_t* access_list;
	pthread_mutex_t sockets_lock;
	si_array_t sockets;
	si_realloc_settings_t* p_settings;
	void (*p_handle_read)(struct si_server_t* const, struct pollfd* const);
	void (*p_handle_write)(struct si_server_t* const, struct pollfd* const);
	si_logger_t* p_logger;
} si_server_t;

void si_server_init_7(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings, si_logger_t* const p_logger);
void si_server_init_6(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings);
void si_server_init_5(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue);

si_server_t* si_server_new_6(const unsigned short port, const int type,
	const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings, si_logger_t* const p_logger);
si_server_t* si_server_new_5(const unsigned short port, const int type,
	const int family, const int max_client_queue,
	si_realloc_settings_t* const p_settings);
si_server_t* si_server_new_4(const unsigned short port, const int type,
	const int family, const int max_client_queue);
si_server_t* si_server_new_3(const unsigned short port, const int type,
	const int family);
si_server_t* si_server_new_2(const unsigned short port, const int type);
si_server_t* si_server_new(const unsigned short port);

bool si_server_is_blocking(si_server_t* const p_server);
bool si_server_set_blocking(si_server_t* const p_server, const bool blocking);

bool si_server_is_keepalive(si_server_t* const p_server);
bool si_server_set_keepalive(si_server_t* const p_server, const bool keepalive);

bool si_server_add_socket(si_server_t* const p_server, const int socket_fd);
void si_server_drop_socket(si_server_t* const p_server, const int socket_fd);

void si_server_accept(si_server_t* const p_server);

void si_server_broadcast(si_server_t* const p_server,
	const uint8_t* const p_buffer, const size_t buffer_size);

void si_server_handle_events(si_server_t* const p_server);

void si_server_free(si_server_t* p_server);
void si_server_free_at(si_server_t** pp_server);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_SERVER_H