#include <limits.h>
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
 * Updated: 20250610
//*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// Prints some or all of the bits in a single byte from MSB->LSB.
void fprint_byte_bits_4(FILE* p_file, const uint8_t byte,
			const unsigned int bit_start, const size_t bit_length)
{
	// Local Mutable Variables
	unsigned int mut_bit_start = bit_start;
	size_t mut_bit_length = bit_length;
	// Validate parameters
	if((NULL == p_file) || (SI_BITS_COUNT <= bit_start))
	{
		goto END;
	}
	// Clamp bit_start to prevent underflow in next if
	if(SI_BITS_COUNT < mut_bit_start)
	{
		mut_bit_start = SI_BITS_COUNT;
	}
	// Clamp bit_length to within a byte
	if((SI_BITS_COUNT - mut_bit_start) < mut_bit_length)
	{
		mut_bit_length = (SI_BITS_COUNT - mut_bit_start);
	}
	// Begin
	const unsigned int bit_end = (mut_bit_start + mut_bit_length);
	for(unsigned int bit_index = mut_bit_start;bit_index < bit_end;bit_index++)
	{
		// Prints from MSB
		fprintf(p_file, "%c", byte & (0x80 >> bit_index) ? '1' : '0');
	}
	// End
END:
	return;
}
inline void fprint_byte_bits_3(FILE* p_file, const uint8_t byte,
			const unsigned int bit_start)
{
	// Default bit_len is UINT_MAX (Prints all bits after bit_start)
	fprint_byte_bits_4(p_file, byte, bit_start, UINT_MAX);
}
inline void fprint_byte_bits(FILE* p_file, const uint8_t byte)
{
	// Default bit_start is 0u (Prints all bits)
	fprint_byte_bits_3(p_file, byte, 0u);
}

// Determines the ordering of the host at runtime.
bool is_host_order_le()
{
	int tmp = 1;
	char *ptr = (char *)&tmp;
	return (*ptr == 1);
}

// Writes size bytes from p_buffer in little endian to p_file as binary chars.
void fprint_le_bytes_bits(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size)
{
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == buffer_size))
	{
		goto END;
	}
	// Begin
	// Little Endian prints bytes in reverse order.
	for(size_t byte_offset = buffer_size - 1u; byte_offset >= 0u; byte_offset--)
	{
		fprint_byte_bits(p_file, p_buffer[byte_offset]);
		if(0u == byte_offset)
		{
			break;
		}
	}
	// End
END:
	return;
}

// Writes size bytes from p_buffer in big endian to p_file as binary chars.
void fprint_be_bytes_bits(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size)
{
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == buffer_size))
	{
		goto END;
	}
	// Begin
	// Big Endian prints bytes in order.
	for(size_t byte_offset = 0u; byte_offset < buffer_size; byte_offset++)
	{
		fprint_byte_bits(p_file, p_buffer[byte_offset]);
	}
	// End
END:
	return;
}

// Writes size bytes from p_buffer in host endian to p_file as binary chars.
void fprint_bytes_bits(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size)
{
	if(is_host_order_le())
	{
		fprint_le_bytes_bits(p_file, p_buffer, buffer_size);
	}
	else
	{
		fprint_be_bytes_bits(p_file, p_buffer, buffer_size);
	}
}

// Writes bit_count bits as 1/0 chars into p_file from p_buffer in little endian
void fprint_le_bits_5(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size,
			const size_t bit_offset, const size_t bit_count)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	// Local Variables
	size_t mut_bit_offset = bit_offset;
	size_t mut_bit_count = bit_count;
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == bit_count))
	{
		goto END;
	}
	if(max_bit < mut_bit_offset)
	{
		mut_bit_offset = max_bit;
	}
	if(max_bit < (mut_bit_count + mut_bit_offset))
	{
		mut_bit_count = (max_bit - mut_bit_offset);
	}
	// Local Constants
	const size_t beg_unord_index = (mut_bit_offset / SI_BITS_COUNT);
	const size_t end_unord_index = ((mut_bit_count + mut_bit_offset - 1u) / SI_BITS_COUNT);
	const size_t full_byte_count = (end_unord_index - beg_unord_index) - 1u;
	const size_t beg_ord_index = buffer_size - 1u - beg_unord_index;
	const size_t end_ord_index = buffer_size - 1u - end_unord_index;
	const size_t bit_offset_remainder = (mut_bit_offset % SI_BITS_COUNT);
	/* Debugging
	printf("bit_offset: %lu\tbit_count: %lu\n", bit_offset, bit_count);
	printf("beg_unord_index: %lu\tend_unord_index: %lu\n",
		beg_unord_index, end_unord_index);
	printf("full_byte_count: %lu\tbit_offset_remainder: %lu\n",
		full_byte_count, bit_offset_remainder);
	printf("beg_unord_index: %lu\tend_unord_index: %lu\n",
		beg_unord_index, end_unord_index);
	//*/
	// Begin
	if(beg_unord_index == end_unord_index)
	{
		// This is printing all or part of 1 byte.
		fprint_byte_bits_4(p_file, p_buffer[beg_ord_index],
			bit_offset_remainder, mut_bit_count);
		goto END;
	}
	fprint_byte_bits_4(p_file, p_buffer[beg_ord_index],
			bit_offset_remainder, SI_BITS_COUNT - bit_offset_remainder);
	fprint_le_bytes_bits(p_file, (&p_buffer[beg_unord_index + 1u]), full_byte_count);
	fprint_byte_bits_4(p_file, p_buffer[end_ord_index],
			0u, mut_bit_count - (SI_BITS_COUNT - bit_offset_remainder));
	// End
