// si_to_string.c
#include "si_to_string.h"

char* vto_string(const size_t max_str_len, const char* const format_str, va_list args)
{
	char* p_result = NULL;
	if ((0u >= max_str_len) || (NULL == format_str))
	{
		goto END;
	}
	p_result = calloc(max_str_len + 1u, sizeof(char));
	if (NULL == p_result)
	{
		goto END;
	}
	const int count = vsnprintf(p_result, max_str_len + 1u, format_str, args);
	if (count > max_str_len)
	{
		// No partials conversions allowed.
		free(p_result);
		p_result = NULL;
		goto END;
	}
	// This should already be done by vsnprintf but better safe than sorry.
	p_result[max_str_len] = '\0';
END:
	return p_result;
}
char* to_string(const size_t max_str_len, const char* const format_str, ...)
{
	char* p_result = NULL;
	if ((0u >= max_str_len) || (NULL == format_str))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, format_str);
	p_result = vto_string(max_str_len, format_str, args);
	va_end(args);
END:
	return p_result;
}

bool fit_str(char** const pp_str)
{
	bool result = false;
	if (NULL == pp_str)
	{
		goto END;
	}
	char* p_str = *pp_str;
	size_t str_len = strnlen(p_str, INT_MAX);
	if (SIZE_MAX <= str_len)
	{
		goto END;
	}
	void* const p_realloc_result = realloc(p_str, str_len);
	if (NULL == p_realloc_result)
	{
		goto END;
	}
	*pp_str = p_realloc_result;
	result = true;
END:
	return result;
}

inline char* char_to_string(const char value)
{
	return to_string(1u, "%c", value);
}
char* bool_to_string(const bool value)
{
	char* p_str = to_string(5u, "%s", value ? "True" : "False");
	if (true == value)
	{
		(void)fit_str(&p_str);
	}
	return p_str;
}

/** Doxygen
 * @brief Calculates the number of digits needed to represent an integer value.
 * 
 * @param value Int value to calculate digit count for.
 * 
 * @return Returns the number of characters in the string value of a signed int
 */
