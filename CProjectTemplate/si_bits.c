#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "si_bits.h"

/* si_bits.c
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Bit fiddling functions.
 * Created: 20250606
 * Updated: 20250606
//*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


bool is_host_order_le()
{
	int tmp = 1;
	char *ptr = (char *)&tmp;
	return (*ptr == 1);
}

void fprint_bits_6(FILE* p_file, const uint8_t* p_buffer, const size_t count,
			const size_t grouping, const bool little_endian, const char spacer)
{
	// Validate Parameters
	if(p_file == NULL || p_buffer == NULL)
		return;
	// Local Constant
	const char spacer_string[2] = { spacer, '\0' };
	// Start
	if(little_endian)
	{
		// Little Endianness
		for(size_t byte_index = 0u; byte_index < count; byte_index++)
		{
			uint8_t next_byte = p_buffer[count - 1u - byte_index];
			if(grouping > 0u)
				for(size_t bit_index = 0u; bit_index < 8u; bit_index++)
				{
					const bool print_spacer =
						(bit_index + byte_index * 8u) % grouping == grouping - 1u;
					const bool not_at_end = byte_index < (count - 1u) || bit_index < 7u;
					fprintf(p_file, "%c%s",
						next_byte & (0x80 >> bit_index) ? '1' : '0',
						print_spacer && not_at_end ? spacer_string : "");
				}
			else
				for(size_t bit_index = 0u; bit_index < 8u; bit_index++)
				{
					fprintf(p_file, "%c",
					next_byte & (0x80 >> bit_index) ? '1' : '0');
				}
		}
	}
	else
	{
		// Big Endianness
		for(size_t byte_index = 0u; byte_index < count; byte_index++)
		{
			uint8_t next_byte = p_buffer[byte_index];
			if(grouping > 0u)
				for(size_t bit_index = 0u; bit_index < 8u; bit_index++)
				{
					const bool print_spacer =
						(bit_index + byte_index * 8u) % grouping == grouping - 1u;
					const bool not_at_end = byte_index < (count - 1u) || bit_index < 7u;
					fprintf(p_file, "%c%s",
						next_byte & (0x01 << bit_index) ? '1' : '0',
						print_spacer && not_at_end ? spacer_string : "");
				}
			else
				for(size_t bit_index = 0u; bit_index < 8u; bit_index++)
					fprintf(p_file, "%c",
					next_byte & (0x01 << bit_index) ? '1' : '0');
		}
	}
}
inline void fprint_bits_5(FILE* file, const uint8_t* p_buffer,
				const size_t count, const size_t grouping,
				const bool little_endian)
{
	// Default parameter spacing = ' '
	fprint_bits_6(file, p_buffer, count, grouping, little_endian, ' ');
}
inline void fprint_bits_4(FILE* file, const uint8_t* p_buffer,
				const size_t count, const size_t grouping)
{
	// Default parameter little_endian = is_host_order_le()
	fprint_bits_5(file, p_buffer, count, grouping, is_host_order_le());
}
inline void fprint_bits(FILE* file, const uint8_t* p_buffer,
				const size_t count)
{
	// Default parameter grouping = 8u
	fprint_bits_4(file, p_buffer, count, 8u);
}

// Returns the length of a string representing the bits of a type with
// byte_count bytes seperated every grouping number of bits by a single space.
size_t bits_string_length(const size_t byte_count, const size_t grouping)
{
	size_t spacer_count =
			(grouping <= 0u) ?
			0u :
			(((8u * byte_count) - 1u) / grouping);
	return (8u * byte_count) + spacer_count;
}

// Writes bit string value to string parameter via fmemopen().
// Returns number of characters written on success.
// Returns 0u on failure/error.
size_t snprint_bits_7(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping, const bool little_endian,
				const char spacer)
{
	// Validate Parameters
	if(n == 0u || count == 0u)
		return 0u;
	const size_t string_length = bits_string_length(count, grouping);
	if(n < string_length)
		// Would write past the length of the buffer.
		return 0u;
	FILE* stream = fmemopen(string, n, "r+");
	if(stream == NULL)
		return 0u;
	fprint_bits_6(stream, p_buffer, count, grouping, little_endian, spacer);
	fclose(stream);
	return string_length;
}
inline size_t snprint_bits_6(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping, const bool little_endian)
{
	// Default Parameter spacer = ' '
	return snprint_bits_7(string, n, p_buffer,
			count, grouping, little_endian, ' ');
}
inline size_t snprint_bits_5(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping)
{
	// Default Parameter little_endian = is_host_order_le()
	return snprint_bits_6(string, n, p_buffer,
			count, grouping, is_host_order_le());
}
inline size_t snprint_bits(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count)
{
	// Default Parameter spacer = 8u
	return snprint_bits_5(string, n, p_buffer,
			count, 8u);
}


#ifdef __cplusplus
}
#endif //__cplusplus
