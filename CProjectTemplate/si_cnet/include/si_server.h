/* si_server.h
 * Created: 20250718
 * Updated: 20251020
 * Purpose: Prototype functions and structs for server-side networking.
 */

#include <errno.h> // errno, strerror()
#include <limits.h> // INT_MAX, ULONG_MAX
#include <stdbool.h> // true false
#include <stdint.h> // SIZE_MAX
#include <stdlib.h> // calloc()
#include <string.h> // memset()

#include "si_mutex.h" // si_mutex_t

#ifdef __linux__

#include <arpa/inet.h> // htonl(), htons()

#include <fcntl.h> // fcntl(), O_NONBLOCK
#include <netdb.h> // addrinfo
#include <poll.h> // poll(), pollfd

#include <sys/resource.h> // getrlimit(), rlimit
#include <sys/socket.h> // socket(), listen(), SOL_SOCKET, SOMAXCONN
#include <sys/stat.h> // stat()
#include <sys/types.h> // ssize_t

#include <unistd.h> // read(), write()

#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)

#define DEFAULT_POLL_TIMEOUT (100)
// Default is: TCP/IPv4
#define DEFAULT_FAMILY AF_INET
#define DEFAULT_TYPE SOCK_STREAM

#else
#error Unsupported Operating System.
#endif // __linux__

#include "si_access_list.h"
#include "si_array.h"
#include "si_realloc_settings.h"
#include "si_logger.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifndef SI_SERVER_H
#define SI_SERVER_H

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

struct si_server_t;
typedef bool (*event_handler_t)(struct si_server_t* const, const int);

typedef struct si_server_t
{
	sa_family_t family;
	si_accesslist_t* p_access_list;
	si_mutex_t sockets_lock;
	si_array_t sockets;
	si_realloc_settings_t* p_settings;
	event_handler_t p_on_connect;
	event_handler_t p_on_read;
	event_handler_t p_on_write;
	event_handler_t p_on_leave;
	si_logger_t* p_logger;
} si_server_t;

/** Doxygen
 * @brief Initializes an existing si_server_t with default values.
 * 
 * @param p_server Pointer to the si_server_t to be initialized.
 * @param p_bind_target Pointer to value to bind server socket to.
 * @param type Type of socket to be used by the server.
 * @param family Address family value of the server socket.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 */
