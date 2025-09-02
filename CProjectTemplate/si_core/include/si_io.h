/* si_io.h
 */

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __linux__

// Requires linking -lacl
#include <acl/libacl.h> // acl_t, ACL_TYPE_ACCESS
#include <errno.h> // errno
#include <fcntl.h> // O_RDONLY, O_WRONLY, O_CREAT
#include <sys/acl.h> // acl_get_file(), acl_free()
#include <sys/sendfile.h> // sendfile()
#include <sys/stat.h> // stat, stat(), lstat()
#include <unistd.h> // close()

#else
#endif//__linux__

#ifdef __linux__

/** Doxygen
 */
bool acl_is_basic(const acl_t const p_acl);

/** Doxygen
 */
bool path_has_acl(const char* const p_path);

/** Doxygen
 */
bool fd_has_acl(const int file_d);

/** Doxygen
 */
void mode_t_fprint(FILE* const p_file, const mode_t mode);

/** Doxygen
 */
void path_permissions_fprint(FILE* const p_file, const char* const p_path);

#endif//__linux__

/** Doxygen
 */
bool file_clone_data(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 */
bool file_clone_meta(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 */
bool file_clone_to(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);
