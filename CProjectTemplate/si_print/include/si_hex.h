/* si_hex.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Prototypes functions for hex printing
 * Created: 20250623
 * Updated: 20250623
//*/

#ifndef SI_HEX_H_
#define SI_HEX_H_

#include <endian.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define SI_BITS_COUNT 8u
#define SI_NIBBLE_COUNT 2u
#define SI_NIBBLE_BITS 4u
#define SI_HEX_DEFAULT_SPACER ' '

/** Doxygen
 * @brief Prints MSB/LSB or all of nibbles in a single byte from MSB->LSB.
 *
 * @param p_file FILE pointer to be written to.
 * @param byte Source of nibbles to be read from.
 * @param nibble_start which bit index to start printing from.
 * @param nibble_length How many bits to print. Caps at SI_BITS_COUNT.
 *
 * @return Doesn't return a value but fails silently.
 */
void fprint_byte_hex_4(FILE* p_file, const uint8_t byte,
	const unsigned int bit_start, const size_t bit_length);
void fprint_byte_hex_3(FILE* p_file, const uint8_t byte,
	const unsigned int bit_start);
void fprint_byte_hex(FILE* p_file, const uint8_t byte);

/** Doxygen
 * @brief Writes size bytes from p_buffer in little endian to p_file as
 *        binary chars.
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_le_bytes_hex(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

/** Doxygen
 * @brief Writes size bytes from p_buffer in big endian to p_file as
 *        binary chars.
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_be_bytes_hex(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

/** Doxygen
 * @brief Writes size bytes from p_buffer in host endian to p_file as
 *        binary chars.
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_bytes_hex(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

/** Doxygen
 * @brief Writes bit_count bits as 1/0 chars into p_file from p_buffer in
 *        little endian
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 * @param nibble_offset Specifies the start of the bits to be printed.
 * @param nibble_count Specifies the number of bits to be printed.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_le_hex_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t nibble_offset, const size_t nibble_count);
void fprint_le_hex_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t nibble_offset);
void fprint_le_hex(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

/** Doxygen
 * @brief Writes hex_count glyphs as chars into p_file from p_buffer in
 *        big endian
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read nibbles.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 * @param nibble_offset Specifies the start of the bits to be printed.
 * @param nibble_count Specifies the number of bits to be printed.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_be_hex_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t nibble_offset, const size_t nibble_count);
void fprint_be_hex_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t nibble_offset);
void fprint_be_hex(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

/** Doxygen
 * @brief Writes bit_count bits as 1/0 chars into p_file from p_buffer in
 *        host order.
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 * @param bit_offset Specifies the start of the bits to be printed.
 * @param bit_count Specifies the number of bits to be printed.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_hex(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t nibble_offset, const size_t nibble_count);

/** Doxygen
 * @brief Writes bytes as hex chars from buffer in host order to file with
 *        spacers.
 *
 * @param p_file FILE pointer to be written to.
 * @param p_buffer Pointer to data buffer of size capacity to read bytes.
 * @param buffer_size Specifies the size of the byte buffer at p_buffer.
 * @param grouping Specifies the group size of bits before printing a spacer.
 * @param spacer Specifies the spacer character to be printed.
 *
 * @return Doesn't return a value, but fails silently on error.
 */
void fprint_grouped_hex_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t grouping, const char spacer);
void fprint_grouped_hex_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t grouping);
void fprint_grouped_hex(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

/** Doxygen
 * @brief Returns the length of a grouped hex string in characters.
 *
 * @param byte_count Number of bytes being printed.
 * @param grouping Number of bits to print before adding a spacer.
 */
size_t hex_string_length(const size_t byte_count, const size_t grouping);

/** Doxygen
 * @brief Writes value of buffer as bit chars into a string buffer.
 *
 * @param string buffer to store resulting bit string into.
 * @param string_size Size of the string buffer being written to.
 */
size_t snprint_hex_6(char* string, const size_t string_size,
	const uint8_t* p_buffer, const size_t count,
	const size_t grouping, const char spacer);
size_t snprint_hex_5(char* string, const size_t string_size,
	const uint8_t* p_buffer, const size_t count,
	const size_t grouping);
size_t snprint_hex(char* string, const size_t string_size,
	const uint8_t* p_buffer, const size_t count);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_HEX_H_
