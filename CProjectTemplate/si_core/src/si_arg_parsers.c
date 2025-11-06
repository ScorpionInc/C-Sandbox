// si_arg_parsers.c

#include "si_arg_parsers.h"

/** Doxygen
 * @brief Local function to parse unsigned integer values <= ULONG_MAX.
 * 
 * @param p_str C String to parse integer from.
 * @param max Maximum value less than ULONG_MAX to cap unsigned int value.
 * 
 * @return Returns max+1 on error. Otherwise returns parsed value.
 */
static unsigned long long parse_uint(const char* const p_str, const unsigned long max)
{
    unsigned long long result = (unsigned long long)(max + 1UL);
    if (NULL == p_str)
    {
        goto END;
    }
    unsigned long parse_buffer = 0L;
    char* p_end_ptr = NULL;
    parse_buffer = strtoul(p_str, &p_end_ptr, 10);
    if ((p_str == p_end_ptr) || (NULL == p_end_ptr))
    {
        goto END;
    }
    if ('\0' != *p_end_ptr)
    {
        goto END;
    }
    if ((ERANGE == errno) || (UINT32_MAX < parse_buffer))
    {
        goto END;
    }
END:
    result = (unsigned long long)parse_buffer;
}

void* uint8_parser(const char* const p_str)
{
    uint8_t* p_result = NULL;
    if (NULL == p_str)
    {
        goto END;
    }

    // Allocate
    p_result = calloc(1u, sizeof(uint8_t));
    if (NULL == p_result)
    {
        goto END;
    }

    // Parse
    unsigned long long parsed = parse_uint(p_str, UINT8_MAX);
    if (UINT8_MAX < parsed)
    {
        // Parse failed.
        free(p_result);
        p_result = NULL;
        goto END;
    }
    *p_result = (uint8_t)parsed;
END:
    return (void*)p_result;
}

void* uint16_parser(const char* const p_str)
{
    uint16_t* p_result = NULL;
    if (NULL == p_str)
    {
        goto END;
    }

    // Allocate
    p_result = calloc(1u, sizeof(uint16_t));
    if (NULL == p_result)
    {
        goto END;
    }

    // Parse
    unsigned long long parsed = parse_uint(p_str, UINT16_MAX);
    if (UINT16_MAX < parsed)
    {
        // Parse failed.
        free(p_result);
        p_result = NULL;
        goto END;
    }
    *p_result = (uint16_t)parsed;
END:
    return (void*)p_result;
}

void* uint32_parser(const char* const p_str)
{
    uint32_t* p_result = NULL;
    if (NULL == p_str)
    {
        goto END;
    }

    // Allocate
    p_result = calloc(1u, sizeof(uint32_t));
    if (NULL == p_result)
    {
        goto END;
    }

    // Parse
    unsigned long long parsed = parse_uint(p_str, UINT32_MAX);
    if (UINT32_MAX < parsed)
    {
        // Parse failed.
        free(p_result);
        p_result = NULL;
        goto END;
    }
    *p_result = (uint32_t)parsed;
END:
    return (void*)p_result;
}

void* dir_parser(const char* const p_str)
{
    void* p_result = NULL;
    if (NULL == p_str)
    {
        goto END;
    }
    // Ensure value is path separator terminated.
    const size_t str_len = strnlen(p_str, INT_MAX);
    if (0u >= str_len)
    {
        goto END;
    }
    if ('/' == p_str[str_len - 1])
    {
#ifdef _GNU_SOURCE
		p_result = strndup(p_str, str_len);
#else
        p_result = strdup(p_str);
#endif//_GNU_SOURCE
    }
    else
    {
        p_result = str_clone_concat(p_str, "/");
    }
END:
    return p_result;
}
