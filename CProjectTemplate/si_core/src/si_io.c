// si_io.c
#include "si_io.h"

#ifdef __linux__

bool acl_is_basic(const acl_t const p_acl)
{
	bool result = false;
	if(NULL == p_acl)
	{
		goto END;
	}
	const size_t BASIC_ACL_COUNT = 3u;
	acl_entry_t entry = {0};
	int entry_id = ACL_FIRST_ENTRY;
	size_t entry_counter = 0u;
	while(acl_get_entry(p_acl, entry_id, &entry))
	{
		entry_counter++;
		entry_id = ACL_NEXT_ENTRY;
		if(BASIC_ACL_COUNT < entry_counter)
		{
			break;
		}
	}
	result = (BASIC_ACL_COUNT >= entry_counter);
END:
	return result;
}

bool path_has_acl(const char* const p_path)
{
	bool result = false;
	if(NULL == p_path)
	{
		goto END;
	}
	acl_t p_acl = acl_get_file(p_path, ACL_TYPE_ACCESS);
	if(NULL == p_acl)
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
	if(0 > file_d)
	{
		goto END;
	}
	acl_t p_acl = acl_get_fd(file_d);
	if(NULL == p_acl)
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
	if(S_ISBLK(mode))
	{
		result = 'b';
		goto END;
	}
	if(S_ISCHR(mode))
	{
		result = 'c';
		goto END;
	}
	if(S_ISDIR(mode))
	{
		result = 'd';
		goto END;
	}
	if(S_ISFIFO(mode))
	{
		result = 'f';
		goto END;
	}
	if(S_ISLNK(mode))
	{
		result = 'l';
		goto END;
	}
	if(S_ISSOCK(mode))
	{
		result = 's';
		goto END;
	}
END:
	return result;
}

void mode_t_fprint(FILE* const p_file, const mode_t mode)
{
	if(NULL == p_file)
	{
		goto END;
	}
	const size_t PERMISSION_BITS = 3u;
	const size_t PERMISSION_GROUPS = 3u;
	const mode_t group_mask = ((mode_t)-1) >>
		(sizeof(mode_t) * CHAR_BIT - PERMISSION_BITS);
	const char type_char = mode_type_char(mode);
	fprintf(p_file, "%c", type_char);
	for(size_t iii = 0u; iii < PERMISSION_GROUPS; iii++)
	{
		const size_t mask_shift = PERMISSION_BITS *
			(PERMISSION_GROUPS - 1 - iii);
		const mode_t mode_group = (mode &
			(group_mask << mask_shift)
		) >> mask_shift;
		fprintf(p_file, "%s", (mode_group) >= 4 ? "r" : "-");
		fprintf(p_file, "%s", (mode_group % 4) >= 2 ? "w" : "-");
		if((0 == iii) && (mode & S_ISUID))
		{
			fprintf(p_file, "s");
			continue;
		}
		fprintf(p_file, "%s", (mode_group % 2) >= 1 ? "x" : "-");
	}
END:
	return;
}

void path_permissions_fprint(FILE* const p_file, const char* const p_path)
{
	if((NULL == p_file) || (NULL == p_path))
	{
		goto END;
	}
	struct stat file_stat = {0};
	if(lstat(p_path, &file_stat) < 0)
	{
		goto END;
	}
	mode_t_fprint(stdout, file_stat.st_mode);
	const bool has_acl = path_has_acl(p_path);
	if(true == has_acl)
	{
		fprintf(p_file, "+");
	}
	fprintf(p_file, " %s", p_path);
END:
	return;
}
#endif//__linux__

bool file_clone_data(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
#ifdef __linux__
	// Opens file(s) for operation(s)
	struct stat source_stat = {0};
	const int source_flags = O_RDONLY |
		(follow_links ? 0x00 : O_NOFOLLOW);
	int source_fd = open(p_source_path, source_flags);
	if(0 > source_fd)
	{
		goto END;
	}
	const int stat_result = fstat(source_fd, &source_stat);
	if(0 > stat_result)
	{
		goto CLEAN;
	}
	const int sink_flags = (O_WRONLY | O_CREAT | O_TRUNC) |
		(follow_links ? 0x00 : O_NOFOLLOW);
	int sink_fd = open(
		p_sink_path, sink_flags, source_stat.st_mode
	);
	if(0 > sink_fd)
	{
		goto CLEAN;
	}

	// Sends the data creating the file as needed.
	off_t offset = 0;
	ssize_t send_file_result = 1;
	while(0 < send_file_result)
	{
		send_file_result = sendfile(
			sink_fd, source_fd, &offset, source_stat.st_size - offset
		);
		if(0 > send_file_result)
		{
			goto CLEAN;
		}
	}

	result = true;
CLEAN:
	if(0 <= source_fd)
	{
		close(source_fd);
	}
	source_fd = -1;
	if(0 <= sink_fd)
	{
		close(sink_fd);
	}
	sink_fd = -1;
#else
	FILE* p_source = fopen(p_source_path, "r");
	if(NULL == p_source)
	{
		goto END;
	}
	FILE* p_sink = fopen(p_sink_path, "w");
	if(NULL == p_sink)
	{
		goto CLEAN;
	}

	char next_byte = fgetc(p_source);
	while(next_byte != EOF)
	{
		fputc(next_byte, p_sink);
		next_byte = fgetc(p_source);
	}

	result = true;
CLEAN:
	if(NULL != p_source)
	{
		fclose(p_source);
	}
	if(NULL != p_sink)
	{
		fclose(p_sink);
	}
#endif//__linux__
END:
	return result;
}

bool file_clone_meta(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
#ifdef __linux__
	// Opens file(s) for operation(s)
	struct stat source_stat = {0};
	const int stat_result = follow_links ?
		stat(p_source_path, &source_stat) :
		lstat(p_source_path, &source_stat);
	if(0 > stat_result)
	{
		goto END;
	}
	const int sink_flags = O_WRONLY |
		(follow_links ? 0x00 : O_NOFOLLOW);
	int sink_fd = open(
		p_sink_path, sink_flags, source_stat.st_mode
	);
	if(0 > sink_fd)
	{
		goto END;
	}

	// Change File Mode
	const int mode_result = fchmod(sink_fd, source_stat.st_mode);
	if(0 > mode_result)
	{
		goto CLEAN;
	}

	// Change File Owner
	const int owner_result = fchown(
		sink_fd, source_stat.st_uid, source_stat.st_gid
	);
	if(0 > owner_result)
	{
		goto CLEAN;
	}

	// Change File Timestamps
	struct timespec new_times[2] = {0};
	new_times[0] = source_stat.st_atim; // Access
	new_times[1] = source_stat.st_mtim; // Modify
	const int time_result = futimens(sink_fd, new_times);
	if(0 > time_result)
	{
		goto CLEAN;
	}

	// Clone File ACL(s)
	acl_t p_acl = acl_get_file(p_source_path, ACL_TYPE_ACCESS);
	if(NULL == p_acl)
	{
		// This may not be an error if no ACL is defined.
		if(ENODATA == errno)
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
	if(0 <= sink_fd)
	{
		close(sink_fd);
	}
	sink_fd = -1;
#else
	// TODO Other Operating System(s)
#endif//__linux__
END:
	return result;
}

bool file_clone_to(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
	const bool data_result = file_clone_data(
		p_source_path, p_sink_path, follow_links
	);
	if(true != data_result)
	{
		goto END;
	}
	result = file_clone_meta(p_source_path, p_sink_path, follow_links);
END:
	return result;
}