END:
	return;
}
inline void fprint_le_bits_4(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size, const size_t bit_offset)
{
	// Default bit_count is SIZE_MAX. (Prints all bits)
	fprint_le_bits_5(p_file, p_buffer, buffer_size, bit_offset, SIZE_MAX);
}
inline void fprint_le_bits(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size)
{
	// Default bit_offset is 0. (Prints from the start.)
	fprint_le_bits_4(p_file, p_buffer, buffer_size, 0u);
}

// Writes bit_count bits as 1/0 chars into p_file from p_buffer in big endian
void fprint_be_bits_5(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size,
			const size_t bit_offset, const size_t bit_count)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	// Local Variables
	size_t mut_bit_offset = bit_offset;
	size_t mut_bit_count = bit_count;
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == bit_count))
	{
		goto END;
	}
	if(max_bit < mut_bit_offset)
	{
		mut_bit_offset = max_bit;
	}
	if(max_bit < (mut_bit_count + mut_bit_offset))
	{
		mut_bit_count = (max_bit - mut_bit_offset);
	}
	// Local Constants
	const size_t beg_unord_index = (mut_bit_offset / SI_BITS_COUNT);
	const size_t end_unord_index = ((mut_bit_count + mut_bit_offset - 1u) / SI_BITS_COUNT);
	const size_t full_byte_count = (end_unord_index - beg_unord_index) - 1u;
	const size_t bit_offset_remainder = (mut_bit_offset % SI_BITS_COUNT);
	// Begin
	if(beg_unord_index == end_unord_index)
	{
		// This is printing all or part of 1 byte.
		fprint_byte_bits_4(p_file, p_buffer[beg_unord_index],
			bit_offset_remainder, mut_bit_count);
		goto END;
	}
	fprint_byte_bits_4(p_file, p_buffer[beg_unord_index],
			bit_offset_remainder, SI_BITS_COUNT - bit_offset_remainder);
	fprint_be_bytes_bits(p_file, (&p_buffer[beg_unord_index + 1u]), full_byte_count);
	fprint_byte_bits_4(p_file, p_buffer[end_unord_index],
			0u, mut_bit_count - (SI_BITS_COUNT - bit_offset_remainder));
	// End
END:
	return;
}
inline void fprint_be_bits_4(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size, const size_t bit_offset)
{
	// Default bit_count is SIZE_MAX. (Prints all bits)
	fprint_le_bits_5(p_file, p_buffer, buffer_size, bit_offset, SIZE_MAX);
}
inline void fprint_be_bits(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size)
{
	// Default bit_offset is 0. (Prints from the start.)
	fprint_le_bits_4(p_file, p_buffer, buffer_size, 0u);
}

// Writes bit_count bits as 1/0 chars into p_file from p_buffer in host order.
void fprint_bits(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t bit_offset, const size_t bit_count)
{
	if(is_host_order_le())
	{
		fprint_le_bits_5(p_file, p_buffer, buffer_size, bit_offset, bit_count);
	}
	else
	{
		fprint_be_bits_5(p_file, p_buffer, buffer_size, bit_offset, bit_count);
	}
}

// Writes bytes as bit chars from buffer in host order to file with spacers.
void fprint_grouped_bits_5(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t grouping, const char spacer)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	// Validate Parameters
	if((NULL == p_file) || (NULL == p_buffer))
	{
		goto END;
	}
	if((0u >= grouping) || (max_bit <= grouping))
	{
		fprint_bytes_bits(p_file, p_buffer, buffer_size);
		goto END;
	}
	const size_t print_count = (max_bit / grouping);
	// Begin
	for(size_t print_index = 0u; print_index < print_count; print_index++)
	{
		fprint_bits(p_file, p_buffer, buffer_size,
			grouping * print_index, grouping);
		if(print_index < (print_count - 1u))
		{
			fprintf(p_file, "%c", spacer);
		}
	}
	// End
END:
	return;
}
inline void fprint_grouped_bits_4(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t grouping)
{
	// Default spacer = ' '
	fprint_grouped_bits_5(p_file, p_buffer, buffer_size, grouping, SI_BITS_DEFAULT_SPACER);
}
inline void fprint_grouped_bits(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size)
{
	// Default group size is SI_BITS_COUNT (8)
	fprint_grouped_bits_4(p_file, p_buffer, buffer_size, SI_BITS_COUNT);
}

// TODO Finish Refactor Below

// Returns the length of a string representing the bits of a type with
// byte_count bytes seperated every grouping number of bits by a single spacer.
size_t bits_string_length(const size_t byte_count, const size_t grouping)
{
	size_t spacer_count =
			(grouping <= 0u) ?
			0u :
			(((SI_BITS_COUNT * byte_count) - 1u) / grouping);
	return (SI_BITS_COUNT * byte_count) + spacer_count;
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
	fprint_grouped_bits_5(stream, p_buffer, count, grouping, spacer);
	fclose(stream);
	return string_length;
}
inline size_t snprint_bits_6(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping, const bool little_endian)
{
	// Default Parameter spacer = ' '
	return snprint_bits_7(string, n, p_buffer,
			count, grouping, little_endian, SI_BITS_DEFAULT_SPACER);
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
			count, SI_BITS_COUNT);
}


#ifdef __cplusplus
}
#endif //__cplusplus
