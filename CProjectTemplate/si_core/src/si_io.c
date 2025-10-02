// si_io.c
#include "si_io.h"

#ifdef __linux__

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

bool path_has_acl(const char* const p_path)
{
	bool result = false;
	if (NULL == p_path)
	{
		goto END;
	}
	acl_t p_acl = acl_get_file(p_path, ACL_TYPE_ACCESS);
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
	fprintf(p_file, "%c", type_char);
	for (size_t iii = 0u; iii < PERMISSION_GROUPS; iii++)
	{
		const size_t mask_shift = PERMISSION_BITS *
			(PERMISSION_GROUPS - 1 - iii);
		const mode_t mode_group = (mode &
			(group_mask << mask_shift)
		) >> mask_shift;
		fprintf(p_file, "%s", (mode_group) >= 4 ? "r" : "-");
		fprintf(p_file, "%s", (mode_group % 4) >= 2 ? "w" : "-");
		if ((0 == iii) && (mode & S_ISUID))
		{
			fprintf(p_file, "s");
			continue;
		}
		fprintf(p_file, "%s", 1 <= (mode_group % 2) ? "x" : "-");
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
	fprintf(p_file, "%lu", p_entry->d_ino);
	fprintf(p_file, " %s", p_entry->d_name);
#ifdef _DIRENT_HAVE_D_TYPE
	fprintf(p_file, " %hhu", p_entry->d_type);
#endif//_DIRENT_HAVE_D_TYPE
#ifdef _DIRENT_HAVE_D_RECLEN
	fprintf(p_file, " %hu", p_entry->d_reclen);
#endif//_DIRENT_HAVE_D_RECLEN
END:
	return;
}

/** Doxygen
 * @brief Linux specific implementation of file_clone_data().
 * 
 * @param p_source_path C string file path of file containing data to be read.
 * @param p_sink_path C string file path of file to be overwritten/created.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool file_clone_data_l(const char* const p_source_path,
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
	int source_fd = open(p_source_path, source_flags);
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
	int sink_fd = open(
		p_sink_path, sink_flags, source_stat.st_mode
	);
	if ((0 > sink_fd) || (0 > source_stat.st_size))
	{
		goto CLEAN;
	}

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

/** Doxygen
 * @brief Linux specific implementation of the file_clone_meta() function.
 * 
 * @param p_source_path C string file path of file containing metadata to read.
 * @param p_sink_path C string file path of file to be modified.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool file_clone_meta_l(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

	// Opens file(s) for operation(s)
	struct stat source_stat = {0};
	const int stat_result = follow_links ?
		stat(p_source_path, &source_stat) :
		lstat(p_source_path, &source_stat);
	if (0 > stat_result)
	{
		goto END;
	}
	const int sink_flags = O_WRONLY |
		(follow_links ? 0x00 : O_NOFOLLOW);
	int sink_fd = open(
		p_sink_path, sink_flags, source_stat.st_mode
	);
	if (0 > sink_fd)
	{
		goto END;
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
	acl_t p_acl = acl_get_file(p_source_path, ACL_TYPE_ACCESS);
	if (NULL == p_acl)
	{
		// This may not be an error if no ACL is defined.
		if (ENODATA == errno)
		{
			result = true;
		}
		goto CLEAN;
	}
	const int acl_result = acl_set_file(p_sink_path, ACL_TYPE_ACCESS, p_acl);
	result = (0 <= acl_result);
	acl_free(p_acl);
	p_acl = NULL;
CLEAN:
	if (0 <= sink_fd)
	{
		(void)close(sink_fd);
	}
	sink_fd = -1;
END:
	return result;
}

static bool for_each_file_l(const char* const p_path,
	const for_file_handler p_handler,
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

	p_dir = opendir(p_path);
	if (NULL == p_dir)
	{
		goto END;
	}

	p_dir_entry = readdir(p_dir);
	while (NULL != p_dir_entry)
	{
		const size_t name_len = strnlen(p_dir_entry->d_name, INT64_MAX);
		if(INT64_MAX <= name_len)
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
		p_fullpath = strv_clone_concat(3u, p_path, "/", p_dir_entry->d_name);
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
				const bool dir_result = for_each_file_l(
					p_fullpath, p_handler, handle_dirs, recursive
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
		const bool is_good = p_handler(p_fullpath, p_dir_entry);
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

#elif defined _WIN32 // End of __linux__

/** Doxygen
 * @brief Windows specific implementation of file_clone_data().
 * 
 * @param p_source_path C string file path of file containing data to be read.
 * @param p_sink_path C string file path of file to be overwritten/created.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool file_clone_data_w(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	//!TODO
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

END:
	return result;
}

/** Doxygen
 * @brief Windows specific implementation of the file_clone_meta() function.
 * 
 * @param p_source_path C string file path of file containing metadata to read.
 * @param p_sink_path C string file path of file to be modified.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
static bool file_clone_meta_w(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	//!TODO
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}

END:
	return result;
}

static bool for_each_file_w(const char* const p_path,
	const for_file_handler p_handler,
	const bool handle_dirs, const bool recursive)
{
	bool result = false;
	if ((NULL == p_path) || (NULL == p_handler))
	{
		goto END;
	}

	const char* p_fullpath = NULL;
	WIN32_FIND_DATA find_data = {0};
	HANDLE file_handle = INVALID_HANDLE_VALUE;

	// Create search string to define file handle
	char search_str[MAX_PATH] = {0};
	sprintf_s(search_str, MAX_PATH, "%s\\*.*", p_path);
	file_handle = FindFirstFile(search_str, &find_data);
	if (INVALID_HANDLE_VALUE == file_handle)
	{
		goto END;
	}
	int find_next_result = 0;
	do
	{
		const size_t name_len = strnlen(find_data.cFileName, INT64_MAX);
		if(INT_MAX <= name_len)
		{
			// File name length is way too large.
			find_next_result = FindNextFile(file_handle, &find_data);
			continue;
		}
		bool is_dot_dir = (0 == strncmp(".", find_data.cFileName, name_len));
		is_dot_dir |= (0 == strncmp("..", find_data.cFileName, name_len));
		if (true == is_dot_dir)
		{
			// Skip dot dirs
			find_next_result = FindNextFile(file_handle, &find_data);
			continue;
		}
		p_fullpath = strv_clone_concat(p_path, "\\", find_data.cFileName);
		if (NULL == p_fullpath)
		{
			find_next_result = FindNextFile(file_handle, &find_data);
			continue;
		}
		const DWORD fad = (
			FILE_ATTRIBUTE_DIRECTORY & findData.dwFileAttributes
		);
		if (0 < fad)
		{
			if (true == recursive)
			{
				const bool dir_result = for_each_file_w(
					p_fullpath, p_handler, handle_dirs, recursive
				);
				if (true != dir_result)
				{
					break;
				}
			}
			if (true != handle_dirs)
			{
				// Skip directories
				free(p_fullpath);
				p_fullpath = NULL;
				find_next_result = FindNextFile(file_handle, &find_data);
				continue;
			}
		}
		// Handle files/links/ect.
		const bool handler_result = p_handler(p_fullpath, &find_data);
		if (true != handler_result)
		{
			break;
		}
		free(p_fullpath);
		p_fullpath = NULL;
		find_next_result = FindNextFile(file_handle, &find_data);
	} while (0 != find_next_result);
	if (NULL != p_fullpath)
	{
		free(p_fullpath);
		p_fullpath = NULL;
	}
	FindClose(file_handle);
	file_handle = INVALID_HANDLE_VALUE;
	result = true;
END:
	return result;
}

#else // End of _WIN32

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

#endif // End of OS Specific function implementations


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
	if(read_amount < *p_buffer_size)
	{
		fprintf(
			stderr,
			"fread_alloc_all() Failed to read all %lu bytes.\n",
			*p_buffer_size
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
	if((NULL == p_file) || (NULL == p_needle) || (NULL == p_needle_size))
	{
		goto END;
	}
	if(0u >= *p_needle_size)
	{
		goto END;
	}

	current_size = *p_needle_size;
	p_result = fread_alloc_all(p_file, &current_size);
	if((NULL == p_result) || (*p_needle_size != current_size))
	{
		goto END;
	}

	uint8_t* p_pattern = NULL;
	size_t next_size = 0u;
	while (NULL == p_pattern)
	{
		p_pattern = memmem(
			p_result, current_size, p_needle, *p_needle_size
		);
		if(NULL != p_pattern)
		{
			break;
		}
		p_pattern = realloc(p_result, current_size + *p_needle_size);
		if(NULL == p_pattern)
		{
			break;
		}
		p_result = p_pattern;
		p_pattern = NULL;
		next_size = fread_all(p_file, &(p_result[current_size]), *p_needle_size);
		if(0u >= next_size)
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
	if(NULL == p_file)
	{
		goto END;
	}
	const char* const p_needle = "\n";
	const size_t needle_len = strnlen(p_needle, INT_MAX);
	if((INT_MAX <= needle_len) || (0 >= needle_len))
	{
		// Should never happen but just in case.
		goto END;
	}
	alloc_size = needle_len;
	p_result = fread_alloc_until(p_file, p_needle, &alloc_size);
	if(NULL != p_result)
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
	if (true == follow_links)
	{
		stat_result = stat(p_path, &path_stat);
	}
	else
	{
		stat_result = lstat(p_path, &path_stat);
	}
	if (0 != stat_result)
	{
		perror("path_is_dir() call to stat() failed");
		goto END;
	}
	result = S_ISDIR(path_stat.st_mode);
#else
	//!TODO
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
	if (true == follow_links)
	{
		stat_result = stat(p_path, &file_stat);
	}
	else
	{
		stat_result = lstat(p_path, &file_stat);
	}
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
	else if(0 <= file_stat.st_size)
	{
		result = (size_t)file_stat.st_size;
	}
#else
	//!TODO Adds support for other OSs(?)
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
		fprintf(p_file, "+");
	}
	fprintf(p_file, " %s", p_path);
#else
	//!TODO
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
#ifdef __linux__
	result = file_clone_data_l(p_source_path, p_sink_path, follow_links);
#elif defined _WIN32
	result = file_clone_data_w(p_source_path, p_sink_path, follow_links);
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
#ifdef __linux__
	result = file_clone_meta_l(p_source_path, p_sink_path, follow_links);
#elif defined _WIN32
	result = file_clone_meta_w(p_source_path, p_sink_path, follow_links);
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
	const bool data_result = file_clone_data(
		p_source_path, p_sink_path, follow_links
	);
	if (true != data_result)
	{
		goto END;
	}
	result = file_clone_meta(p_source_path, p_sink_path, follow_links);
END:
	return result;
}

bool for_each_file_4(const char* const p_path,
	const for_file_handler p_handler,
	const bool handle_dirs, const bool recursive)
{
	bool result = false;
	if ((NULL == p_path) || (NULL == p_handler))
	{
		goto END;
	}
#ifdef __linux__
	result = for_each_file_l(p_path, p_handler, handle_dirs, recursive);
	goto END;
#elif defined _WIN32
	result = for_each_file_w(p_path, p_handler, handle_dirs, recursive);
	goto END;
#else
	#warning Unsupported Operating System.
#endif // End call OS specific function
END:
	return result;
}
inline bool for_each_file_3(const char* const p_path,
	const for_file_handler p_handler, const bool handle_dirs)
{
	// Default value for bool recursive(false).
	return for_each_file_4(p_path, p_handler, handle_dirs, false);
}
inline bool for_each_file(const char* const p_path,
	const for_file_handler p_handler)
{
	// Default value for bool handle_dirs(false).
	return for_each_file_3(p_path, p_handler, false);
}