static size_t signed_to_digit_count(const long long int value)
{
	size_t result = 0u;
	long long int mut_value = value;
	if (0 > mut_value)
	{
		mut_value *= -1L;
		result++;
	}
	const double decimal_base = 10;
	double base_power = 1;
	double next_result = 0.0;
	do
	{
		const double next_divisor = pow(decimal_base, base_power);
		next_result = ((double)mut_value) / next_divisor;
		base_power += 1.0;
		result++;
	} while (1.0 < next_result);
	return result;
}
char* signed_to_string_3(const void* const p_value, const size_t size,
	const int pad)
{
	char* p_result = NULL;
	if ((NULL == p_value) || (0u >= size) || (sizeof(long long int) < size))
	{
		goto END;
	}
	
	long long int l_value = 0L;
	switch (size)
	{
	case(1u):
		l_value = (long long int)(*(int8_t*)p_value);
	break;
	case(2u):
		l_value = (long long int)(*(int16_t*)p_value);
	break;
	case(4u):
		l_value = (long long int)(*(int32_t*)p_value);
	break;
	case(8u):
		l_value = (long long int)(*(int64_t*)p_value);
	break;
	default:
		// Weird unsupported read size(3,5,6,7)
		goto END;
	break;
	}
	const int abs_pad = (0 > pad) ? (-1 * pad) : pad;
	const size_t char_count = signed_to_digit_count(l_value);
	const size_t result_count = (abs_pad > char_count) ? abs_pad : char_count;
	p_result = to_string(result_count, "%*lld", pad, l_value);
END:
	return p_result;
}
char* signed_to_string(const void* const p_value, const size_t size)
{
	// Default value of pad is 0(None).
	return signed_to_string_3(p_value, size, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* short_to_string_2(const short value, const int pad)
{
	return signed_to_string_3(&value, sizeof(short), pad);
}
inline char* short_to_string(const short value)
{
	// Default value of pad is 0(None).
	return short_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* int_to_string_2(const int value, const int pad)
{
	return signed_to_string_3(&value, sizeof(int), pad);
}
inline char* int_to_string(const int value)
{
	// Default value of pad is 0(None).
	return int_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* long_to_string_2(const long int value, const int pad)
{
	return signed_to_string_3(&value, sizeof(long), pad);
}
inline char* long_to_string(const long int value)
{
	// Default value of pad is 0(None).
	return long_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* llong_to_string_2(const long long int value, const int pad)
{
	return signed_to_string_3(&value, sizeof(long long int), pad);
}
inline char* llong_to_string(const long long int value)
{
	// Default value of pad is 0(None).
	return llong_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

/** Doxygen
 * @brief Calculates the number of digits needed to represent an integer value.
 * 
 * @param value Int value to calculate digit count for.
 * 
 * @return Returns the number of characters in the string value of a signed int
 */
static size_t unsigned_to_digit_count(const unsigned long long int value)
{
	size_t result = 0u;
	const double decimal_base = 10;
	double base_power = 1;
	double next_result = 0.0;
	do
	{
		const double next_divisor = pow(decimal_base, base_power);
		next_result = ((double)value) / next_divisor;
		base_power += 1.0;
		result++;
	} while (1.0 < next_result);
	return result;
}
char* unsigned_to_string_3(const void* const p_value, const size_t size,
	const int pad)
{
	char* p_result = NULL;
	if ((NULL == p_value) || (0u >= size) || (sizeof(long long int) < size))
	{
		goto END;
	}
	
	unsigned long long int ul_value = 0U;
	switch (size)
	{
	case(1u):
		ul_value = (long long int)(*(uint8_t*)p_value);
	break;
	case(2u):
		ul_value = (long long int)(*(uint16_t*)p_value);
	break;
	case(4u):
		ul_value = (long long int)(*(uint32_t*)p_value);
	break;
	case(8u):
		ul_value = (long long int)(*(uint64_t*)p_value);
	break;
	default:
		// Weird unsupported read size(3,5,6,7)
		goto END;
	break;
	}
	const int abs_pad = (0 > pad) ? (-1 * pad) : pad;
	const size_t char_count = unsigned_to_digit_count(ul_value);
	const size_t result_count = (abs_pad > char_count) ? abs_pad : char_count;
	p_result = to_string(result_count, "%*llu", pad, ul_value);
END:
	return p_result;
}
char* unsigned_to_string(const void* const p_value, const size_t size)
{
	// Default value of pad is 0(None).
	return unsigned_to_string_3(p_value, size, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* ushort_to_string_2(const unsigned short value, const int pad)
{
	return unsigned_to_string_3(&value, sizeof(unsigned short), pad);
}
inline char* ushort_to_string(const unsigned short value)
{
	// Default value of pad is 0(None).
	return ushort_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* uint_to_string_2(const unsigned int value, const int pad)
{
	return unsigned_to_string_3(&value, sizeof(unsigned int), pad);
}
inline char* uint_to_string(const unsigned int value)
{
	// Default value of pad is 0(None).
	return uint_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* ulong_to_string_2(const unsigned long int value, const int pad)
{
	return unsigned_to_string_3(&value, sizeof(unsigned long int), pad);
}
inline char* ulong_to_string(const unsigned long int value)
{
	// Default value of pad is 0(None).
	return ulong_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}

inline char* ullong_to_string_2(const unsigned long long int value,
	const int pad)
{
	return unsigned_to_string_3(&value, sizeof(unsigned long long int), pad);
}
inline char* ullong_to_string(const unsigned long long int value)
{
	// Default value of pad is 0(None).
	return ullong_to_string_2(value, SI_TO_STRING_DEFAULT_PADDING);
}
