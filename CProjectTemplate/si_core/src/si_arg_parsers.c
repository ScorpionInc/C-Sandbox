// si_arg_parsers.c

#include "si_arg_parsers.h"

/** Doxygen
 * @brief Attempts to parse a uint16 port number from string.
 * 
 * @param p_str C String to parse port number from.
 * 
 * @return Returns heap pointer to port number on success. NULL otherwise.
 */
void* port_parser(const char* const p_str)
{
    uint16_t* p_port = NULL;
    if (NULL == p_str)
    {
        goto END;
    }
    p_port = calloc(1u, sizeof(uint16_t));
    if (NULL == p_port)
    {
        goto END;
    }
    unsigned long parse_buffer = 0L;
    char* p_end_ptr = NULL;
    parse_buffer = strtoul(p_str, &p_end_ptr, 10);
    if ((p_str == p_end_ptr) || (NULL == p_end_ptr))
    {
        free(p_port);
        p_port = NULL;
        goto END;
    }
    if ('\0' != *p_end_ptr)
    {
        free(p_port);
        p_port = NULL;
        goto END;
    }
    if ((ERANGE == errno) || (UINT16_MAX < parse_buffer))
    {
        free(p_port);
        p_port = NULL;
        goto END;
    }
    *p_port = (uint16_t)parse_buffer;
END:
    return (void*)p_port;
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