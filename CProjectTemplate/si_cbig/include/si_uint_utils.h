/* si_big_uint.h
 * Language: C
 * Purpose: Defines functions for managing a byte buffer as a big unsigned int.
 * Created: 20150626
 * Updated: 20250626
//*/

#include "si_endian.h" // BYTE_ORDER, LITTLE_ENDIAN

#include <limits.h>
#include <stdbool.h> // bool, false, true
#include <stdint.h> // uint8_t
#include <stdio.h>
#include <string.h>

#ifndef SI_BIG_UINT_H
#define SI_BIG_UINT_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/* Doxygen
 * @brief Reverses the order of bytes in an array.
 *
 * @param p_bytes Pointer to byte array to be reversed.
 * @param size Size of the array located at the pointer's address.
 */
void reverse_bytes(uint8_t* const p_bytes, const size_t size);

/* Doxygen
 * @brief Inverts(Not) the bits of all bytes in a byte array.
 *
 * @param p_bytes Pointer to byte array to be inverted.
 * @param size Number of bytes in the byte array to be inverted.
 */
void invert_uint_bytes(uint8_t* const p_bytes, const size_t size);

/* Doxygen
 * @brief Shifts bits in byte array left once. Ignores Endianess.
 *
 * @param p_bytes Pointer to byte array to be shifted.
 * @param size Number of byte elements in the byte array being shifted.
 */
void lshift_bytes(uint8_t* const p_bytes, const size_t size);
/* Doxygen
 * @brief Shifts bits in byte array left amount times. Ignores endianess.
 *
 * @param p_bytes Pointer to array of bytes to be shifted.
 * @param size Number of elements(bytes) in the byte array.
 * @param amount Number of times bits are to be shifted in the array.
 */
void lshift_bytes_by(uint8_t* const p_bytes, const size_t size,
	const size_t amount);

/* Doxygen
 * @brief Shifts bits in byte array right once. Ignores Endianess.
 *
 * @param p_bytes Pointer to byte array to be shifted.
 * @param size Number of byte elements in the byte array being shifted.
 */
void rshift_bytes(uint8_t* const p_bytes, const size_t size);
/* Doxygen
 * @brief Shifts bits in byte array right amount times. Ignores endianess.
 *
 * @param p_bytes Pointer to array of bytes to be shifted.
 * @param size Number of elements(bytes) in the byte array.
 * @param amount Number of times bits are to be shifted in the array.
 */
void rshift_bytes_by(uint8_t* const p_bytes, const size_t size,
	const size_t amount);

/* Doxygen
 * @brief Adds two byte arrays of size as unsigned int in little endian order.
 *
 * @param p_a Pointer to first byte array which is changed to the result.
 * @param p_b Pointer to const second byte array to be added to p_a.
 * @param size Number of bytes in both arrays. (Must be of the same size).
 *
 * @return Returns the carry/overflow flag as a bool. Results are stored in p_a
 */
bool add_uint_le_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);
/* Doxygen
 * @brief Adds two byte arrays of size as unsigned int in big endian order.
 *
 * @param p_a Pointer to first byte array which is changed to the result.
 * @param p_b Pointer to const second byte array to be added to p_a.
 * @param size Number of bytes in both arrays. (Must be of the same size).
 *
 * @return Returns the carry/overflow flag as a bool. Results are stored in p_a
 */
bool add_uint_be_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);
/* Doxygen
 * @brief Adds two byte arrays of size as unsigned int in host endian order.
 *
 * @param p_a Pointer to first byte array which is changed to the result.
 * @param p_b Pointer to const second byte array to be added to p_a.
 * @param size Number of bytes in both arrays. (Must be of the same size).
 *
 * @return Returns the carry/overflow flag as a bool. Results are stored in p_a
 */
bool add_uint_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Adds one to the byte array p_bytes in little endian order.
 *
 * @param p_bytes Pointer to byte array.
 * @param size Number of bytes in the array.
 */
void increment_le_bytes(uint8_t* const p_bytes, const size_t size);

/* Doxygen
 * @brief Adds one to the byte array p_bytes in big endian order.
 *
 * @param p_bytes Pointer to byte array.
 * @param size Number of bytes in the array.
 */
void increment_be_bytes(uint8_t* const p_bytes, const size_t size);

/* Doxygen
 * @brief Adds one to the byte array p_bytes in host endian order.
 *
 * @param p_bytes Pointer to byte array.
 * @param size Number of bytes in the array.
 */
void increment_bytes(uint8_t* const p_bytes, const size_t size);

