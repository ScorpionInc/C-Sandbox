/* si_access_list.h
 * Author: Scorpioninc
 * Created: 20250801
 * Updated: 20250804
 * Purpose: List of network addresses for whitelisting or blacklisting.
 */

#include <stdbool.h> // true, false
#include <string.h> // memcpy(), memset()

#ifdef __linux__

#include <pthread.h> // pthread_mutex_t

#include <sys/socket.h> // sockaddr, AF_INET, AF_INET6
#define SOCKET_SUCCESS (0)
#define SOCKET_ERROR (-1)

#else
#error Unsupported Operating System.
#endif//__linux__

#include "si_array.h"
#include "si_realloc_settings.h"
#include "si_paddr.h"

#ifndef SI_ACCESSLIST_T
#define SI_ACCESSLIST_T

typedef struct si_accesslist_t
{
	bool is_blacklist;
	bool is_ipv4;
	pthread_mutex_t entries_lock;
	si_array_t entries;
	si_realloc_settings_t settings;
} si_accesslist_t;

/** Doxygen
 * @brief Initializes an accesslist struct that is already allocated.
 * 
 * @param p_access Pointer to the accesslist struct to be initialized.
 * @param is_blacklist stdbool flag if list is in a whitelist or blacklist mode
 * @param is_ipv4 stdbool flag if entires are sockaddr_in or sockaddr_in6.
 * @param p_settings Pointer to reallocation settings to clone from.
 */
void si_accesslist_init_4(si_accesslist_t* const p_access,
	const bool is_blacklist, const bool is_ipv4,
	const si_realloc_settings_t* const p_settings);
void si_accesslist_init(si_accesslist_t* const p_access,
	const bool is_blacklist, const bool is_ipv4);

/** Doxygen
 * @brief Allocates and initializes a new accesslist struct.
 * 
 * @param is_blacklist stdbool flag if list is in a whitelist or blacklist mode
 * @param is_ipv4 stdbool flag if entires are sockaddr_in or sockaddr_in6.
 * @param p_settings Pointer to reallocation settings to clone from.
 * 
 * @return Returns pointer to new struct on success. Returns NULL otherwise.
 */
si_accesslist_t* si_accesslist_new_3(const bool is_blacklist,
	const bool is_ipv4, const si_realloc_settings_t* const p_settings);
si_accesslist_t* si_accesslist_new(const bool is_blacklist,
	const bool is_ipv4);

/** Doxygen
 * @brief Returns true if the socket address at index is valid.
 * 
 * @param p_access Pointer to the accesslist to read from.
 * @param index Index of the entry to validate.
 * 
 * @return Returns stdbool true if valid. Returns false otherwise.
 */
bool si_accesslist_is_valid_at(si_accesslist_t* const p_access,
	const size_t index);

/** Doxygen
 * @brief Determines quantity of valid socket address entries in accesslist.
 * 
 * @param p_access Pointer to the accesslist to read from.
 * 
 * @return Returns size_t count of valid sockets. Returns SIZE_MAX otherwise.
 */
size_t si_accesslist_count(si_accesslist_t* const p_access);

/** Doxygen
 * @brief Determines if none of the allocated memory is valid.
 * 
 * @param p_access Pointer to the accesslist to read from.
 * 
 * @return Returns stdbool true if empty. Returns false otherwise.
 */
bool si_accesslist_is_empty(si_accesslist_t* const p_access);

/** Doxygen
 * @brief Determines if all of the allocated entries contain valid addresses.
 * 
 * @param p_access Pointer to the accesslist to read from.
 * 
 * @return Returns stdbool true if full. Returns false otherwise.
 */
bool si_accesslist_is_full(si_accesslist_t* const p_access);

/** Doxygen
 * @brief Finds first matching address index in the accesslist.
 * 
 * @param p_access Pointer to the access list to search for address.
 * @param p_addr Pointer to socket address to find in the list.
 * 
 * @return Returns size_t index of first match on success. SIZE_MAX otherwise.
 */
size_t si_accesslist_index_of(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr);

/** Doxygen
 * @brief Determines if the list has an address within it's entries.
 * 
 * @param p_access Pointer to the accesslist to search in.
 * @param p_addr Pointer to the socket address to test for
 * 
 * @return Returns stdbool true if p_addr exists in entries, false otherwise.
 */
bool si_accesslist_has(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr);

/** Doxygen
 * @brief Adds an address to accesslist grows capacity if needed by settings.
 * 
 * @param p_access Pointer to the accesslist to add address to.
 * @param p_addr Pointer to address to add to entries from.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_accesslist_append(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr);

/** Doxygen
 * @brief Removes an address from the accesslist by index. Doesn't shrink.
 * 
 * @param p_access Pointer to the accesslist to remove address from.
 * @param index size_t index of entry to be removed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_accesslist_remove_at(si_accesslist_t* const p_access,
	const size_t index);

/** Doxygen
 * @brief Removes first occurance of an address from accesslist.
 * 
 * @param p_access Pointer to the accesslist to remove address from.
 * @param p_addr Pointer to the socket address to be removed.
 * 
 * @return Returns true on success. Returns false otherwise.
 */
bool si_accesslist_remove(si_accesslist_t* const p_access,
	const struct sockaddr* const p_addr);

/** Doxygen
 * @brief Frees allocated memory within an accesslist at pointer.
 * 
 * @param p_access Pointer to accesslist to have it's contents freed.
 */
void si_accesslist_free(si_accesslist_t* const p_access);

/** Doxygen
 * @brief Frees accesslist at pp_access and any memory allocated within.
 * 
 * @param pp_access Pointer to pointer of accesslist to be freed.
 */
void si_accesslist_destroy(si_accesslist_t** pp_access);

#endif//SI_ACCESSLIST_T