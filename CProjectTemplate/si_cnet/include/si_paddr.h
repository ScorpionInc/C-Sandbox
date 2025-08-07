/* si_paddr.h
 * Author: Scorpioninc
 * Created: 20250804
 * Updated: 20250804
 * Purpose: Common functions for working with linux socket addresses.
 */

#ifndef SI_PADDR_H
#define SI_PADDR_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdbool.h> // true, false
#include <stdint.h> // SIZE_MAX
#include <stdio.h> // printf()
#include <stdlib.h> // calloc()
#include <string.h> // memcmp()

#ifdef __linux__

#include <netinet/in.h> // sockaddr_in, sockaddr_in6, in_port_t
#include <sys/socket.h> // sockaddr

#else
#error Unsupported Operating System.
#endif//__linux__

#define INET_ADDRESS_SIZE (4u)
#define INET6_ADDRESS_SIZE (16u)

/** Doxygen
 * @brief Determines if an IPv6 address was a mapped ipv4 address.
 * 
 * @param p_address Byte array holding a 16 byte ipv6 address to test.
 * 
 * @return Returns stdbool true if valid ipv4-mapped address or false otherwise
 */
bool is_ipv6_a_mapped_ipv4(const uint8_t p_address[INET6_ADDRESS_SIZE]);

/** Doxygen
 * @brief Gets the ipv4 address of a mapped ipv6 if valid. UINT32_MAX otherwise
 * 
 * @param p_address Byte array holding a 16 byte ipv6 address to convert.
 * 
 * @return Returns ipv4 address is ipv6 is valid mapped. UINT32_MAX otherwise.
 */
uint32_t ipv4_from_ipv6(const uint8_t p_address[INET6_ADDRESS_SIZE]);

/** Doxygen
 * @brief Determines the struct size of the sockaddr for by family enum.
 * 
 * @param family AF enum int value to ID the socket family.
 * 
 * @return Returns sizeof() the struct used that is associated by family on
 *         success. Otherwise returns SIZE_MAX on error.
 */
size_t sockaddr_sizeof(const sa_family_t family);

/** Doxygen
 * @brief Allocates and initializes family of a sockaddr based upon family enum
 * 
 * @param family AF enum int value to ID the socket family.
 * 
 * @return Returns pointer to new sockaddr on success. Returns NULL otherwise.
 */
struct sockaddr* sockaddr_new(const sa_family_t family);

/** Doxygen
 * @brief Compares ipv4 address family, values, and ports ignores zero/len
 * 
 * @param p_left  Pointer to ipv4 address to compare
 * @param p_right Pointer to ipv4 address to compare
 * 
 * @return Returns an int value of: -1, 0, or 1 like memcmp().
 */
int sockaddr_in_cmp(const struct sockaddr_in* const p_left,
	const struct sockaddr_in* const p_right);

/** Doxygen
 * @brief Compares ipv6 address family, values, and ports ignores flow/scope
 * 
 * @param p_left  Pointer to ipv6 address to compare
 * @param p_right Pointer to ipv6 address to compare
 * 
 * @return Returns an int value of: -1, 0, or 1 like memcmp().
 */
int sockaddr_in6_cmp(const struct sockaddr_in6* const p_left,
	const struct sockaddr_in6* const p_right);

/** Doxygen
 * @brief Compares socket addresses using the family of the structs to select
 *        an implimented compare function(sockaddr_in_cmp / sockaddr_in6_cmp).
 * 
 * @param p_left  Pointer to ipv6 address to compare
 * @param p_right Pointer to ipv6 address to compare
 * @param family Family of the addresses being compared
 * 
 * @return Returns an int value of: -1, 0, or 1 like memcmp().
 */
int sockaddr_cmp(const struct sockaddr* const p_left,
	const struct sockaddr* const p_right, const sa_family_t family);

/** Doxygen
 * @brief Conducts basic sanity checks on ipv4 address to determine validity.
 * 
 * @return Returns true on valid. Returns false otherwise.
 */
bool sockaddr_in_is_valid(struct sockaddr_in* const p_addr);

/** Doxygen
 * @brief Conducts basic sanity checks on ipv6 address to determine validity.
 * 
 * @return Returns true on valid. Returns false otherwise.
 */
bool sockaddr_in6_is_valid(struct sockaddr_in6* const p_addr);

/** Doxygen
 * @brief Conducts sanity tests based upon the socket family of the address.
 * 
 * @param p_addr Pointer to address struct to validate.
 * @param family sa_family_t enum value to ID the socket family.
 * 
 * @return Returns true if valid. Returns false otherwise.
 */
bool sockaddr_is_valid(struct sockaddr* const p_addr,
	const sa_family_t family);

/** Doxygen
 * @brief Prints the ipv4 4-byte formatted address from p_address.
 * 
 * @param p_file FILE pointer to write formated value to.
 * @param p_address Pointer to byte buffer of 4 bytes holding the raw address.
 */
void sin_addr_fprint(FILE* const p_file,
	const uint8_t p_address[INET_ADDRESS_SIZE]);

/** Doxygen
 * @brief Prints the ipv6 16-byte expanded formatted address from p_address.
 * 
 * @param p_file FILE pointer to write formated value to.
 * @param p_address Pointer to byte buffer of 16 bytes holding the raw address.
 */
void sin6_addr_fprint(FILE* const p_file,
	const uint8_t p_address[INET6_ADDRESS_SIZE]);

/** Doxygen
 * @brief Prints the socket address from p_addr based upon it's socklen value.
 * 
 * @param p_file FILE pointer to write formated value to.
 * @param p_addr Pointer to sockaddr struct to be printed.
 * @param sock_len Size of the sockaddr struct to be printed.
 */
void sockaddr_fprint(FILE* const p_file, const struct sockaddr* const p_addr,
	const socklen_t sock_len);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_PADDR_H
