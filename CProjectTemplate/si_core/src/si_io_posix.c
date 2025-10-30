// si_io_posix.c
#include "si_io_posix.h"

#ifndef _GNU_SOURCE
void* memmem(const void* const p_haystack, const size_t haystack_len,
	const void* const p_needle, const size_t needle_len)
{
	const void* p_result = NULL;
	if ((NULL == p_haystack) || (NULL == p_needle))
	{
		goto END;
	}
	if ((0u >= haystack_len) || (needle_len > haystack_len))
	{
		goto END;
	}
	// This is to maintain compatibility with the GNU implementation.
	if (0u >= needle_len)
	{
		p_result = p_haystack;
		goto END;
	}
	const uint8_t* const p_hay = (uint8_t*)p_haystack;
	const size_t iterations = (haystack_len - needle_len) + 1u;
	for (size_t iii = 0u; iii < iterations; iii++)
	{
		const int cmp_result = memcmp(&(p_hay[iii]), p_needle, needle_len);
		if (0 == cmp_result)
		{
			p_result = (void*)&(p_hay[iii]);
			break;
		}
	}
END:
	return (void*)p_result;
}
#endif// Adds support for a used GNU function.

#ifdef __linux__

void vfprintf_exclusive_np(FILE* const p_file, const char* const p_format,
	va_list arg_list)
{
	if ((NULL == p_file) || (NULL == p_format))
	{
		goto END;
	}
	flockfile(p_file);
	(void)vfprintf(p_file, p_format, arg_list);
	funlockfile(p_file);
END:
	return;
}
void fprintf_exclusive_np(FILE* const p_file, const char* const p_format, ...)
{
	if ((NULL == p_file) || (NULL == p_format))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_format);
	vfprintf_exclusive_np(p_file, p_format, args);
	va_end(args);
END:
	return;
}

bool acl_is_basic(const acl_t p_acl)
{
	bool result = false;
	if (NULL == p_acl)
	{
		goto END;
	}
	const size_t BASIC_ACL_COUNT = 3u;
	acl_entry_t entry = {0};
	int entry_id = ACL_FIRST_ENTRY;
	size_t entry_counter = 0u;
	bool got_entry = acl_get_entry(p_acl, entry_id, &entry);
	while (true == got_entry)
	{
		entry_counter++;
		entry_id = ACL_NEXT_ENTRY;
		if (BASIC_ACL_COUNT < entry_counter)
		{
			break;
		}
		got_entry = acl_get_entry(p_acl, entry_id, &entry);
	}
	result = (BASIC_ACL_COUNT >= entry_counter);
END:
	return result;
}

/** Doxygen
 * @brief Writes a human-readable string value of WRDE return to a FILE.
 * 
 * @param p_file Pointer to a FILE to be written to.
 * @param value wordexp() return value whose string value is to be written.
 */
static void WRDE_fprint_return(FILE* const p_file, const int value)
{
	if (NULL == p_file)
	{
		goto END;
	}
	switch (value)
	{
		case(WRDE_BADCHAR):
			fprintf_exclusive_np(p_file, "%s", "BADCHAR");
		break;
		case(WRDE_BADVAL):
			fprintf_exclusive_np(p_file, "%s", "BADVAL");
		break;
		case(WRDE_CMDSUB):
			fprintf_exclusive_np(p_file, "%s", "CMDSUB");
		break;
		case(WRDE_NOSPACE):
			fprintf_exclusive_np(p_file, "%s", "NOSPACE");
		break;
		case(WRDE_SYNTAX):
			fprintf_exclusive_np(p_file, "%s", "SYNTAX");
		break;
		default:
			fprintf_exclusive_np(p_file, "%s", "UNKNOWN");
		break;
	}
END:
	return;
}

char* shell_expand_path_l(const char* const p_path)
{
	char* p_result = NULL;
	if (NULL == p_path)
	{
		goto END;
	}
	wordexp_t word_expansion = {0};
	const int expansion_result = wordexp(
		p_path, &word_expansion, (WRDE_NOCMD | WRDE_SHOWERR)
	);
	if ((EXIT_SUCCESS != expansion_result) || (0u >= word_expansion.we_wordc))
	{
		// No words were found/substituted. Or the word expansion failed.
		p_result = strdup(p_path);
	}
	if (EXIT_SUCCESS != expansion_result)
	{
		fprintf_exclusive_np(
			stderr,
			"Path expansion failed with error code: %d => ",
			expansion_result
		);
		WRDE_fprint_return(stderr, expansion_result);
		fprintf_exclusive_np(stderr, ".\n");
		goto END;
	}
	if (0u < word_expansion.we_wordc)
	{
		// Path was expanded, clone results onto the heap
		p_result = strdup(word_expansion.we_wordv[0]);
	}
	wordfree(&word_expansion);
END:
	return p_result;
}

