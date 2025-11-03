// si_adler.c
#include "si_adler.h"

// Selects prime number to use based on block size.
// For block sizes >= 9 bytes uses 128 block prime.
unsigned long long si_adler_select_prime(const size_t block_size)
{
	unsigned long long prime = ADLER_128_PRIME;
	if (2u >= block_size)
	{
		prime = ADLER_16_PRIME;
		goto END;
	}
	if (4u >= block_size)
	{
		prime = ADLER_32_PRIME;
		goto END;
	}
	if (8u >= block_size)
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
	if (NULL == p_bytes)
	{
		goto END;
	}
	const size_t prime_size = block_size / 2u;
	const unsigned long long prime = si_adler_select_prime(block_size);
	for (size_t iii = 0; iii < sizeof(long long); iii++)
	{
		if (iii >= prime_size)
		{
			break;
		}
		p_bytes[iii] = ((uint8_t*)&prime)[iii];
	}
END:
	return;
}
// Same as init_prime_le above but in big endian.
void si_adler_init_prime_be(uint8_t* const p_bytes, const size_t block_size)
{
	if (NULL == p_bytes)
	{
		goto END;
	}
	const size_t prime_size = block_size / 2u;
	const unsigned long long prime = si_adler_select_prime(block_size);
	for (size_t iii = 0; iii < sizeof(long long); iii++)
	{
		if (iii >= prime_size)
		{
			break;
		}
		const size_t next_index = prime_size - 1u - iii;
		p_bytes[next_index] = ((uint8_t*)&prime)[next_index];
	}
END:
	return;
}
// Same as init_prime_le above but in host order.
void si_adler_init_prime(uint8_t* const p_bytes, const size_t block_size)
{
	if (NULL == p_bytes)
	{
		goto END;
	}
	if (BYTE_ORDER == LITTLE_ENDIAN)
	{
		si_adler_init_prime_le(p_bytes, block_size);
	}
	else
	{
		si_adler_init_prime_be(p_bytes, block_size);
	}
END:
	return;
}

void si_adler_init(si_adler_t* const p_hash)
{
	if (NULL == p_hash)
	{
		goto END;
	}
	memset(p_hash->p_hash, 0x00, p_hash->block_size);
	// Seed hash buffer
	increment_bytes(p_hash->p_hash, p_hash->block_size);
END:
	return;
}

void si_adler_new(si_adler_t* const p_hash, const size_t block_size)
{
	if (NULL == p_hash)
	{
		goto END;
	}
	p_hash->block_size = block_size;
	p_hash->p_hash = calloc(sizeof(uint8_t), p_hash->block_size);
	if (NULL == p_hash->p_hash)
	{
		goto END;
	}
	si_adler_init(p_hash);
END:
	return;
}

// Adler Varient for N-Bits
void si_adler_update(si_adler_t* const p_hash, const uint8_t* const p_buffer,
	const size_t input_buffer_size)
{
	// Validate parameters
	if ((NULL == p_hash) || (NULL == p_buffer))
	{
		goto END;
	}
	const size_t half_bytes = p_hash->block_size / 2u;
	const size_t buffer_size = half_bytes + 1u;
	
	// Declare some block_size buffers
	uint8_t* p_prm = calloc(buffer_size, sizeof(uint8_t));
	uint8_t* p_inp = calloc(buffer_size, sizeof(uint8_t));
	uint8_t* p_lsb = calloc(buffer_size, sizeof(uint8_t));
	uint8_t* p_msb = calloc(buffer_size, sizeof(uint8_t));
	if ((NULL == p_prm) || (NULL == p_inp) ||
	    (NULL == p_lsb) || (NULL == p_msb))
	{
		goto CLEAN;
	}
	
	// Initialize prime number for block size.
	si_adler_init_prime(p_prm, p_hash->block_size);
	// Digest fixed block size input
	for (size_t iii = 0u; iii < input_buffer_size; iii++)
	{
		// Buffer = LSB with +1 pad
		memset(p_lsb, 0x00, buffer_size);
		memcpy(p_lsb, &p_hash->p_hash[0u], half_bytes);
		// Buffer = MSB with +1 pad
		memset(p_msb, 0x00, buffer_size);
		memcpy(p_msb, &p_hash->p_hash[half_bytes], half_bytes);
		// Buffer input byte +2 pad
		memset(p_inp, 0x00, buffer_size);
		if (BYTE_ORDER == LITTLE_ENDIAN)
		{
			p_inp[0u] = p_buffer[iii];
		}
		else
		{
			p_inp[half_bytes - 1u] = p_buffer[iii];
		}
		// Add. lsb = lsb + input_buffer
		add_uint_bytes(p_lsb, p_inp, buffer_size);
		// Mod. lsb = lsb % PRIME
		mod_uint_bytes(p_lsb, p_prm, buffer_size);
		// Add. msb = msb + lsb
		add_uint_bytes(p_msb, p_lsb, buffer_size);
		// Mod. msb = msb % PRIME
		mod_uint_bytes(p_msb, p_prm, buffer_size);
		// Finished LSB & MSB apply to hash
		memcpy(&p_hash->p_hash[0u], p_lsb, half_bytes);
		memcpy(&p_hash->p_hash[half_bytes], p_msb, half_bytes);
	}
	// End
CLEAN:
	if (NULL != p_prm)
	{
		free(p_prm);
		p_prm = NULL;
	}
	if (NULL != p_inp)
	{
		free(p_inp);
		p_inp = NULL;
	}
	if (NULL != p_lsb)
	{
		free(p_lsb);
		p_lsb = NULL;
	}
	if (NULL != p_msb)
	{
		free(p_msb);
		p_msb = NULL;
	}
END:
	return;
}

void si_adler_free(si_adler_t* const p_hash)
{
	if (NULL == p_hash)
	{
		goto END;
	}
	if (NULL != p_hash->p_hash)
	{
		free(p_hash->p_hash);
		p_hash->p_hash = NULL;
	}
	p_hash->block_size = 0u;
END:
	return;
}

void si_adler_fprint(const si_adler_t* const p_hash, FILE* const p_file)
{
	if ((NULL == p_hash) || (NULL == p_file))
	{
		goto END;
	}
	if ((NULL == p_hash->p_hash) || (0u >= p_hash->block_size))
	{
		goto END;
	}
	fprintf(p_file, "0x");
	for (size_t iii = 0u; iii < p_hash->block_size; iii++)
	{
		size_t next_index = iii;
		if (BYTE_ORDER == LITTLE_ENDIAN)
		{
			next_index = p_hash->block_size - 1u - next_index;
		}
		fprintf(p_file, "%02X", p_hash->p_hash[next_index]);
	}
END:
	return;
}
