// si_arg_validators.c

#include "si_arg_validators.h"

bool port_validator(const void* const p_port)
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

bool file_path_validator(const void* const p_path)
{
	bool result = false;
	if(NULL == p_path)
	{
		goto END;
	}
#ifdef __linux__
	size_t sanity_check = 0u;
	const char* p_str = (const char*)p_path;
	while('\0' != *p_str)
	{
		if(sanity_check > PATH_MAX)
		{
			break;
		}
		sanity_check++;
		// All characters are valid except NULL in linux file paths.
		p_str++;
	}
	result = (sanity_check <= PATH_MAX);
#else
#warning Unsupported/Unknown OS
#endif // __linux__
END:
	return result;
}

bool dir_validator(const void* const p_path)
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