bool path_has_acl(const char* const p_path)
{
	bool result = false;
	if (NULL == p_path)
	{
		goto END;
	}
	char* expanded_path = shell_expand_path_l(p_path);
	acl_t p_acl = acl_get_file(expanded_path, ACL_TYPE_ACCESS);
	free(expanded_path);
	expanded_path = NULL;
	if (NULL == p_acl)
	{
		goto END;
	}
	result = !acl_is_basic(p_acl);
	acl_free(p_acl);
	p_acl = NULL;
END:
	return result;
}

bool fd_has_acl(const int file_d)
{
	bool result = false;
	if (0 > file_d)
	{
		goto END;
	}
	acl_t p_acl = acl_get_fd(file_d);
	if (NULL == p_acl)
	{
		goto END;
	}
	result = !acl_is_basic(p_acl);
	acl_free(p_acl);
	p_acl = NULL;
END:
	return result;
}

bool fd_can_read(const int file_d)
{
	bool result = false;
	if (0 > file_d)
	{
		// Invalid file
		goto END;
	}
	struct pollfd pfd = {0};
	pfd.fd = file_d;
	pfd.events = POLLIN;
	const int poll_result = poll(&pfd, 1, 0);
	if (0 > poll_result)
	{
		goto END;
	}
	result = (pfd.revents & POLLIN);
END:
	return result;
}

bool fd_printf(const int file_d, const char* const p_format, ...)
{
	bool result = false;
	if ((0 > file_d) || (NULL == p_format))
	{
		goto END;
	}
	va_list args = {0};
	va_start(args, p_format);
	char* p_str = vstr_format(p_format, args);
	va_end(args);
	if (NULL == p_str)
	{
		goto END;
	}
	const size_t str_len = strnlen(p_str, INT_MAX);
	if (INT_MAX <= str_len)
	{
		free(p_str);
		p_str = NULL;
		goto END;
	}
	const size_t amount_written = fd_write_all(
		file_d, (uint8_t*)p_str, str_len
	);
	free(p_str);
	p_str = NULL;
	if (str_len != amount_written)
	{
		goto END;
	}
	result = true;
END:
	return result;
}

size_t fd_write_all(const int file_d,
	const uint8_t* const p_data, const size_t data_size)
{
	size_t result = 0u;
	if (0 > file_d)
	{
		// Invalid file
		goto END;
	}
	if ((NULL == p_data) || (0u >= data_size))
	{
		goto END;
	}
	while(result < data_size)
	{
		ssize_t next_write = write(
			file_d, &(p_data[result]), data_size - result
		);
		if (0 > next_write)
		{
			break;
		}
		result += next_write;
	}
END:
	return result;
}

size_t fd_read_all(const int file_d,
	uint8_t* const p_data, const size_t data_size)
{
	size_t result = 0u;
	if (0 > file_d)
	{
		// Invalid file
		goto END;
	}
	if ((NULL == p_data) || (0u >= data_size))
	{
		goto END;
	}
	while (result < data_size)
	{
		ssize_t next_read = read(
			file_d, &(p_data[result]), data_size - result
		);
		if (0 > next_read)
		{
			break;
		}
		result += next_read;
	}
END:
	return result;
}

char* fd_read_alloc_all(const int file_d, size_t* const p_size)
{
	char* p_result = NULL;
	if ((0 > file_d) || (NULL == p_size))
	{
		goto END;
	}
	const size_t alloc_size = *p_size;
	*p_size = 0u;
	p_result = calloc(alloc_size, sizeof(char));
	if (NULL == p_result)
	{
		goto END;
	}
	const size_t read_amount = fd_read_all(file_d, p_result, alloc_size);
	*p_size = read_amount;
END:
	return p_result;
}

