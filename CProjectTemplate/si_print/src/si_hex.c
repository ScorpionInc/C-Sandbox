//si_hex.c

#include "si_hex.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// Prints 0-2 nibbles in a single byte from MSB->LSB.
void fprint_byte_hex_4(FILE* p_file, const uint8_t byte,
			const unsigned int bit_start, const size_t nibble_count)
{
	// Local Mutable Variables
	size_t mut_nibble_count = nibble_count;
	// Local Constant
	const size_t max_bit_index = (SI_BITS_COUNT - SI_NIBBLE_BITS);
	const size_t max_nibble_count = ((SI_BITS_COUNT - bit_start) / SI_NIBBLE_BITS);
	// Validate parameters
	if((NULL == p_file) || (max_bit_index < bit_start))
	{
		// Nothing to print
		goto END;
	}
	// Clamp nibble_count to within the byte
	if(max_nibble_count < mut_nibble_count)
	{
		mut_nibble_count = max_nibble_count;
	}
	// Begin
	for(unsigned int nibble_index = 0u; nibble_index < mut_nibble_count;
		nibble_index++)
	{
		// Prints from MSB
		const unsigned int mask = (
			0xF0 >> ((SI_NIBBLE_BITS * nibble_index) + bit_start)
		);
		const unsigned int shift = (
			(SI_NIBBLE_BITS * (mut_nibble_count - 1u - nibble_index)) + bit_start
		);
		fprintf(p_file, "%01x", (byte & mask) >> shift);
	}
	// End
END:
	return;
}
inline void fprint_byte_hex_3(FILE* p_file, const uint8_t byte,
			const unsigned int bit_start)
{
	// Default bit_len is UINT_MAX (Prints all nibbles after bit_start)
	fprint_byte_hex_4(p_file, byte, bit_start, UINT_MAX);
}
inline void fprint_byte_hex(FILE* p_file, const uint8_t byte)
{
	// Default bit_start is 0u (Prints all nibbles)
	fprint_byte_hex_3(p_file, byte, 0u);
}

// Writes size bytes from p_buffer in little endian to p_file as binary chars.
void fprint_le_bytes_hex(FILE* p_file,
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
		fprint_byte_hex(p_file, p_buffer[byte_offset]);
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
void fprint_be_bytes_hex(FILE* p_file,
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
		fprint_byte_hex(p_file, p_buffer[byte_offset]);
	}
	// End
END:
	return;
}

// Writes size bytes from p_buffer in host endian to p_file as binary chars.
void fprint_bytes_hex(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size)
{
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		fprint_le_bytes_hex(p_file, p_buffer, buffer_size);
	}
	else
	{
		fprint_be_bytes_hex(p_file, p_buffer, buffer_size);
	}
}

// Writes nibble_count nibbles as chars into p_file from p_buffer in little endian
void fprint_le_hex_5(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size,
			const size_t bit_offset, const size_t nibble_count)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	// Local Variables
	size_t mut_bit_offset = bit_offset;
	size_t mut_nibble_count = nibble_count;
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == nibble_count))
	{
		goto END;
	}
	if(max_bit < mut_bit_offset)
	{
		mut_bit_offset = max_bit;
	}
	if(max_bit < ((mut_nibble_count * SI_NIBBLE_BITS) + mut_bit_offset))
	{
		mut_nibble_count = (max_bit - mut_bit_offset) / SI_NIBBLE_BITS;
	}
	// Local Constants
	const size_t beg_unord_index = (mut_bit_offset / SI_BITS_COUNT);
	const size_t end_unord_index = (
		((mut_nibble_count * SI_NIBBLE_BITS) + mut_bit_offset - 1u)
		/ SI_BITS_COUNT
	);
	const size_t full_byte_count = (end_unord_index - beg_unord_index) - 1u;
	const size_t beg_ord_index = buffer_size - 1u - beg_unord_index;
	const size_t end_ord_index = buffer_size - 1u - end_unord_index;
	const size_t bit_offset_remainder = (mut_bit_offset % SI_BITS_COUNT);
	// Begin
	if(beg_unord_index == end_unord_index)
	{
		// This is printing all or part of 1 byte.
		fprint_byte_hex_4(p_file, p_buffer[beg_ord_index],
			bit_offset_remainder, mut_nibble_count);
		goto END;
	}
	fprint_byte_hex_4(p_file, p_buffer[beg_ord_index],
		bit_offset_remainder, SI_NIBBLE_COUNT - bit_offset_remainder);
	fprint_le_bytes_hex(p_file, (&p_buffer[beg_unord_index + 1u]),
		full_byte_count);
	fprint_byte_hex_4(p_file, p_buffer[end_ord_index],
		0u, mut_nibble_count - (SI_NIBBLE_COUNT - bit_offset_remainder));
	// End
