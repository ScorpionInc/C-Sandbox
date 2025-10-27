/* si_io_posix.h
 * Language: C
 * Purpose: Defines function(s) for working with file descriptors in POSIX.
 * Created: 20251008
 * Updated: 20251008
//*/

#ifndef SI_IO_POSIX
#define SI_IO_POSIX

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "si_strings.h" // strv_clone_concat(), vstr_format()

#ifdef __linux__

#ifndef _POSIX_C_SOURCE// O_NOFOLLOW
#define _POSIX_C_SOURCE (200809L)
#endif//_POSIX_C_SOURCE

#ifndef _DIRENT_HAVE_D_TYPE

#ifndef _BSD_SOURCE
#define _BSD_SOURCE (1)
#endif//_BSD_SOURCE

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE (1)
#endif//_DEFAULT_SOURCE

#endif//_DIRENT_HAVE_D_TYPE

// Requires linking -lacl
#include <acl/libacl.h> // acl_t, ACL_TYPE_ACCESS
#include <dirent.h> // DT_DIR, opendir(), readdir()
#include <fcntl.h> // O_RDONLY, O_WRONLY, O_CREAT, O_NOFOLLOW
#include <poll.h> // pollfd
#include <sys/acl.h> // acl_get_file(), acl_free()
#include <sys/sendfile.h> // sendfile()
#include <sys/stat.h> // stat, stat(), lstat()
#include <sys/types.h> // DIR, struct dirent
#include <unistd.h> // close()
#include <wordexp.h> // wordexp()

typedef int si_file_desc_t;

#endif//__linux__

#include <errno.h> // errno, perror()
#include <limits.h> // INT_MAX
#include <stdarg.h> // ...
#include <stdbool.h> // bool, false, true
#include <stdio.h> // FILE
#include <stdlib.h> // free()
#include <string.h> // strcmp()
#include <time.h> // timespec

#ifndef _GNU_SOURCE
void* memmem(const void* p_haystack, const size_t haystack_len,
	const void* p_needle, const size_t needle_len);
#endif// Add support for used GNU functions.

#ifdef __linux__

#define si_flock(p_file) flockfile(p_file)
#define si_funlock(p_file) funlockfile(p_file)

/** Doxygen
 * @brief Variadic function that locks and unlocks a FILE for exclusive
 *        reading/writing before and after printing respectively.
 * 
 * @param p_file Pointer to FILE to be printed to.
 * @param p_format Format C string.
 * @param arg_list Variadic optional arguments list.
 */
void vfprintf_exclusive_np(FILE* const p_file, const char* const p_format, va_list arg_list);
void  fprintf_exclusive_np(FILE* const p_file, const char* const p_format, ...);

/** Doxygen
 * @brief Determines if a provided acl pointer is basic or extended.
 * 
 * @param p_acl Pointer to an access control struct to be examined.
 * 
 * @return Returns stdbool true if ACL is basic. Returns false otherwise.
 */
bool acl_is_basic(const acl_t p_acl);

/** Doxygen
 * @brief Expand any POSIX-shell specific values in a provided file path.
 * 
 * @param p_path C String to be expanded.
 * 
 * @return Returns a heap C string on success. Returns NULL otherwise.
 */
char* shell_expand_path_l(const char* const p_path);

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
 * @brief Determines if the file descriptor has data available to be read.
 * 
 * @param file_d File descriptor int to check for available data.
 * 
 * @return Returns stdbool true if data is available. Otherwise returns false.
 */
bool fd_can_read(const int file_d);

/** Doxygen
 * @brief Writes all buffer data to a file by it's descriptor.
 * 
 * @param file_d File descriptor int to write to.
 * @param p_data Pointer to the data to be written.
 * @param data_size Number of bytes in the buffer to write.
 * 
 * @return Returns amount written. Error if less than data_size.
 */
