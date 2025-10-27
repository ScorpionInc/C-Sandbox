/* si_io_windows.h
 * Language: C
 * Purpose: Defines function(s) for working with file descriptors in Windows.
 * Created: 20251008
 * Updated: 20251008
//*/

#ifndef SI_IO_WINDOWS_H
#define SI_IO_WINDOWS_H

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#ifdef _WIN32

#include <io.h> // _fileno(), _get_osfhandle()
#include <windows.h> // FILE_ATTRIBUTE_DIRECTORY, HANDLE, WIN32_FIND_DATA, ect.
#include <fileapi.h> // LockFileEx(), UnlockFileEx()
#include <shlobj.h> // IShellLinkA
#include <shlguid.h> // CLSID_ShellLink

typedef HANDLE si_file_desc_t;

#endif//_WIN32

#include <errno.h> // errno, perror()
#include <limits.h> // INT_MAX
#include <stdarg.h> // ...
#include <stdbool.h> // bool, false, true
#include <stdio.h> // FILE
#include <stdlib.h> // free()
#include <string.h> // strcmp()
#include <time.h> // timespec

#ifdef _WIN32

#define si_flock(p_file) \
	(void)LockFileEx(get_handle_from_file(p_file), LOCKFILE_EXCLUSIVE_LOCK, \
	0, MAXDWORD, MAXDWORD, &((OVERLAPPED){0}))
#define si_funlock(p_file) \
	(void)UnlockFileEx(get_handle_from_file(p_file), \
	0, MAXDWORD, MAXDWORD, &((OVERLAPPED){0}))

/** Doxygen
 * @brief Get the unbuffered OS specific HANDLE from the C standard FILE type.
 * 
 * @param p_file Pointer to a FILE to get the handle of.
 * 
 * @return Returns FILE HANDLE on success. Returns INVALID_HANDLE otherwise.
 */
HANDLE get_handle_from_file(FILE* const p_file);

/** Doxygen
 * @brief Determines if the file at the target path is a shortcut file.
 * 
 * @param p_path C string to be checked.
 * 
 * @return Returns BOOL TRUE on success. Returns FALSE otherwise.
 */
BOOL path_is_shortcut(const char* const p_path);

/** Doxygen
 * @brief Resolve path to linked target if path is to a shortcut file.
 * 
 * @param p_path C String to be expanded.
 * 
 * @return Returns a heap C string on success. Returns NULL otherwise.
 */
char* follow_path(const char* const p_path);

/** Doxygen
 * @brief Windows specific implementation of file_clone_data().
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
 * @brief Windows specific implementation of the file_clone_meta() function.
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
 * @brief Windows file copy at one path to a file at a different path
 *        maintaining data and metadata while optionally following links.
 * 
 * @param p_source_path C string file path of file to read.
 * @param p_sink_path C string file path of file to write.
 * @param follow_links stdbool flag specifying if links should be followed.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool file_clone_to_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links);

// typedef for support of OS specific information(filepath, WIN32_FIND_DATA)
typedef bool (*for_file_handler)(
	const char* const, WIN32_FIND_DATA* const, void* const
);
#define SI_IO_WINDOWS_DEFAULT_HANDLE_DIRS (false)
#define SI_IO_WINDOWS_DEFAULT_RECURSIVE   (true )

/** Doxygen
 * @brief Windows specific implementation of a for_each loop file iterator.
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

#endif//_WIN32

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_IO_WINDOWS_H