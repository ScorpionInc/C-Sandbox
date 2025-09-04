/* si_io.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s)/type(s) for working with files.
 * Created: 20250901
 * Updated: 20250904
//*/

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <errno.h> // errno
#include <limits.h>
#include <stdbool.h> // bool, false, true
#include <stdio.h> // FILE
#include <stdlib.h> // free()
#include <string.h> // strcmp()
#include <time.h> // timespec

#include "si_strings.h" // strv_clone_concat()

#ifdef __linux__

// Requires linking -lacl
#include <acl/libacl.h> // acl_t, ACL_TYPE_ACCESS
#include <dirent.h> // DT_DIR, opendir(), readdir()
#include <fcntl.h> // O_RDONLY, O_WRONLY, O_CREAT, O_NOFOLLOW
#include <sys/acl.h> // acl_get_file(), acl_free()
#include <sys/sendfile.h> // sendfile()
#include <sys/stat.h> // stat, stat(), lstat()
#include <sys/types.h> // DIR, struct dirent
#include <unistd.h> // close()

// typedef for support of OS specific information(filepath, dirent)
typedef bool (*for_file_handler)(const char* const, struct dirent* const);

#elif defined _WIN32

#include <windows.h> // FILE_ATTRIBUTE_DIRECTORY, HANDLE, WIN32_FIND_DATA, ect.

// typedef for support of OS specific information(filepath, WIN32_FIND_DATA)
typedef bool (*for_file_handler)(const char* const, WIN32_FIND_DATA* const);

#else

// Basic information(filepath)
typedef bool (*for_file_handler)(const char* const);

#endif //OS specific includes/defines


#ifndef SI_IO_H
#define SI_IO_H

// Start of OS specific function prototypes
#ifdef __linux__

/** Doxygen
 * @brief Determines if a provided acl pointer is basic or extended.
 * 
 * @param p_acl Pointer to an access control struct to be examined.
 * 
 * @return Returns stdbool true if ACL is basic. Returns false otherwise.
 */
bool acl_is_basic(const acl_t const p_acl);

/** Doxygen
 * @brief Determines if file object at path has an extended ACL.
 * 
 * @param p_path C string pointer of file path to be checked.
 * 
 * @return Returns stdbool true if ACL. Returns false otherwise.
 */
bool path_has_acl(const char* const p_path);

/** Doxygen
 * @brief Determines if a file descriptor has an extended ACL.
 * 
 * @param file_d File descriptor int to check ACL from.
 * 
 * @return Returns stdbool true if ACL extended. Returns false otherwise.
 */
bool fd_has_acl(const int file_d);

/** Doxygen
 * @brief Prints values of a mode_t struct to a FILE stream.
 * 
 * @param p_file Pointer to FILE to be written to.
 * @param mode mode_t struct to read values from.
 */
void mode_t_fprint(FILE* const p_file, const mode_t mode);

/** Doxygen
 * @brief Prints values of a directory entry to a FILE stream.
 * 
 * @param p_file Pointer to FILE to be written to.
 * @param p_entry Pointer to struct dirent to read values from.
 */
void dirent_fprint(FILE* const p_file, const struct dirent* const p_entry);

#elif defined _WIN32
#else
#endif // End of OS specific function prototypes


/** Doxygen
 * @brief Determines if file type of file object at file path is a directory.
 * 
 * @param p_path C string of file path of file object to be checked.
 * @param follow_links stdbool flag specifying if links should be followed.
 */
bool path_is_dir(const char* const p_path, const bool follow_links);

/** Doxygen
 * @brief Prints permissions of file object at C string path to FILE.
 * 
 * @param p_file Pointer to FILE to be written to.
 * @param p_path C string file path referencing file object to read perms from.
 */
void path_perms_fprint(FILE* const p_file, const char* const p_path);

/** Doxygen
 * @brief Copy data from file at one path to a file at a different path while
 *        optionally following links.
 * 
 * @param p_source_path C string file path of file containing data to be read.
 * @param p_sink_path C string file path of file to be overwritten/created.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_data(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 * @brief Copy metadata from file at one path to a file at a different path
 *        while optionally following links.
 * 
 * @param p_source_path C string file path of file containing metadata to read.
 * @param p_sink_path C string file path of file to be modified.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_meta(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 * @brief Copy file at one path to a file at a different path maintaining data
 *        and metadata while optionally following links.
 * 
 * @param p_source_path C string file path of file to read.
 * @param p_sink_path C string file path of file to write.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_to(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 * @brief Runs function on each file object in a path optionally recursive.
 * 
 * @param p_path C string file path to directory to search.
 * @param p_handler Function address of type for_file_handler to be called.
 * @param handle_dirs stdbool flag determines if directories are passed to the
 *                    handler function or not.
 * @param recursive stdbool flag determines if subfolders are enumerated.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool for_each_file_4(const char* const p_path,
	const for_file_handler p_handler, const bool handle_dirs,
	const bool recursive);
bool for_each_file_3(const char* const p_path,
	const for_file_handler p_handler, const bool handle_dirs);
bool for_each_file(const char* const p_path,
	const for_file_handler p_handler);

#endif // SI_IO_H

#ifdef __cplusplus
}
#endif //__cplusplus