char* fd_read_alloc_until(const int file_d,
	const uint8_t* const p_needle, size_t* const p_needle_size)
{
	char* p_result = NULL;
	if ((0 > file_d) || (NULL == p_needle) || (NULL == p_needle_size))
	{
		goto END;
	}
	const size_t needle_size = *p_needle_size;
	p_result = fd_read_alloc_all(file_d, p_needle_size);
	if (NULL == p_result)
	{
		goto END;
	}
	uint8_t* p_match = (uint8_t*)memmem(
		p_result, *p_needle_size, p_needle, needle_size
	);
	while (NULL == p_match)
	{
		uint8_t* p_tmp = NULL;
		p_tmp = realloc(p_result, (*p_needle_size) + 1u);
		if (NULL == p_tmp)
		{
			break;
		}
		p_result = p_tmp;
		ssize_t read_next = 0;
		while (1 > read_next)
		{
			read_next = read(file_d, &(p_result[*p_needle_size]), 1u);
			if (0 > read_next)
			{
				break;
			}
			*p_needle_size += read_next;
		}
		if (0 > read_next)
		{
			break;
		}
		p_match = (uint8_t*)memmem(
			p_result, *p_needle_size, p_needle, needle_size
		);
	}
END:
	return p_result;
}

char* fd_read_alloc_line(const int file_d, size_t* const p_size)
{
	size_t needle_size = 0u;
	char* p_result = NULL;
	if (0 > file_d)
	{
		goto END;
	}
	needle_size = 1u;
	p_result = fd_read_alloc_until(file_d, "\n", &needle_size);
	if (NULL == p_result)
	{
		goto END;
	}
	p_result[needle_size - 1u] = '\0';
END:
	if (NULL != p_size)
	{
		*p_size = needle_size;
	}
	return p_result;
}

/** Doxygen
 * @brief Maps the mode value from file stat struct and returns type char.
 *
 * @param mode mode_t value from a stat struct type.
 *
 * @return Returns char of file type based upon value of mode_t mode.
*/
static char mode_type_char(const mode_t mode)
{
	char result = '-';
	// Can't use switch here as each case test is a different macro
	const bool is_blk = S_ISBLK(mode);
	if (true == is_blk)
	{
		result = 'b';
		goto END;
	}
	const bool is_chr = S_ISCHR(mode);
	if (true == is_chr)
	{
		result = 'c';
		goto END;
	}
	const bool is_dir = S_ISDIR(mode);
	if (true == is_dir)
	{
		result = 'd';
		goto END;
	}
	const bool is_fifo = S_ISFIFO(mode);
	if (true == is_fifo)
	{
		result = 'f';
		goto END;
	}
	const bool is_lnk = S_ISLNK(mode);
	if (true == is_lnk)
	{
		result = 'l';
		goto END;
	}
	const bool is_sock = S_ISSOCK(mode);
	if (true == is_sock)
	{
		result = 's';
		goto END;
	}
END:
	return result;
}

void mode_t_fprint(FILE* const p_file, const mode_t mode)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const size_t PERMISSION_BITS = 3u;
	const size_t PERMISSION_GROUPS = 3u;
	const mode_t group_mask = ((mode_t)-1) >>
		(sizeof(mode_t) * CHAR_BIT - PERMISSION_BITS);
	const char type_char = mode_type_char(mode);
	fprintf_exclusive_np(p_file, "%c", type_char);
	for (size_t iii = 0u; iii < PERMISSION_GROUPS; iii++)
	{
		const size_t mask_shift = PERMISSION_BITS *
			(PERMISSION_GROUPS - 1 - iii);
		const mode_t mode_group = (mode &
			(group_mask << mask_shift)
		) >> mask_shift;
		fprintf_exclusive_np(p_file, "%s", (mode_group) >= 4 ? "r" : "-");
		fprintf_exclusive_np(p_file, "%s", (mode_group % 4) >= 2 ? "w" : "-");
		if ((0 == iii) && (mode & S_ISUID))
		{
			fprintf_exclusive_np(p_file, "s");
			continue;
		}
		fprintf_exclusive_np(p_file, "%s", 1 <= (mode_group % 2) ? "x" : "-");
	}
END:
	return;
}

