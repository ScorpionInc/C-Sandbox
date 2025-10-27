/* si_io.h
 * Language: C
 * Purpose: Defines function(s)/type(s) for working with files cross-platform.
 * Created: 20250901
 * Updated: 20250904
//*/

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#include "si_io_posix.h" // file_clone_data_np(), file_clone_meta_np(), ect.
#include "si_io_windows.h" // file_clone_data_np(), file_clone_meta_np(), ect.
#include "si_strings.h" // strv_clone_concat(), str_format()

#if (defined(__linux__) || defined(_WIN32))
#define SI_OS_IS_SUPPORTED
#else
#define SI_OS_IS_NOT_SUPPORTED
#endif// Is this OS supported check

#include <errno.h> // errno, perror()
#include <limits.h> // INT_MAX
#include <poll.h> // pollfd
#include <stdarg.h> // ...
#include <stdbool.h> // bool, false, true
#include <stdio.h> // FILE
#include <stdlib.h> // free()
#include <string.h> // strcmp()
#include <time.h> // timespec

#ifndef SI_IO_H
#define SI_IO_H

/** Doxygen
 * @brief Variadic function that locks and unlocks a FILE for exclusive
 *        reading/writing before and after printing respectively.
 * 
 * @param p_file Pointer to FILE to be printed to.
 * @param p_format Format C string.
 * @param arg_list Variadic optional arguments list.
 */
void vfprintf_exclusive(FILE* const p_file, const char* const p_format,
	va_list arg_list);
void  fprintf_exclusive(FILE* const p_file, const char* const p_format, ...);

/** Doxygen
 * @brief Determines if the FILE has data available to be read.
 * 
 * @param p_file Pointer to FILE to check for available data.
 * 
 * @return Returns stdbool true if data is available. Otherwise returns false.
 */
bool fcan_read(const FILE* const p_file);

/** Doxygen
 * @brief Handles partial writes blocking until all data is written or error.
 * 
 * @param p_file Pointer to FILE to be written to.
 * @param p_data Pointer to the data to be written/read from.
 * @param data_size size_t count of bytes to be written from p_data.
 * 
 * @return Returns the number of bytes written. Error if less than data_size.
 */
size_t fwrite_all(FILE* const p_file,
	const void* const p_data, const size_t data_size);

/** Doxygen
 * @brief Handles partial reads by blocking until amount is read or error.
 * 
 * @param p_file Pointer to the FILE to read from.
 * @param p_buffer Pointer to the buffer to write the read values into.
 * @param buffer_size Number of bytes to be read from the file into the buffer.
 * 
 * @return Returns the number of bytes read. Error if less than buffer_size.
 */
size_t fread_all(FILE* const p_file,
	const void* const p_buffer, const size_t buffer_size);

/** Doxygen
 * @brief Allocates a heap buffer of size and fills values from FILE pointer.
 * 
 * @param p_file Pointer to FILE to read from.
 * @param buffer_size Initially stores the amount to read from the FILE into
 *                    the buffer. Afterwords, stores the results of bytes read.
 * 
 * @return Returns heap buffer pointer on success or on a partial read. Returns
 *         a NULL pointer value otherwise.
 */
void* fread_alloc_all(FILE* const p_file, size_t* const p_buffer_size);

/** Doxygen
 * @brief Allocates growing heap buffer of input from file until pattern found.
 * 
 * @param p_file Pointer to FILE to read from.
 * @param p_needle Byte pattern to find in input (not truncated, in buffer)
 * @param p_needle_size Initially its size of needle, after holds result size.
 * 
 * @return Returns heap buffer pointer on success/partial(EOF) or returns NULL.
 */
void* fread_alloc_until(FILE* const p_file,
	const uint8_t* const p_needle, size_t* const p_needle_size);

/** Doxygen
 * @brief Allocates growing heap buffer from file until new line char is found.
 * 
 * @param p_file Pointer to FILE to read from.
 * @param p_size Optional pointer to be set to the size of the line read.
 * 
 * @return Returns heap buffer pointer on success/partial(EOF) or returns NULL.
 */
char* fread_alloc_line(FILE* const p_file, size_t* const p_size);

/** Doxygen
 * @brief Prompts user for input via stdin.
 * 
 * @param p_format (Optional) Formatter C string to send to stdout before read.
 * 
 * @return Returns heap C string on success. Otherwise returns NULL.
 */
char* prompt(const char* const p_format, ...);

/** Doxygen
 * @brief Determines if file type of file object at file path is a directory.
 * 
 * @param p_path C string of file path of file object to be checked.
 * @param follow_links stdbool flag specifying if links should be followed.
 */
bool path_is_dir(const char* const p_path, const bool follow_links);

/** Doxygen
 * @brief Determines the file size of file object at a file path.
 * 
 * @param p_path C string of the file path of file object to check.
 * @param follow_links stdbool flag specifying if links should be followed.
 * @param recursive stdbool flag should get size of all files in and below dir?
 * 
 * @return Returns a size_t of the byte size on success. Directories return 0u
 *         if not recursive. Returns SIZE_MAX if not successful.
 */
size_t path_file_size_3(const char* const p_path, const bool follow_links,
	const bool recursive);
size_t path_file_size(const char* const p_path, const bool follow_links);

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


#ifdef SI_OS_IS_NOT_SUPPORTED

// Basic information(filepath)
typedef bool (*for_file_handler)(const char* const);

#endif//SI_OS_IS_NOT_SUPPORTED
#define SI_IO_DEFAULT_HANDLE_DIRS (false)
#define SI_IO_DEFAULT_RECURSIVE   (true )

/** Doxygen
 * @brief Runs function on each file object in a path optionally recursive.
 * 
 * @param p_path C string file path to directory to search.
 * @param p_handler Function address of type for_file_handler to be called.
 * @param p_param (Optional) Parameter value passed to handler function on call
 * @param handle_dirs stdbool flag determines if directories are passed to the
 *                    handler function or not.
 * @param recursive stdbool flag determines if subfolders are enumerated.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool for_each_file_5(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs, const bool recursive);
bool for_each_file_4(const char* const p_path,
	const for_file_handler p_handler, void* const p_param,
	const bool handle_dirs);
bool for_each_file_3(const char* const p_path,
	const for_file_handler p_handler, void* const p_param);
bool for_each_file(const char* const p_path,
	const for_file_handler p_handler);

#endif // SI_IO_H

#ifdef __cplusplus
}
#endif //__cplusplus
