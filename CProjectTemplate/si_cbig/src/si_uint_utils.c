//si_big_uint.c

#include "si_uint_utils.h"

// Used for easy but inefficient big endian implementations
void reverse_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	uint8_t byte = 0x00;
	for(size_t i = 0u; i < (size / 2); i++)
	{
		const size_t far_index = (size - 1u - i);
		byte = p_bytes[0];
		p_bytes[0] = p_bytes[far_index];
		p_bytes[far_index] = byte;
	}
	// End
END:
	return;
}

// Needed for generation of a 2's compliment.
// No change to bytes on error.
void invert_uint_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation parameters
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < size; i++)
	{
		p_bytes[i] = ~p_bytes[i];
	}
	// End
END:
	return;
}

// These shifts could be optimized faster by doing multiple bits at a time but
// this works for now.
// TODO Add support for multiple bit shifts per operation.
void lshift_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < size; i++)
	{
		uint8_t bit = 0x00;
		if(i < (size - 1))
		{
			bit = (p_bytes[i + 1] & (1u << (CHAR_BIT - 1u))) ? 1u : 0u;
		}
		p_bytes[i] <<= 1;
		p_bytes[i] |= bit;
	}
	// End
END:
	return;
}
void lshift_bytes_by(uint8_t* const p_bytes, const size_t size,
	const size_t amount)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < amount; i++)
	{
		lshift_bytes(p_bytes, size);
	}
	// End
END:
	return;
}
void rshift_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < size; i++)
	{
		uint8_t bit = 0x00;
		if(i < (size - 1))
		{
			bit = p_bytes[i] & 1;
		}
		p_bytes[i] >>= 1;
		p_bytes[i] |= (bit << (CHAR_BIT - 1u));
	}
	// End
END:
	return;
}
void rshift_bytes_by(uint8_t* const p_bytes, const size_t size,
	const size_t amount)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < amount; i++)
	{
		rshift_bytes(p_bytes, size);
	}
	// End
END:
	return;
}

// Adds two byte arrays of size as unsigned int in little endian order.
// Results are stored in p_a, Returns carry/overflow flag/bit.
// No change to any value on error.
bool add_uint_le_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool carry_flag = false;
	// Validate
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u >= size)
	{
		goto END;
	}
	// Begin
	uint8_t last_carry = 0u;
	// Add unsigned long long when possible for fewer ops(faster)
	const size_t ull_size = sizeof(unsigned long long);
	const size_t full_ull_count = size / ull_size;
	unsigned long long* p_ull_a = (unsigned long long*)p_a;
	unsigned long long* p_ull_b = (unsigned long long*)p_b;
	for(size_t i = 0u; i < full_ull_count; i++)
	{
		carry_flag = false;
		if(p_ull_a[i] > (ULLONG_MAX - p_ull_b[i] - last_carry))
		{
			// Overflow
			carry_flag = true;
		}
		p_ull_a[i] += p_ull_b[i] + last_carry;
		last_carry = 0u;
		if(carry_flag)
		{
			last_carry++;
		}
	}
	// Add remaining individual bytes
	for(size_t i = (full_ull_count * ull_size); i < size; i++)
	{
		carry_flag = false;
		if(p_a[i] > (UINT8_MAX - p_b[i] - last_carry))
		{
			// Overflow
			carry_flag = true;
		}
		p_a[i] += p_b[i] + last_carry;
		last_carry = 0u;
		if(carry_flag)
		{
			last_carry++;
		}
	}
	// End
END:
	return carry_flag;
}

// Adds to byte arrays in big endian order storing the result in p_a.
bool add_uint_be_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool result = false;
	// Validate
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u == size)
	{
		goto END;
	}
	// Reversing array bytes here is easier, but slower than implementing
	// add_uint_bytes() again but for the big endian type byte iteration.
	// TODO Reimplement with fewer array iterations.
	reverse_bytes(p_a, size);
	{
		uint8_t b_reversed[size];
		memcpy(&b_reversed[0], p_b, size);
		reverse_bytes(&b_reversed[0], size);

		result = add_uint_le_bytes(p_a, &b_reversed[0], size);
	}
	reverse_bytes(p_a, size);