void dirent_fprint(FILE* const p_file, const struct dirent* const p_entry)
{
	if ((NULL == p_file) || (NULL == p_entry))
	{
		goto END;
	}
	fprintf_exclusive_np(p_file, "%lu", p_entry->d_ino);
	fprintf_exclusive_np(p_file, " %s", p_entry->d_name);
#ifdef _DIRENT_HAVE_D_TYPE
	fprintf_exclusive_np(p_file, " %hhu", p_entry->d_type);
#endif//_DIRENT_HAVE_D_TYPE
#ifdef _DIRENT_HAVE_D_RECLEN
	fprintf_exclusive_np(p_file, " %hu", p_entry->d_reclen);
#endif//_DIRENT_HAVE_D_RECLEN
END:
	return;
}

bool file_clone_data_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

	// Opens file(s) for operation(s)
	struct stat source_stat = {0};
	const int source_flags = O_RDONLY |
		(follow_links ? 0x00 : O_NOFOLLOW);
	char* expanded_path = shell_expand_path_l(p_source_path);
	int source_fd = open(expanded_path, source_flags);
	free(expanded_path);
	expanded_path = NULL;
	if (0 > source_fd)
	{
		goto END;
	}
	const int stat_result = fstat(source_fd, &source_stat);
	if (0 > stat_result)
	{
		goto CLEAN;
	}
	const int sink_flags = (O_WRONLY | O_CREAT | O_TRUNC) |
		(follow_links ? 0x00 : O_NOFOLLOW);
	expanded_path = shell_expand_path_l(p_sink_path);
	int sink_fd = open(
		expanded_path, sink_flags, source_stat.st_mode
	);
	if ((0 > sink_fd) || (0 > source_stat.st_size))
	{
		goto CLEAN;
	}
	free(expanded_path);
	expanded_path = NULL;

	// Sends the data creating the file as needed.
	off_t offset = 0;
	ssize_t send_file_result = 1;
	while (0 < send_file_result)
	{
		const size_t count = ((size_t)source_stat.st_size - (size_t)offset);
		send_file_result = sendfile(
			sink_fd, source_fd, &offset, count
		);
		if ((0 > send_file_result) || (0 > offset))
		{
			goto CLEAN;
		}
	}

	result = true;
CLEAN:
	if (0 <= source_fd)
	{
		(void)close(source_fd);
	}
	source_fd = -1;
	if (0 <= sink_fd)
	{
		(void)close(sink_fd);
	}
	sink_fd = -1;
END:
	return result;
}

bool file_clone_meta_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result                = false;
	int  sink_fd               = -1;
	char* sink_expanded_path   = NULL;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

	// Opens file(s) for operation(s)
	struct stat source_stat    = {0};
	char* source_expanded_path = shell_expand_path_l(p_source_path);
	const int stat_result      = follow_links ?
		stat(source_expanded_path, &source_stat) :
		lstat(source_expanded_path, &source_stat);
	if (0 > stat_result)
	{
		goto CLEAN;
	}

	sink_expanded_path = shell_expand_path_l(p_sink_path);
	const int sink_flags = O_WRONLY |
		(follow_links ? 0x00 : O_NOFOLLOW);
	sink_fd = open(
		sink_expanded_path, sink_flags, source_stat.st_mode
	);
	if (0 > sink_fd)
	{
		goto CLEAN;
	}

	// Change File Mode
	const int mode_result = fchmod(sink_fd, source_stat.st_mode);
	if (0 > mode_result)
	{
		goto CLEAN;
	}

	// Change File Owner
	const int owner_result = fchown(
		sink_fd, source_stat.st_uid, source_stat.st_gid
	);
	if (0 > owner_result)
	{
		goto CLEAN;
	}

	// Change File Timestamps
	struct timespec new_times[2] = {0};
	new_times[0] = source_stat.st_atim; // Access
	new_times[1] = source_stat.st_mtim; // Modify
	const int time_result = futimens(sink_fd, new_times);
	if (0 > time_result)
	{
		goto CLEAN;
	}

	// Clone File ACL(s)
	acl_t p_acl = acl_get_file(source_expanded_path, ACL_TYPE_ACCESS);
	if (NULL == p_acl)
	{
		// This may not be an error if no ACL is defined.
		if (ENODATA == errno)
		{
			result = true;
		}
		goto CLEAN;
	}
	const int acl_result = acl_set_file(
		sink_expanded_path, ACL_TYPE_ACCESS, p_acl
	);
	result = (0 <= acl_result);
	acl_free(p_acl);
	p_acl = NULL;
