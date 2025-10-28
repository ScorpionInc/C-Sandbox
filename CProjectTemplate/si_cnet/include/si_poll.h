/* si_poll.h
 * Created: 20251027
 * Updated: 20251027
 * Purpose: Defines function(s)/type(s) for working with concurrent socket io.
 */

#include "si_array.h" // si_array_t, si_array_at(), si_array_resize()
#include "si_realloc_settings.h" // si_realloc_settings_t, ...settings_grow()
#include "si_socket.h" // si_socket_close(), si_socket_t, si_socket_is_valid()

#if (defined(__APPLE__) || defined(__linux__) || defined(__unix__))
#define SI_UNIX (1)
#endif// Test for Unix-like OSs

#if defined(SI_UNIX)

#include <poll.h> // pollfd, poll()

#define si_poll_error (-1)
#define si_poll_info struct pollfd

#elif defined(_WIN32)

#include <winsock2.h> // WSAPOLLFD, WSAPoll()

// Needs to be linked with ws2_32
#pragma comment(lib, "ws2_32.lib")

#define si_poll_error (SOCKET_ERROR)
#define si_poll_info WSAPOLLFD

#else
#error Unknown/Unsupported OS
#endif// OS Specific includes/defines

#define si_poll_t si_array_t

#include <stdbool.h> // bool, false, true
#include <stddef.h> // size_t
#include <stdlib.h> // calloc(), free()

#ifndef SI_POLL_H
#define SI_POLL_H

/** Doxygen
 * @brief Initializes an existing si_poll_t struct's values.
 * 
 * @param p_poll Pointer to the si_poll_t struct to be initialized.
 * @param capacity size_t Number of elements to allocate within the array.
 * @param events initial value of the events field of each element in the array
 */
void si_poll_init_3(si_poll_t* const p_poll, const size_t capacity,
	const short events);
void si_poll_init(si_poll_t* const p_poll, const size_t capacity);

/** Doxygen
 * @brief Allocates/Initializes a new heap si_poll_t struct for use in poll().
 * 
 * @param capacity size_t Number of elements to allocate within the array.
 * @param events initial value of the events field of each element in the array
 * 
 * @return Returns heap pointer on success. Returns NULL otherwise.
 */
si_poll_t* si_poll_new_2(const size_t capacity, const short events);
si_poll_t* si_poll_new(const size_t capacity);

/** Doxygen
 * @brief Gets the number of initialized socket values in a poll array.
 * 
 * @param p_poll Pointer si_poll_t struct to be walked/counted.
 * @param mask short Specifies revent filter to count. 0 counts all.
 * @param start size_t Offset to start counting from within array.
 * @param count size_t Maximum number of elements to count in the array.
 * 
 * @return Returns size_t count of initialized sockets that match mask.
 */
size_t si_poll_count_4(const si_poll_t* const p_poll, const short mask,
	const size_t start, const size_t count);
size_t si_poll_count_3(const si_poll_t* const p_poll, const short mask,
	const size_t start);
size_t si_poll_count_2(const si_poll_t* const p_poll, const short mask);
size_t si_poll_count(const si_poll_t* const p_poll);

/** Doxygen
 * @brief Find the first instance of si_poll_info where fd matches p_socket.
 * 
 * @param p_poll Pointer si_poll_t struct to be searched.
 * @param p_socket Pointer to si_socket_t to find in si_poll_t's array.
 * 
 * @return Returns array index on success. Returns SIZE_MAX otherwise.
 */
size_t si_poll_find(const si_poll_t* const p_poll,
	const si_socket_t* p_socket);

/** Doxygen
 * @brief Adds socket to si_poll_t array first available index, grows as needed
 * 
 * @param p_poll Pointer si_poll_t struct to be appended to.
 * @param p_socket Pointer to the si_socket_t value to be appended.
 * @param events short si_poll_info events mask value.
 * @param p_settings Pointer to si_realloc_settings to specify grow options.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_poll_append_4(si_poll_t* const p_poll,
	const si_socket_t* const p_socket, const short events,
	const si_realloc_settings_t* const p_settings);
bool si_poll_append(si_poll_t* const p_poll,
	const si_socket_t* const p_socket, const short events);

/** Doxygen
 * @brief Gets the address of the si_poll_info at index in si_poll_t on success
 * 
 * @param p_poll Pointer si_poll_t struct to be read.
 * @param index Array offset/index to get si_poll_info from.
 * 
 * @return Returns element pointer on success. Returns NULL otherwise.
 */
si_poll_info* si_poll_at(const si_poll_t* const p_poll, const size_t index);

/** Doxygen
 * @brief Calls poll on si_poll_t's array to query io state of si_poll_fd's
 * 
 * @param p_poll Pointer si_poll_t struct to be polled.
 * @param timeout Timeout value used with poll function. (<0 is blocking).
 * 
 * @return Will return si_poll_error on error or number of elements ready.
 */
int si_poll_update(si_poll_t* const p_poll, const int timeout);

/** Doxygen
 * @brief Closes valid si_poll_fd's within a si_poll_t struct's array at index.
 * 
 * @param p_poll Pointer si_poll_t struct that has the socket_t to be closed.
 * @param index size_t Index/Offset into the si_poll_t's array to close.
 */
void si_poll_close_at(si_poll_t* const p_poll, const size_t index);

/** Doxygen
 * @brief Closes all socket instances within a si_poll_t struct's array.
 * 
 * @param p_poll Pointer si_poll_t struct to be walked.
 * @param socket Socket value to be found and closed in poll array.
 * 
 * @return Returns true if at least one match was found. Otherwise false.
 */
bool si_poll_close(si_poll_t* const p_poll, const si_socket_t socket);

/** Doxygen
 * @brief Closes all valid si_poll_fd's within a si_poll_t struct's array.
 * 
 * @param p_poll Pointer si_poll_t struct to be closed.
 */
void si_poll_close_all(si_poll_t* const p_poll);

/** Doxygen
 * @brief Frees the allocated heap contents of a si_poll_t struct.
 * 
 * @param p_poll Pointer si_poll_t struct to have contents freed.
 */
void si_poll_free(si_poll_t* const p_poll);

/** Doxygen
 * @brief Frees an allocated si_poll_info heap array by pointer. Doesn't close.
 * 
 * @param pp_poll Pointer to si_poll_info heap pointer to be freed.
 */
void si_poll_destroy(si_poll_t** const pp_poll);

#endif//SI_POLL_H