END:
	return result;
}

// Adds to byte arrays in host order storing the result in p_a.
bool add_uint_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u >= size)
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		result = add_uint_le_bytes(p_a, p_b, size);
	}
	else
	{
		result = add_uint_be_bytes(p_a, p_b, size);
	}
	// End
END:
	return result;
}

// Needed for generation of a 2's compliment.
void increment_le_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	{
		// Create a new big uint with a value of one in little endian.
		uint8_t one[size];
		// Zeroize
		memset(&one[0u], 0x00, size);
		// new big uint with a value of one in little endian.
		one[0u] = 0x01;
		// Add it to p_bytes
		add_uint_le_bytes(&p_bytes[0u], &one[0u], size);
	}
	// End
END:
	return;
}

// Needed for generation of a 2's compliment.
void increment_be_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	{
		// Create a big uint with a value of one in big endian.
		uint8_t one[size];
		// Zeroize
		memset(&one[0u], 0x00, size);
		one[size - 1u] = 0x01;
		// Add it to p_bytes
		add_uint_le_bytes(&p_bytes[0u], &one[0u], size);
	}
	// End
END:
	return;
}

// Needed for generation of a 2's compliment.
void increment_bytes(uint8_t* const p_bytes, const size_t size)
{
	// Validation
	if(NULL == p_bytes)
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		increment_le_bytes(p_bytes, size);
	}
	else
	{
		increment_be_bytes(p_bytes, size);
	}
	// End
END:
	return;
}

// Results stored in p_a, Returns underflow flag
// No change to any value on error.
bool sub_uint_le_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool underflow = false;
	// Validation parameters
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u >= size)
	{
		goto END;
	}
	// Begin
	{
		uint8_t b_compliment[size];
		memcpy(&b_compliment[0], p_b, size);
		invert_uint_bytes(&b_compliment[0], size);
		increment_le_bytes(&b_compliment[0], size);

		underflow = !add_uint_le_bytes(p_a, &b_compliment[0], size);
	}
	// End
END:
	return underflow;
}

bool sub_uint_be_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool result = false;
	// Validation
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u == size)
	{
		goto END;
	}
	// TODO See add_uint_be_bytes about reversing efficiency
	reverse_bytes(p_a, size);
	{
		uint8_t b_reversed[size];
		memcpy(&b_reversed[0], p_b, size);
		reverse_bytes(&b_reversed[0], size);

		sub_uint_le_bytes(p_a, &b_reversed[0], size);
	}
	reverse_bytes(p_a, size);
END:
	return result;
}

bool sub_uint_bytes(uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	bool result = false;
	// Validate parameters
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	if(0u >= size)
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		result = sub_uint_le_bytes(p_a, p_b, size);
	}
	else
	{
		result = sub_uint_be_bytes(p_a, p_b, size);
	}
	// End
END:
	return result;
}

//memcmp() but from MSB->LSB
int cmp_uint_le_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	int result = 0;
	// Validation
	if(p_a == p_b)
	{
		// Pointing to the same address
		goto END;
	}
	if((NULL == p_a) || (NULL == p_b))
	{
		// Undefined behavior
		goto END;
	}
	// Begin
	for(size_t i = 0u; i < size; i++)
	{
		const size_t next_index = size - 1u - i;
		if(p_a[next_index] > p_b[next_index])
		{
			result = 1;
			goto END;
		}
		if(p_a[next_index] < p_b[next_index])
		{
			result = -1;
			goto END;
		}
	}
	// End
END:
	return result;
}

// Already in MSB->LSB order
int cmp_uint_be_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	int result = 0;
	// Validation
	if((NULL == p_a) || (NULL == p_b))
	{
		// Undefined behavior
		goto END;
	}
	// Begin
	result = memcmp(p_a, p_b, size);
	// End
END:
	return result;
}

int cmp_uint_bytes(const uint8_t* const p_a, const uint8_t* const p_b,
	const size_t size)
{
	int result = 0;
	// Validate parameters
	if((NULL == p_a) || (NULL == p_b))
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		result = cmp_uint_le_bytes(p_a, p_b, size);
	}
	else
	{
		result = cmp_uint_be_bytes(p_a, p_b, size);
	}
	// End