void si_server_init_7(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue, const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
void si_server_init_6(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue, const si_realloc_settings_t* const p_settings);
void si_server_init_5(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family,
	const int max_client_queue);
void si_server_init_4(si_server_t* const p_server,
	const void* const p_bind_target, const int type, const int family);
void si_server_init_3(si_server_t* const p_server,
	const void* const p_bind_target, const int type);
void si_server_init(si_server_t* const p_server,
	const void* const p_bind_target);

/** Doxygen
 * @brief Allocated and initializes an existing si_server_t with default values
 * 
 * @param p_server Pointer to the si_server_t to be initialized.
 * @param p_bind_target Pointer to value to bind server socket to.
 * @param type Type of socket to be used by the server.
 * @param family Address family value of the server socket.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 */
si_server_t* si_server_new_6(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
si_server_t* si_server_new_5(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue,
	const si_realloc_settings_t* const p_settings);
si_server_t* si_server_new_4(const void* const p_bind_target, const int type,
	const int family, const int max_client_queue);
si_server_t* si_server_new_3(const void* const p_bind_target, const int type,
	const int family);
si_server_t* si_server_new_2(const void* const p_bind_target, const int type);
si_server_t* si_server_new(const void* const p_bind_target);

/** Doxygen
 * @brief Initializes an existing si_server_t with AF_INET(4/6) default values.
 * 
 * @param p_server Pointer to the si_server_t to be initialized.
 * @param port Port number to bind server to.
 * @param type Type of socket to be used by the server.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 */
void si_server_init_inet_6(si_server_t* const p_server, const uint16_t port,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
void si_server_init_inet_5(si_server_t* const p_server, const uint16_t port,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings);
void si_server_init_inet_4(si_server_t* const p_server, const uint16_t port,
	const int type, const int max_client_queue);
void si_server_init_inet_3(si_server_t* const p_server, const uint16_t port,
	const int type);
void si_server_init_inet(si_server_t* const p_server, const uint16_t port);

/** Doxygen
 * @brief Allocates and initializes an existing si_server_t with AF_INET(4/6).
 * 
 * @param port Port number to bind server to.
 * @param type Type of socket to be used by the server.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_server_t* si_server_new_inet_5(const uint16_t port, const int type,
	const int max_client_queue, const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
si_server_t* si_server_new_inet_4(const uint16_t port, const int type,
	const int max_client_queue, const si_realloc_settings_t* const p_settings);
si_server_t* si_server_new_inet_3(const uint16_t port, const int type,
	const int max_client_queue);
si_server_t* si_server_new_inet_2(const uint16_t port, const int type);
si_server_t* si_server_new_inet(const uint16_t port);

/** Doxygen
 * @brief Initializes an existing si_server_t with AF_UNIX default values.
 * 
 * @param p_server Pointer to the si_server_t to be initialized.
 * @param p_path C string file path to use for file socket.
 * @param type Type of socket to be used by the server.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 */
void si_server_init_unix_6(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
void si_server_init_unix_5(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings);
void si_server_init_unix_4(si_server_t* const p_server,
	const char* const p_path, const int type, const int max_client_queue);
void si_server_init_unix_3(si_server_t* const p_server,
	const char* const p_path, const int type);
void si_server_init_unix(si_server_t* const p_server,
	const char* const p_path);

/** Doxygen
 * @brief Allocates and initializes an existing si_server_t with AF_UNIX values
 * 
 * @param port Port number to bind server to.
 * @param type Type of socket to be used by the server.
 * @param max_client_queue Number of clients to queue to handle by server.
 * @param p_settings (Optional) Pointer to reallocation settings to use.
 * @param p_logger (Optional) Pointer to si_logger_t to handle error messages.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_server_t* si_server_new_unix_5(const char* const p_path,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings,
	si_logger_t* const p_logger);
si_server_t* si_server_new_unix_4(const char* const p_path,
	const int type, const int max_client_queue,
	const si_realloc_settings_t* const p_settings);
si_server_t* si_server_new_unix_3(const char* const p_path,
	const int type, const int max_client_queue);
si_server_t* si_server_new_unix_2(const char* const p_path,
	const int type);
si_server_t* si_server_new_unix(const char* const p_path);

/** Doxygen
 * @brief Determines the number of valid client file descriptors in server.
 * 
 * @param p_server Pointer to the si_server_t to check.
 * 
 * @return Returns client count on success. Returns SIZE_MAX otherwise.
 */
size_t si_server_count_clients(si_server_t* const p_server);

/** Doxygen
 * @brief Determines if server is configured to function in a blocking mode.
 * 
 * @param p_server Pointer to the si_server_t to check.
 * 
 * @return Returns stdbool true if blocking. Returns false otherwise.
 */
bool si_server_is_blocking(si_server_t* const p_server);

/** Doxygen
 * @brief Sets the execution of the server to either blocking or non-blocking.
 * 
 * @param p_server Pointer to the si_server_t to be modified.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_server_set_blocking(si_server_t* const p_server, const bool blocking);

/** Doxygen
 * @brief Determines if the server is using keepalive messages.
 * 
 * @param p_server Pointer to the si_server_t to check.
 *
 * @return Returns stdbool true if keepalive. Returns false otherwise.
 */
bool si_server_is_keepalive(si_server_t* const p_server);

/** Doxygen
 * @brief Enables or disables keep alive messages on the server.
 * 
 * @param p_server Pointer to the si_server_t to be modified.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_server_set_keepalive(si_server_t* const p_server,
	const bool keepalive);

/** Doxygen
 * @brief Appends an existing client socket to the servers poll array.
 * 
 * @param p_server pointer to the si_server_t to append socket to.
 * @param socket_fd Client file descriptor to be appended.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_server_add_socket(si_server_t* const p_server, const int socket_fd);

/** Doxygen
 * @brief Closes and removes first socket of file descriptor from si_server_t.
 * 
 * @param p_server Pointer to si_server_t to remove socket from.
 * @param socket_fd Which socket file descriptor value to be removed.
 */
void si_server_drop_socket(si_server_t* const p_server, const int socket_fd);

/** Doxygen
 * @brief Accepts a new incoming connection adding them to the client list.
 * 
 * @param p_server Pointer to the si_server_t to accept a client for.
 */
void si_server_accept(si_server_t* const p_server);

/** Doxygen
 * @brief Sends a data buffer to all clients in a si_server_t skipping sender.
 * 
 * @param p_server Pointer to the si_server_t to send message on.
 * @param p_buffer Pointer to the data buffer to be sent.
 * @param buffer_size Size of the data_buffer in bytes.
 * @param sender_fd Socket file descriptor of the sender to be skipped.
 */
void si_server_broadcast(si_server_t* const p_server,
	const uint8_t* const p_buffer, const size_t buffer_size,
	const int sender_fd);

/** Doxygen
 * @brief Uses poll and event handlers to handle pending read/write operations.
 * 
 * @param p_server Pointer to the si_server_t to handle it's clients.
 */
void si_server_handle_events(si_server_t* const p_server);

/** Doxygen
 * @brief Frees allocated memory inside an existing si_server_t struct.
 * 
 * @param p_server Pointer to a si_server_t to free the contents of.
 */
void si_server_free(si_server_t* const p_server);

/** Doxygen
 * @brief Frees heap si_server_t struct and all of its internal contents.
 * 
 * @param pp_server Pointer to the si_server_t heap pointer to be freed.
 */
void si_server_destroy(si_server_t** const pp_server);

#endif//SI_SERVER_H

#ifdef __cplusplus
}
#endif //__cplusplus