/* Doxygen
 * @brief Subtracts p_b from byte array p_a. Little endian with 2's compliment.
 *
 * @param p_a Pointer to array to be subtracted from.
 * @param p_b Pointer to array to be subtracted by.
 * @param size Number of bytes allocated in each array.
 *
 * @return Returns the underflow flag as a std bool. Results are in p_a.
 */
bool sub_uint_le_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Subtracts p_b from byte array p_a. Big endian with 2's compliment.
 *
 * @param p_a Pointer to array to be subtracted from.
 * @param p_b Pointer to array to be subtracted by.
 * @param size Number of bytes allocated in each array.
 *
 * @return Returns the underflow flag as a std bool. Results are in p_a.
 */
bool sub_uint_be_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Subtracts p_b from byte array p_a. Host endian with 2's compliment.
 *
 * @param p_a Pointer to array to be subtracted from.
 * @param p_b Pointer to array to be subtracted by.
 * @param size Number of bytes allocated in each array.
 *
 * @return Returns the underflow flag as a std bool. Results are in p_a.
 */
bool sub_uint_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Compares byte values of byte array from MSB to LSB in Little Endian.
 *
 * @param p_a Pointer to first byte array to compare.
 * @param p_b Pointer to second byte array to compare.
 * @param size Size of buffers in bytes.
 *
 * @return Returns int 0 if equal or returns 1 if p_a > p_b or -1 if <
 */
int cmp_uint_le_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Compares byte values of byte array from MSB to LSB in Big Endian.
 *
 * @param p_a Pointer to first byte array to compare.
 * @param p_b Pointer to second byte array to compare.
 * @param size Size of buffers in bytes.
 *
 * @return Returns int 0 if equal or returns 1 if p_a > p_b or -1 if <
 */
int cmp_uint_be_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Compares byte values of byte array from MSB to LSB in Host order.
 *
 * @param p_a Pointer to first byte array to compare.
 * @param p_b Pointer to second byte array to compare.
 * @param size Size of buffers in bytes.
 *
 * @return Returns int 0 if equal or returns 1 if p_a > p_b or -1 if <
 */
int cmp_uint_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size);

/* Doxygen
 * @brief Division of little endian byte arrays p_dividend /= p_divisor.
 *
 * @param p_dividend Pointer to dividend byte array. Stores quotient result.
 * @param p_divisor Pointer to const divisor byte array.
 * @param size Number of bytes in both arrays (must be same size).
 * @param p_remainder Optional byte array to store division remainder.
 */
void div_uint_le_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size,
	uint8_t* const p_remainder);

/* Doxygen
 * @brief Division of big endian byte arrays p_dividend /= p_divisor.
 *
 * @param p_dividend Pointer to dividend byte array. Stores quotient result.
 * @param p_divisor Pointer to const divisor byte array.
 * @param size Number of bytes in both arrays (must be same size).
 * @param p_remainder Optional byte array to store division remainder.
 */
void div_uint_be_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size,
	uint8_t* const p_remainder);

/* Doxygen
 * @brief Division of host endian byte arrays p_dividend /= p_divisor.
 *
 * @param p_dividend Pointer to dividend byte array. Stores quotient result.
 * @param p_divisor Pointer to const divisor byte array.
 * @param size Number of bytes in both arrays (must be same size).
 * @param p_remainder Optional byte array to store division remainder.
 */
void div_uint_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size,
	uint8_t* const p_remainder);

/* Doxygen
 * @brief Returns remainder of p_dividend / p_divisor as p_dividend le bytes.
 *
 * @param p_dividend Pointer to byte array to be divided & store the remainder.
 * @param p_divisor Pointer to byte array to be divided by.
 * @param size Number of bytes in both arrays (Must be the same).
 */
void mod_uint_le_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size);

/* Doxygen
 * @brief Returns remainder of p_dividend / p_divisor as p_dividend be bytes.
 *
 * @param p_dividend Pointer to byte array to be divided & store the remainder.
 * @param p_divisor Pointer to byte array to be divided by.
 * @param size Number of bytes in both arrays (Must be the same).
 */
void mod_uint_be_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size);

/* Doxygen
 * @brief Returns remainder of p_dividend / p_divisor as p_dividend ho bytes.
 *
 * @param p_dividend Pointer to byte array to be divided & store the remainder.
 * @param p_divisor Pointer to byte array to be divided by.
 * @param size Number of bytes in both arrays (Must be the same).
 */
void mod_uint_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_BIG_UINT_H
