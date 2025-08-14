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
#include <fcntl.h> // fcntl(), O_NONBLOCK
#include <limits.h> // ULONG_MAX
#include <stdbool.h> // true false
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // calloc()
#include <string.h> // memset()
#include <netdb.h> // addrinfo
#include <unistd.h> // read() write()

#ifdef __linux__

#include <arpa/inet.h> // inet_addr() htonl()

#include <poll.h> // poll(), pollfd
#include <pthread.h> // pthread_mutex_t

#include <sys/resource.h> // getrlimit(), rlimit
#include <sys/socket.h> // socket(), listen(), SOMAXCONN
#include <sys/stat.h> // stat()
#include <sys/types.h> // ssize_t

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
#include "si_paddr.h"
#include "si_realloc_settings.h"

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
	si_realloc_settings_t settings;
} si_server_t;

void si_server_init_5(si_server_t* const p_server, const unsigned short port,
	const int type, const int family, const int max_client_queue);

si_server_t* si_server_new_4(const unsigned short port, const int type,
	const int family, const int max_client_queue);
si_server_t* si_server_new_3(const unsigned short port, const int type,
	const int family);
si_server_t* si_server_new_2(const unsigned short port, const int type);
si_server_t* si_server_new(const unsigned short port);

bool si_server_is_blocking(si_server_t* const p_server);
bool si_server_set_blocking(si_server_t* const p_server, const bool blocking);

bool si_server_add_socket(si_server_t* const p_server, const int socket_fd);

void si_server_accept(si_server_t* const p_server);

void si_server_free(si_server_t* p_server);
void si_server_free_at(si_server_t** pp_server);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_SERVER_H