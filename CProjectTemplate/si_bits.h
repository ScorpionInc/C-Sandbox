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

bool is_host_order_le();

void fprint_le_byte_bits_4(FILE* p_file, const uint8_t byte,
	const unsigned int spacer_position, const char spacer)
void fprint_le_byte_bits_3(FILE* p_file, const uint8_t byte,
	const unsigned int spacer_position)
void fprint_le_byte_bits(FILE* p_file, const uint8_t byte)

void fprint_be_byte_bits_4(FILE* p_file, const uint8_t byte,
	const unsigned int spacer_position, const char spacer)
void fprint_be_byte_bits_3(FILE* p_file, const uint8_t byte,
	const unsigned int spacer_position)
void fprint_be_byte_bits(FILE* p_file, const uint8_t byte)

void fprint_bits_6(FILE* p_file, const uint8_t* p_buffer, const size_t count,
	const size_t grouping, const bool little_endian, const char spacer);
void fprint_bits_5(FILE* p_file, const uint8_t* p_buffer, const size_t count,
	const size_t grouping, const bool little_endian);
void fprint_bits_4(FILE* p_file, const uint8_t* p_buffer, const size_t count,
	const size_t grouping);
void fprint_bits(FILE* p_file, const uint8_t* p_buffer, const size_t count);

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
