/* si_bits.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Prototypes functions for bit fiddling
 * Created: 20250606
 * Updated: 20250606
//*/

#ifndef SI_BITS_H_
#define SI_BITS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define SI_BITS_COUNT 8u
#define SI_BITS_DEFAULT_SPACER ' '

// Prints a byte
void fprint_byte_bits_4(FILE* p_file, const uint8_t byte,
	const unsigned int bit_start, const size_t bit_length);
void fprint_byte_bits_3(FILE* p_file, const uint8_t byte,
	const unsigned int bit_start);
void fprint_byte_bits(FILE* p_file, const uint8_t byte);

// Determines host order
bool is_host_order_le();

// Prints full le bytes
void fprint_le_bytes_bits(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

// Prints full be bytes
void fprint_be_bytes_bits(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

// Prints full bytes
void fprint_bytes_bits(FILE* p_file, const uint8_t* p_buffer,
	const size_t buffer_size);

// Prints le bit blocks
void fprint_le_bits_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t bit_offset, const size_t bit_count);
void fprint_le_bits_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t bit_offset);
void fprint_le_bits(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

// Prints be bit blocks
void fprint_be_bits_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t bit_offset, const size_t bit_count);
void fprint_be_bits_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t bit_offset);
void fprint_be_bits(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

// Prints bit blocks
void fprint_bits(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t bit_offset, const size_t bit_count);

// Print full bytes grouped
void fprint_grouped_bits_5(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t grouping, const char spacer);
void fprint_grouped_bits_4(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size,
	const size_t grouping);
void fprint_grouped_bits(FILE* p_file,
	const uint8_t* p_buffer, const size_t buffer_size);

size_t bits_string_length(const size_t byte_count, const size_t grouping);

size_t snprint_bits_7(char* string, const size_t n,
	const uint8_t* p_buffer, const size_t count,
	const size_t grouping, const bool little_endian,
	const char spacer);
size_t snprint_bits_6(char* string, const size_t n,
	const uint8_t* p_buffer, const size_t count,
	const size_t grouping, const bool little_endian);
size_t snprint_bits_5(char* string, const size_t n,
	const uint8_t* p_buffer, const size_t count,
	const size_t grouping);
size_t snprint_bits(char* string, const size_t n,
	const uint8_t* p_buffer, const size_t count);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif //SI_BITS_H_