END:
	return result;
}

// TODO Use Shift-Subtract algorithm for faster binary division
// Stores quotient in p_dividend. Remainder in p_remainder if it's not NULL.
// No change to any value on error.
void div_uint_le_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size,
	uint8_t* const p_remainder)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	if(0u >= size)
	{
		goto END;
	}
	{
		uint8_t zero[size];
		memset(&zero[0], 0x00, size);
		if(0 == cmp_uint_le_bytes(&p_divisor[0], &zero[0], size))
		{
			// Divide by zero error.
			goto END;
		}
	}
	if(0 > cmp_uint_bytes(&p_dividend[0], &p_divisor[0], size))
	{
		// If not NULL set remainder.
		if(NULL != p_remainder)
		{
			memcpy(&p_remainder[0], &p_dividend[0], size);
		}
		// Dividend < Divisor thus no quotient
		memset(&p_dividend[0], 0x00, size);
		goto END;
	}
	// Begin
	{
		uint8_t one[size];
		uint8_t counter[size];
		memset(&one, 0x00, size);
		memset(&counter, 0x00, size);
		one[0] = 0x01;
		while(0 <= cmp_uint_le_bytes(&p_dividend[0], &p_divisor[0], size))
		{
			// Divisor >= Dividend thus can divide.
			// Update dividend
			sub_uint_le_bytes(&p_dividend[0], &p_divisor[0], size);
			// Increment counter
			add_uint_le_bytes(&counter[0], &one[0], size);
		}
		// Set dividend to quotient and if not NULL set remainder.
		if(NULL != p_remainder)
		{
			memcpy(&p_remainder[0], &p_dividend[0], size);
		}
		memcpy(&p_dividend[0], &counter[0], size);
	}
	// End
END:
	return;
}

// See above note on reversing efficiency.
void div_uint_be_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size,
	uint8_t* const p_remainder)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	// Begin
	// TODO See add_uint_be_bytes about reversing efficiency
	reverse_bytes(&p_dividend[0], size);
	{
		uint8_t p_mut_divisor[size];
		memcpy(&p_mut_divisor[0], &p_divisor[0], size);
		reverse_bytes(&p_mut_divisor[0], size);
		div_uint_le_bytes(&p_dividend[0], &p_mut_divisor[0], size,
			p_remainder);
	}
	if(NULL != p_remainder)
	{
		reverse_bytes(&p_remainder[0], size);
	}
	reverse_bytes(&p_dividend[0], size);
	// End
END:
	return;
}

void div_uint_bytes(uint8_t* const p_dividend,
    const uint8_t* const p_divisor, const size_t size,
    uint8_t* const p_remainder)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		div_uint_le_bytes(p_dividend, p_divisor, size, p_remainder);
	}
	else
	{
		div_uint_be_bytes(p_dividend, p_divisor, size, p_remainder);
	}
	// End
END:
	return;
}

void mod_uint_le_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	// Begin
	{
		uint8_t quotient[size];
		memcpy(&quotient[0u], &p_dividend[0u], size);
		div_uint_le_bytes(&quotient[0u], &p_divisor[0u], size, &p_dividend[0]);
	}
	// End
END:
	return;
}
void mod_uint_be_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	// Begin
	{
		uint8_t quotient[size];
		memcpy(&quotient[0u], &p_dividend[0u], size);
		div_uint_be_bytes(&quotient[0u], &p_divisor[0u], size, &p_dividend[0]);
	}
	// End
END:
	return;
}
void mod_uint_bytes(uint8_t* const p_dividend,
	const uint8_t* const p_divisor, const size_t size)
{
	// Validation
	if((NULL == p_dividend) || (NULL == p_divisor))
	{
		goto END;
	}
	// Begin
	if(BYTE_ORDER == LITTLE_ENDIAN)
	{
		mod_uint_le_bytes(p_dividend, p_divisor, size);
	}
	else
	{
		mod_uint_be_bytes(p_dividend, p_divisor, size);
	}
	// End
END:
	return;
}