CLEAN:
	free(source_expanded_path);
	source_expanded_path = NULL;
	free(sink_expanded_path);
	sink_expanded_path = NULL;
	if (0 <= sink_fd)
	{
		(void)close(sink_fd);
	}
	sink_fd = -1;
END:
	return result;
}

bool file_clone_to_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
	const bool data_result = file_clone_data_np(
		p_source_path, p_sink_path, follow_links
	);
	if (true != data_result)
	{
		goto END;
	}
	result = file_clone_meta_np(p_source_path, p_sink_path, follow_links);
END:
	return result;
}

bool for_each_file_np_5(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs, const bool recursive)
{
	bool result = false;
	if ((NULL == p_path) || (NULL == p_handler))
	{
		goto END;
	}

	char* p_fullpath = NULL;
	DIR* p_dir = NULL;
	struct dirent* p_dir_entry = NULL;

	char* expanded_path = shell_expand_path_l(p_path);
	p_dir = opendir(expanded_path);
	free(expanded_path);
	expanded_path = NULL;
	if (NULL == p_dir)
	{
		goto END;
	}

	p_dir_entry = readdir(p_dir);
	while (NULL != p_dir_entry)
	{
		const size_t name_size = sizeof(p_dir_entry->d_name);
		const size_t name_len = strnlen(p_dir_entry->d_name, name_size);
		if (INT64_MAX <= name_len)
		{
			// File name length is way too large.
			p_dir_entry = readdir(p_dir);
			continue;
		}
		bool is_dot_dir = (0 == strncmp(".", p_dir_entry->d_name, name_len));
		is_dot_dir |= (0 == strncmp("..", p_dir_entry->d_name, name_len));
		if (true == is_dot_dir)
		{
			// Skip dot dirs
			p_dir_entry = readdir(p_dir);
			continue;
		}
		char* expanded_path = shell_expand_path_l(p_path);
		p_fullpath = strv_clone_concat(3u, expanded_path, "/", p_dir_entry->d_name);
		free(expanded_path);
		expanded_path = NULL;
		if (NULL == p_fullpath)
		{
			p_dir_entry = readdir(p_dir);
			continue;
		}
		bool is_dir = false;
#ifdef _DIRENT_HAVE_D_TYPE
		is_dir = (DT_DIR == p_dir_entry->d_type);
#else
		is_dir = path_is_dir(p_fullpath, false);
#endif//_DIRENT_HAVE_D_TYPE
		if (true == is_dir)
		{
			if (true == recursive)
			{
				// Depth-First enumeration
				const bool dir_result = for_each_file_np_5(
					p_fullpath, p_handler, p_param, handle_dirs, recursive
				);
				if (true != dir_result)
				{
					break;
				}
			}
			if (true != handle_dirs)
			{
				// Skip Directories
				free((void*)p_fullpath);
				p_fullpath = NULL;
				p_dir_entry = readdir(p_dir);
				continue;
			}
		}
		// Handle file/link/ect.
		const bool is_good = p_handler(p_fullpath, p_dir_entry, p_param);
		if (true != is_good)
		{
			break;
		}
		free((void*)p_fullpath);
		p_fullpath = NULL;
		p_dir_entry = readdir(p_dir);
	}
	if (NULL != p_fullpath)
	{
		free((void*)p_fullpath);
		p_fullpath = NULL;
	}
	(void)closedir(p_dir);
	p_dir = NULL;
	result = true;
END:
	return result;
}
inline bool for_each_file_np_4(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs)
{
	// Default value of recursive is SI_IO_POSIX_DEFAULT_RECURSIVE(true)
	return for_each_file_np_5(
		p_path, p_handler, p_param, handle_dirs, SI_IO_POSIX_DEFAULT_RECURSIVE
	);
}
inline bool for_each_file_np_3(const char* const p_path,
	const for_file_handler p_handler, void* const p_param)
{
	// Default value of handle_dirs is SI_IO_POSIX_DEFAULT_HANDLE_DIRS(false)
	return for_each_file_np_4(
		p_path, p_handler, p_param, SI_IO_POSIX_DEFAULT_HANDLE_DIRS
	);
}
inline bool for_each_file_np(const char* const p_path,
	const for_file_handler p_handler)
{
	// Default value of p_param is NULL.
	return for_each_file_np_3(p_path, p_handler, NULL);
}

#endif //__linux__