size_t fd_write_all(const int file_d,
	const uint8_t* const p_data, const size_t data_size);

/** Doxygen
 * @brief Writes formatted string data to a file descriptor.
 * 
 * @param file_d File descriptor int to write to.
 * @param p_format Format C string to be used.
 * @param ... Variadic arguments list.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
*/
bool fd_printf(const int file_d, const char* const p_format, ...);

/** Doxygen
 * @brief Reads all buffer data from a file by it's descriptor.
 * 
 * @param file_d File descriptor int to read from.
 * @param p_data Pointer to the buffer to hold the read data.
 * @param data_size Size fo the read buffer in bytes.
 * 
 * @return Returns amount read in. Error if less than data_size.
 */
size_t fd_read_all(const int file_d,
	uint8_t* const p_data, const size_t data_size);

/** Doxygen
 * @brief Allocates a heap buffer of size and fills values from file descriptor.
 * 
 * @param file_d File descriptor int to read from.
 * @param buffer_size Initially stores the amount to read from the file into
 *                    the buffer. Afterwords, stores the results of bytes read.
 * 
 * @return Returns heap buffer pointer on success or on a partial read. Returns
 *         a NULL pointer value otherwise.
 */
char* fd_read_alloc_all(const int file_d, size_t* const p_size);

/** Doxygen
 * @brief Allocates growing heap buffer of input from file until pattern found.
 * 
 * @param file_d File descriptor int to read from.
 * @param p_needle Byte pattern to find in input (not truncated, in buffer)
 * @param p_needle_size Initially its size of needle, after holds result size.
 * 
 * @return Returns heap buffer pointer on success/partial(EOF) or returns NULL.
 */
char* fd_read_alloc_until(const int file_d,
	const uint8_t* const p_needle, size_t* const p_needle_size);

/** Doxygen
 * @brief Allocates growing heap buffer from file until new line char is found.
 * 
 * @param file_d File descriptor int to read from.
 * @param p_size Optional pointer to be set to the size of the line read.
 * 
 * @return Returns heap buffer pointer on success/partial(EOF) or returns NULL.
 */
char* fd_read_alloc_line(const int file_d, size_t* const p_size);

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

/** Doxygen
 * @brief Linux specific implementation of file_clone_data().
 * 
 * @param p_source_path C string file path of file containing data to be read.
 * @param p_sink_path C string file path of file to be overwritten/created.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_data_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 * @brief Linux specific implementation of the file_clone_meta() function.
 * 
 * @param p_source_path C string file path of file containing metadata to read.
 * @param p_sink_path C string file path of file to be modified.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_meta_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

/** Doxygen
 * @brief Linux file copy at one path to a file at a different path maintaining
 *        data and metadata while optionally following links.
 * 
 * @param p_source_path C string file path of file to read.
 * @param p_sink_path C string file path of file to write.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_to_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

// typedef for support of OS specific information(filepath, dirent, param)
typedef bool (*for_file_handler)(
	const char* const, struct dirent* const, void* const
);
#define SI_IO_POSIX_DEFAULT_HANDLE_DIRS (false)
#define SI_IO_POSIX_DEFAULT_RECURSIVE   (true )

/** Doxygen
 * @brief Linux specific implementation of a for_each loop file iterator.
 * 
 * @param p_path C string file path to be iterated.
 * @param p_handler Function handler to be called on each file resolved.
 * @param p_param (Optional) Pointer to be passed to the handler function.
 * @param handle_dirs stdbool flag specifying if handler should be ran on dirs.
 * @param recursive stdbool flag specifying if subdirs should also be iterated.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool for_each_file_np_5(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs, const bool recursive);
bool for_each_file_np_4(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs);
bool for_each_file_np_3(const char* const p_path,
	const for_file_handler p_handler, void* const p_param);
bool for_each_file_np(const char* const p_path,
	const for_file_handler p_handler);

#endif//__linux__

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_IO_POSIX