END:
	return;
}
inline void fprint_le_hex_4(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size, const size_t nibble_offset)
{
	// Default nibble_count is SIZE_MAX. (Prints all nibbles)
	fprint_le_hex_5(p_file, p_buffer, buffer_size, nibble_offset, SIZE_MAX);
}
inline void fprint_le_hex(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size)
{
	// Default nibble_offset is 0. (Prints from the start.)
	fprint_le_hex_4(p_file, p_buffer, buffer_size, 0u);
}

// Writes nibble_count nibbles as chars into p_file from p_buffer in big endian
void fprint_be_hex_5(FILE* p_file,
			const uint8_t* p_buffer, const size_t buffer_size,
			const size_t bit_offset, const size_t nibble_count)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	// Local Variables
	size_t mut_bit_offset = bit_offset;
	size_t mut_nibble_count = nibble_count;
	// Validate parameters
	if((NULL == p_file) || (NULL == p_buffer) || (0u == nibble_count))
	{
		goto END;
	}
	if(max_bit < mut_bit_offset)
	{
		mut_bit_offset = max_bit;
	}
	if(max_bit < ((mut_nibble_count * SI_NIBBLE_BITS) + mut_bit_offset))
	{
		mut_nibble_count = (max_bit - mut_bit_offset) / SI_NIBBLE_BITS;
	}
	// Local Constants
	const size_t beg_unord_index = (mut_bit_offset / SI_BITS_COUNT);
	const size_t end_unord_index = (
		((mut_nibble_count * SI_NIBBLE_BITS) + mut_bit_offset - 1u)
		/ SI_BITS_COUNT
	);
	const size_t full_byte_count = (end_unord_index - beg_unord_index) - 1u;
	const size_t bit_offset_remainder = (mut_bit_offset % SI_BITS_COUNT);
	// Begin
	if(beg_unord_index == end_unord_index)
	{
		// This is printing all or part of 1 byte.
		fprint_byte_hex_4(p_file, p_buffer[beg_unord_index],
			bit_offset_remainder, mut_nibble_count);
		goto END;
	}
	fprint_byte_hex_4(p_file, p_buffer[beg_unord_index],
		bit_offset_remainder, SI_NIBBLE_COUNT - bit_offset_remainder);
	fprint_be_bytes_hex(p_file, (&p_buffer[beg_unord_index + 1u]),
		full_byte_count);
	fprint_byte_hex_4(p_file, p_buffer[end_unord_index],
		0u, mut_nibble_count - (SI_NIBBLE_COUNT - bit_offset_remainder));
	// End
END:
	return;
}
inline void fprint_be_hex_4(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size, const size_t nibble_offset)
{
	// Default nibble_count is SIZE_MAX. (Prints all nibbles)
	fprint_be_hex_5(p_file, p_buffer, buffer_size, nibble_offset, SIZE_MAX);
}
inline void fprint_be_hex(FILE* p_file, const uint8_t* p_buffer,
			const size_t buffer_size)
{
	// Default nibble_offset is 0. (Prints from the start.)
	fprint_be_hex_4(p_file, p_buffer, buffer_size, 0u);
}

