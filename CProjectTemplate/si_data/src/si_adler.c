//si_adler.c

#include "si_adler.h"

// Selects prime number to use based on block size.
// For block sizes >= 9 bytes uses 128 block prime.
unsigned long long si_adler_select_prime(const size_t block_size)
{
	unsigned long long prime = ADLER_128_PRIME;
	if(2u >= block_size)
	{
		prime = ADLER_16_PRIME;
		goto END;
	}
	if(4u >= block_size)
	{
		prime = ADLER_32_PRIME;
		goto END;
	}
	if(8u >= block_size)
	{
		prime = ADLER_64_PRIME;
	}
	// End
END:
	return prime;
}

// Initializes block to the Adler prime value for it's size in le.
void si_adler_init_prime_le(uint8_t* const p_bytes, const size_t block_size)
{
	// Validate
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	const size_t prime_size = block_size / 2u;
	const unsigned long long prime = si_adler_select_prime(block_size);
	for(size_t i = 0; i < sizeof(long long); i++)
	{
		if(i >= prime_size)
		{
			break;
		}
		p_bytes[i] = ((uint8_t*)&prime)[i];
	}
	// End
END:
	return;
}
// " in be.
void si_adler_init_prime_be(uint8_t* const p_bytes, const size_t block_size)
{
	// Validate
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	const size_t prime_size = block_size / 2u;
	const unsigned long long prime = si_adler_select_prime(block_size);
	for(size_t i = 0; i < sizeof(long long); i++)
	{
		if(i >= prime_size)
		{
			break;
		}
		const size_t next_index = prime_size - 1u - i;
		p_bytes[next_index] = ((uint8_t*)&prime)[next_index];
	}
	// End
END:
	return;
}
// " in host order.
void si_adler_init_prime(uint8_t* const p_bytes, const size_t block_size)
{
	// Validate
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		si_adler_init_prime_le(p_bytes, block_size);
	}
	else
	{
		si_adler_init_prime_be(p_bytes, block_size);
	}
	// End
END:
	return;
}

void si_adler_init(si_adler_t* const p_hash)
{
	// Validate parameter
	if(NULL == p_hash)
	{
		goto END;
	}
	// Zeroize the Hash Buffer
	memset(p_hash->hash, 0x00, p_hash->block_size);
	// Seed hash buffer
	increment_bytes(p_hash->hash, p_hash->block_size);
	// End
END:
	return;
}

void si_adler_new(si_adler_t* const p_hash, const size_t block_size)
{
	// Validate parameter
	if(NULL == p_hash)
	{
		goto END;
	}
	// Begin
	p_hash->block_size = block_size;
	p_hash->hash = calloc(sizeof(uint8_t), p_hash->block_size);
	if(NULL == p_hash->hash)
	{
		goto END;
	}
	si_adler_init(p_hash);
	// End
END:
	return;
}

// Adler Varient for N-Bits
void si_adler_update(si_adler_t* const p_hash, const uint8_t* const p_buffer,
	const size_t input_buffer_size)
{
	// Validate parameters
	if((NULL == p_hash) || (NULL == p_buffer))
	{
		goto END;
	}
	const size_t half_bytes = p_hash->block_size / 2u;
	const size_t buffer_size = half_bytes + 1u;
	{
		// Declare some block_size buffers
		uint8_t prime[buffer_size];
		uint8_t input_buffer[buffer_size];
		uint8_t lsb_buffer[buffer_size];
		uint8_t msb_buffer[buffer_size];
		// Initialize prime number for block size.
		memset(&prime[0], 0x00, buffer_size);
		si_adler_init_prime(&prime[0u], p_hash->block_size);
		// Digest fixed block size input
		for(size_t i = 0u; i < input_buffer_size; i++)
		{
			// Buffer = LSB with +1 pad
			memset(&lsb_buffer[0u], 0x00, buffer_size);
			memcpy(&lsb_buffer[0u], &p_hash->hash[0u], half_bytes);
			// Buffer = MSB with +1 pad
			memset(&msb_buffer[0u], 0x00, buffer_size);
			memcpy(&msb_buffer[0u], &p_hash->hash[half_bytes], half_bytes);
			// Buffer input byte +2 pad
			memset(&input_buffer[0u], 0x00, buffer_size);
			if(BYTE_ORDER == LITTLE_ENDIAN)
			{
				input_buffer[0u] = p_buffer[i];
			}
			else
			{
				input_buffer[half_bytes - 1u] = p_buffer[i];
			}
			// Add. lsb = lsb + input_buffer
			add_uint_bytes(&lsb_buffer[0u], &input_buffer[0u], buffer_size);
			// Mod. lsb = lsb % PRIME
			mod_uint_bytes(&lsb_buffer[0u], &prime[0], buffer_size);
			// Add. msb = msb + lsb
			add_uint_bytes(&msb_buffer[0u], &lsb_buffer[0], buffer_size);
			// Mod. msb = msb % PRIME
			mod_uint_bytes(&msb_buffer[0u], &prime[0], buffer_size);
			// Finished LSB & MSB apply to hash
			memcpy(&p_hash->hash[0u], &lsb_buffer[0u], half_bytes);
			memcpy(&p_hash->hash[half_bytes], &msb_buffer[0u], half_bytes);
		}
	}
	// End
END:
	return;
}

void si_adler_free(si_adler_t* const p_hash)
{
	// Validate parameter
	if(NULL == p_hash)
	{
		goto END;
	}
	// Begin
	if(NULL != p_hash->hash)
	{
		free(p_hash->hash);
		p_hash->hash = NULL;
	}
	p_hash->block_size = 0u;
	// End
END:
	return;
}

void si_adler_fprint(const si_adler_t* const p_hash, FILE* const p_file)
{
	// Validate parameters
	if((NULL == p_hash) || (NULL == p_file))
	{
		goto END;
	}
	if((NULL == p_hash->hash) || (0u >= p_hash->block_size))
	{
		goto END;
	}
	// Begin
	fprintf(p_file, "0x");
	for(size_t i = 0u; i < p_hash->block_size; i++)
	{
		size_t next_index = i;
		if(BYTE_ORDER == LITTLE_ENDIAN)
		{
			next_index = p_hash->block_size - 1u - next_index;
		}
		fprintf(p_file, "%02X", p_hash->hash[next_index]);
	}
	// End
END:
	return;
}
