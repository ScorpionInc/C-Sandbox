/* si_socket.h
 * Created: 20251027
 * Updated: 20251027
 * Purpose: Defines function(s)/type(s) for working with cross-platform sockets
 */

#if (defined(__APPLE__) || defined(__linux__) || defined(__unix__))
#define SI_UNIX (1)
#endif// Test for Unix-like OSs

#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)


#if defined(SI_UNIX)

#include <fcntl.h> // fcntl(), O_NONBLOCK
#include <sys/socket.h> // socket(), listen(), SOL_SOCKET, SOMAXCONN
#include <sys/un.h> // sockaddr_un
#include <unistd.h> // close()

#define si_socket_t int
#define si_socket_invalid (-1)

#elif defined(_WIN32)

#include <winsock2.h> // closesocket()

// Needs to be linked with ws2_32
#pragma comment(lib, "ws2_32.lib")

#define si_socket_t SOCKET
#define si_socket_invalid (INVALID_SOCKET)

#else
#warning Unknown/Unsupported OS
#endif// OS Specific includes/defines


#include <errno.h> // errno
#include <limits.h> // INT_MAX
#include <stdbool.h> // bool, false, true
#include <stdlib.h> // calloc(), free()

#ifndef SI_SOCKET_H
#define SI_SOCKET_H

/** Doxygen
 * @brief Uses error code from linux socket() to determine if IPv6 is supported
          on this machine.
 *
 * @return Returns stdbool true if ipv6 is supported. Returns false otherwise.
 */
bool is_ipv6_supported(void);

/** Doxygen
 * @brief Initializes an existing socket's value(s).
 * 
 * @param p_socket Pointer to the si_socket_t to be initialized
 * @param family Desired socket domain/family to be initialized to.
 * @param type Type of the socket to be initialized.
 * @param protocol (Optional) Specify the desired protocol of the socket.
 */
void si_socket_init_4(si_socket_t* const p_socket, const int family,
	const int type, const int protocol);
void si_socket_init(si_socket_t* const p_socket, const int family,
	const int type);

/** Doxygen
 * @brief Heap allocates and initializes a new si_socket_t.
 * 
 * @param family Desired socket domain/family to be initialized to.
 * @param type Type of the socket to be initialized.
 * @param protocol (Optional) Specify the desired protocol of the socket.
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_socket_t* si_socket_new_3(const int family, const int type,
	const int protocol);
si_socket_t* si_socket_new(const int family, const int type);

/** Doxygen
 * @brief Validates the socket being pointed at.
 * 
 * @param p_socket Pointer to the si_socket_t to be validated.
 * 
 * @return Returns stdbool true on valid. Returns false otherwise.
 */
bool si_socket_is_valid(const si_socket_t* const p_socket);

/** Doxygen
 * @brief Determines if the socket being pointed at is invalid.
 * 
 * @param p_socket Pointer to the si_socket_t to be checked.
 * 
 * @return Returns stdbool true on invalid. Returns false otherwise.
 */
bool si_socket_is_invalid(const si_socket_t* const p_socket);

/** Doxygen
 * @brief Determines if the socket being pointed at is blocking.
 * 
 * @param p_socket Pointer to the si_socket_t to be checked.
 * 
 * @return Returns stdbool true on blocking. Returns false otherwise.
 */
bool si_socket_is_blocking(const si_socket_t* const p_socket);

/** Doxygen
 * @brief Sets the socket being pointed to desired blocking mode.
 * 
 * @param p_socket Pointer to the si_socket_t to be modified.
 * @param is_blocking Flag stdbool of desired blocking mode.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_socket_set_blocking(const si_socket_t* const p_socket,
	const bool is_blocking);

/** Doxygen
 * @brief Determines if the socket being pointed at is keepalive mode.
 * 
 * @param p_socket Pointer to the si_socket_t to be checked.
 * 
 * @return Returns stdbool true on keepalive. Returns false otherwise.
 */
bool si_socket_is_keepalive(const si_socket_t* const p_socket);

/** Doxygen
 * @brief Sets the socket being pointed to desired keepalive mode.
 * 
 * @param p_socket Pointer to the si_socket_t to be modified.
 * @param is_keepalive Flag stdbool of desired keepalive mode.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_socket_set_keepalive(const si_socket_t* const p_socket,
	const bool is_keepalive);

/** Doxygen
 * @brief Closes pointed at socket if it's valid.
 * 
 * @param p_socket Pointer to the si_socket_t to be closed.
 */
void si_socket_close(si_socket_t* const p_socket);

#endif//SI_SOCKET_H