// Writes nibble_count nibbles as chars into p_file from p_buffer in host order.
void fprint_hex(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t nibble_offset, const size_t nibble_count)
{
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		fprint_le_hex_5(p_file, p_buffer, buffer_size, nibble_offset, nibble_count);
	}
	else
	{
		fprint_be_hex_5(p_file, p_buffer, buffer_size, nibble_offset, nibble_count);
	}
}

// Writes bytes as hex chars from buffer in host order to file with spacers.
void fprint_grouped_hex_5(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t grouping, const char spacer)
{
	const size_t max_bit = (buffer_size * SI_BITS_COUNT);
	const size_t max_nibble = (max_bit / SI_NIBBLE_BITS);
	// Validate Parameters
	if((NULL == p_file) || (NULL == p_buffer))
	{
		goto END;
	}
	if((0u >= grouping) || (max_bit <= grouping))
	{
		// Output isn't grouped just print
		fprint_bytes_hex(p_file, p_buffer, buffer_size);
		goto END;
	}
	const size_t full_print_count = (max_nibble / grouping);
	// Begin
	for(size_t print_index = 0u; print_index < full_print_count; print_index++)
	{
		fprint_hex(p_file, p_buffer, buffer_size,
			SI_NIBBLE_BITS * grouping * print_index, grouping);
		const bool has_more_iterations = print_index < (full_print_count - 1u);
		if(has_more_iterations)
		{
			fprintf(p_file, "%c", spacer);
		}
	}
	// End
END:
	return;
}
inline void fprint_grouped_hex_4(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size,
    const size_t grouping)
{
	// Default spacer = ' '
	fprint_grouped_hex_5(p_file, p_buffer, buffer_size, grouping, SI_HEX_DEFAULT_SPACER);
}
inline void fprint_grouped_hex(FILE* p_file,
    const uint8_t* p_buffer, const size_t buffer_size)
{
	// Default group size is SI_NIBBLE_COUNT (8)
	fprint_grouped_hex_4(p_file, p_buffer, buffer_size, SI_NIBBLE_COUNT);
}

// Returns the length of a string representing the hex chars of a type with
// byte_count bytes seperated every grouping number of chars by a single spacer.
size_t nibbles_string_length(const size_t byte_count, const size_t grouping)
{
	size_t spacer_count = 0u;
	if(grouping > 0u)
	{
		spacer_count = (((SI_NIBBLE_COUNT * byte_count) - 1u) / grouping);
	}
	return (SI_NIBBLE_COUNT * byte_count) + spacer_count;
}

// Writes bit string value to string parameter via fmemopen().
// Returns number of characters written on success.
// Returns 0u on failure/error.
size_t snprint_hex_6(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping, const char spacer)
{
	size_t written = 0u;
	// Validate Parameters
	if((0u == n) || (0u == count))
	{
		goto END;
	}
	const size_t string_length = nibbles_string_length(count, grouping);
	if(n < string_length)
	{
		// Would write past the length of the buffer.
		goto END;
	}
	FILE* p_stream = fmemopen(string, n, "r+");
	if(NULL == p_stream)
	{
		// Failed to open buffer as file.
		goto END;
	}
	fprint_grouped_hex_5(p_stream, p_buffer, count, grouping, spacer);
	written = string_length;
	fclose(p_stream);
END:
	return written;
}
inline size_t snprint_hex_5(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count,
				const size_t grouping)
{
	// Default Parameter spacer = ' '
	return snprint_hex_6(string, n, p_buffer,
			count, grouping, SI_HEX_DEFAULT_SPACER);
}
inline size_t snprint_hex(char* string, const size_t n,
				const uint8_t* p_buffer, const size_t count)
{
	// Default Parameter spacer = SI_NIBBLE_COUNT (2u)
	return snprint_hex_5(string, n, p_buffer,
			count, SI_NIBBLE_COUNT);
}


#ifdef __cplusplus
}
#endif //__cplusplus
