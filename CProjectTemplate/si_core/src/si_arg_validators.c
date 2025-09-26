// si_arg_validators.c

#include "si_arg_validators.h"

bool port_validator(void* p_port)
{
    bool result = false;
    if (NULL == p_port)
    {
        goto END;
    }
    if (0 == *((uint16_t*)p_port))
    {
        goto END;
    }
    result = true;
END:
    return result;
}

bool dir_validator(void* p_path)
{
    bool result = false;
    // On linux any NULL-terminated string can be a path.
    // Here we just ensure the path is a directory.
    const bool is_dir = path_is_dir(p_path, true);
    if (false == is_dir)
    {
        goto END;
    }
    result = true;
END:
    return result;
}