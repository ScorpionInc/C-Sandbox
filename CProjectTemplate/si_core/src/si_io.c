// si_io.c
#include "si_io.h"

#ifdef SI_OS_IS_NOT_SUPPORTED

/** Doxygen
 * @brief Generic/Unknown implementation of file_clone_data().
 * 
 * @param p_source_path C string file path of file containing data to be read.
 * @param p_sink_path C string file path of file to be overwritten/created.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool file_clone_data_u(const char* const p_source_path,
	const char* const p_sink_path)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

	FILE* p_source = fopen(p_source_path, "r");
	if (NULL == p_source)
	{
		goto END;
	}
	FILE* p_sink = fopen(p_sink_path, "w");
	if (NULL == p_sink)
	{
		goto CLEAN;
	}

	char next_byte = fgetc(p_source);
	while (next_byte != EOF)
	{
		fputc(next_byte, p_sink);
		next_byte = fgetc(p_source);
	}

	result = true;
CLEAN:
	if (NULL != p_source)
	{
		(void)fclose(p_source);
	}
	if (NULL != p_sink)
	{
		(void)fclose(p_sink);
	}
END:
	return result;
}

#endif // SI_OS_IS_NOT_SUPPORTED


void vfprintf_exclusive(FILE* const p_file, const char* const p_format, va_list arg_list)
{
	if ((NULL == p_file) || (NULL == p_format))
	{
		goto END;
	}
	si_flock(p_file);
	(void)vfprintf(p_file, p_format, arg_list);
	si_funlock(p_file);
END:
	return;
}

void  fprintf_exclusive(FILE* const p_file, const char* const p_format, ...)
{
	if ((NULL == p_file) || (NULL == p_format))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_format);
	vfprintf_exclusive(p_file, p_format, args);
	va_end(args);
END:
	return;
}

bool fcan_read(const FILE* const p_file)
{
	bool result = false;
	if (NULL == p_file)
	{
		goto END;
	}
#ifdef __linux__
	const int file_d = fileno((FILE*)p_file);
	if (0 > file_d)
	{
		goto END;
	}
	result = fd_can_read(file_d);
#else
#warning Unknown/Unsupported OS
#endif//__linux
END:
	return result;
}

size_t fwrite_all(FILE* const p_file,
	const void* const p_data, const size_t data_size)
{
	size_t amount_written = 0u;
	if ((NULL == p_file) || (NULL == p_data) || (0u >= data_size))
	{
		goto END;
	}
	while (amount_written < data_size)
	{
		const uint8_t* const p_next_start = &(
			((uint8_t*)p_data)[amount_written]
		);
		const size_t next_result = fwrite(
			p_next_start, sizeof(uint8_t),
			(data_size - amount_written), p_file
		);
		const bool file_error = ferror(p_file);
		if ((0u >= next_result) || (true == file_error))
		{
			perror("fwrite_all");
			goto END;
		}
		amount_written += next_result;
	}
	fflush(p_file);
END:
	return amount_written;
}

size_t fread_all(FILE* const p_file,
	const void* const p_buffer, const size_t buffer_size)
{
	size_t bytes_read = 0u;
	if ((NULL == p_file) || (NULL == p_buffer) || (0u >= buffer_size))
	{
		goto END;
	}
	while (bytes_read < buffer_size)
	{
		uint8_t* const p_next_start = &(
			((uint8_t*)p_buffer)[bytes_read]
		);
		const size_t next_read = fread(
			p_next_start, sizeof(uint8_t),
			(buffer_size - bytes_read), p_file
		);
		const bool file_error = ferror(p_file);
		if ((0u >= next_read) || (true == file_error))
		{
			perror("fread_all");
			goto END;
		}
		bytes_read += next_read;
	}
END:
	return bytes_read;
}

void* fread_alloc_all(FILE* const p_file, size_t* const p_buffer_size)
{
	uint8_t* p_result = NULL;
	if ((NULL == p_file) || (NULL == p_buffer_size))
	{
		goto END;
	}
	if (0u >= *p_buffer_size)
	{
		goto END;
	}
	p_result = calloc(*p_buffer_size, sizeof(uint8_t));
	if (NULL == p_result)
	{
		goto END;
	}
	const size_t read_amount = fread_all(
		p_file, (void*)p_result, *p_buffer_size
	);
	if (read_amount < *p_buffer_size)
	{
		fprintf_exclusive(
			stderr,
			"fread_alloc_all() Failed to read all %lu bytes, read: %lu.\n",
			*p_buffer_size, read_amount
		);
	}
	*p_buffer_size = read_amount;
END:
	return (void*)p_result;
}

void* fread_alloc_until(FILE* const p_file,
	const uint8_t* const p_needle, size_t* const p_needle_size)
{
	//! TODO *BUG* May over-read for large byte patterns.
	uint8_t* p_result     = NULL;
	size_t   current_size = 0u;
	if ((NULL == p_file) || (NULL == p_needle) || (NULL == p_needle_size))
	{
		goto END;
	}
	if (0u >= *p_needle_size)
	{
		goto END;
	}

	current_size = *p_needle_size;
	p_result = fread_alloc_all(p_file, &current_size);
	if ((NULL == p_result) || (*p_needle_size != current_size))
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}

	uint8_t* p_pattern = NULL;
	size_t next_size = 0u;
	while (NULL == p_pattern)
	{
		p_pattern = memmem(
			p_result, current_size, p_needle, *p_needle_size
		);
		if (NULL != p_pattern)
		{
			break;
		}
		p_pattern = realloc(p_result, current_size + *p_needle_size);
		if (NULL == p_pattern)
		{
			break;
		}
		p_result = p_pattern;
		p_pattern = NULL;
		next_size = fread_all(p_file, &(p_result[current_size]), *p_needle_size);
		if (0u >= next_size)
		{
			break;
		}
		current_size += next_size;
		p_pattern = memmem(
			p_result, current_size, p_needle, *p_needle_size
		);
	}
END:
	(*p_needle_size) = current_size;
	return (void*)p_result;
}

char* fread_alloc_line(FILE* const p_file, size_t* const p_size)
{
	char* p_result = NULL;
	size_t alloc_size = 0u;
	if (NULL == p_file)
	{
		goto END;
	}
	const char* const p_needle = "\n";
	const size_t needle_len = strnlen(p_needle, INT_MAX);
	if ((INT_MAX <= needle_len) || (0 >= needle_len))
	{
		// Should never happen but just in case.
		goto END;
	}
	alloc_size = needle_len;
	p_result = fread_alloc_until(
		p_file, (const uint8_t*)p_needle, &alloc_size
	);
	if (NULL != p_result)
	{
		p_result[alloc_size - 1u] = '\0';
	}
END:
	if (NULL != p_size)
	{
		*p_size = alloc_size;
	}
	return p_result;
}

char* prompt(const char* const p_format, ...)
{
	char* p_result = NULL;
	const char* const default_prompt = "=> ";
	va_list args = {0};
	va_start(args, p_format);
	if (NULL == p_format)
	{
		printf("%s", default_prompt);
	}
	else
	{
		vprintf(p_format, args);
	}
	p_result = fread_alloc_line(stdin, NULL);
	va_end(args);
	return p_result;
}

bool path_is_dir(const char* const p_path, const bool follow_links)
{
	bool result = false;
	if (NULL == p_path)
	{
		goto END;
	}
#ifdef __linux__
	struct stat path_stat = {0};
	int stat_result = -1;
	char* expanded_path = shell_expand_path_l(p_path);
	if (true == follow_links)
	{
		stat_result = stat(expanded_path, &path_stat);
	}
	else
	{
		stat_result = lstat(expanded_path, &path_stat);
	}
	free(expanded_path);
	expanded_path = NULL;
	if (0 != stat_result)
	{
		perror("path_is_dir() call to stat() failed");
		goto END;
	}
	result = S_ISDIR(path_stat.st_mode);
#else
#warning Unsupported Operating System
#endif//__linux__
END:
	return result;
}

size_t path_file_size_3(const char* const p_path, const bool follow_links,
	const bool recursive)
{
	//!TODO See TODOs
	size_t result = SIZE_MAX;
	if (NULL == p_path)
	{
		goto END;
	}
#ifdef __linux__
	struct stat file_stat = {0};
	int stat_result = -1;
	char* expanded_path = shell_expand_path_l(p_path);
	if (true == follow_links)
	{
		stat_result = stat(expanded_path, &file_stat);
	}
	else
	{
		stat_result = lstat(expanded_path, &file_stat);
	}
	free(expanded_path);
	expanded_path = NULL;
	if (0 != stat_result)
	{
		goto END;
	}
	const bool is_dir = S_ISDIR(file_stat.st_mode);
	if (true == is_dir)
	{
		result = 0u;
		if (true != recursive)
		{
			goto END;
		}
		// TODO Implement directory recursion.
	}
	else if (0 <= file_stat.st_size)
	{
		result = (size_t)file_stat.st_size;
	}
#else
#warning Unsupported Operating System
#endif//__linux__
END:
	return result;
}
inline size_t path_file_size(const char* const p_path, const bool follow_links)
{
	// Default value of recursive is false.
	return path_file_size_3(p_path, follow_links, false);
}

void path_perms_fprint(FILE* const p_file, const char* const p_path)
{
	if ((NULL == p_file) || (NULL == p_path))
	{
		goto END;
	}
#ifdef __linux__
	struct stat file_stat = {0};
	const int lstat_result = lstat(p_path, &file_stat);
	if (0 > lstat_result)
	{
		goto END;
	}
	mode_t_fprint(stdout, file_stat.st_mode);
	const bool has_acl = path_has_acl(p_path);
	if (true == has_acl)
	{
		fprintf_exclusive(p_file, "+");
	}
	fprintf_exclusive(p_file, " %s", p_path);
#else
#warning Unsupported Operating System
#endif//__linux__
END:
	return;
}

bool file_clone_data(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
#ifdef SI_OS_IS_SUPPORTED
	result = file_clone_data_np(p_source_path, p_sink_path, follow_links);
#else
	result = file_clone_data_u(p_source_path, p_sink_path);
#endif // End call OS specific function
END:
	return result;
}

bool file_clone_meta(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
#ifdef SI_OS_IS_SUPPORTED
	result = file_clone_meta_np(p_source_path, p_sink_path, follow_links);
#else
#warning Unsupported OS
#endif // End call OS specific function
END:
	return result;
}

bool file_clone_to(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
#ifdef SI_OS_IS_SUPPORTED
	result = file_clone_to_np(
		p_source_path, p_sink_path, follow_links
	);
	goto END;
#else
#warning Unsupported Operating System.
#endif // End call OS specific function
END:
	return result;
}

bool for_each_file_5(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs, const bool recursive)
{
	bool result = false;
	if ((NULL == p_path) || (NULL == p_handler))
	{
		goto END;
	}
#ifdef SI_OS_IS_SUPPORTED
	result = for_each_file_np_5(
		p_path, p_handler, p_param, handle_dirs, recursive
	);
	goto END;
#else
#warning Unsupported Operating System.
#endif // End call OS specific function
END:
	return result;
}
inline bool for_each_file_4(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs)
{
	// Default value for bool recursive is SI_IO_DEFAULT_RECURSIVE(true).
	return for_each_file_5(
		p_path, p_handler, p_param, handle_dirs, SI_IO_DEFAULT_RECURSIVE
	);
}
inline bool for_each_file_3(const char* const p_path,
	const for_file_handler p_handler, void* const p_param)
{
	// Default value for bool handle_dirs is SI_IO_DEFAULT_HANDLE_DIRS(false).
	return for_each_file_4(
		p_path, p_handler, p_param, SI_IO_DEFAULT_HANDLE_DIRS
	);
}
inline bool for_each_file(const char* const p_path,
	const for_file_handler p_handler)
{
	// Default value for void* p_param is NULL.
	return for_each_file_3(p_path, p_handler, NULL);
}
