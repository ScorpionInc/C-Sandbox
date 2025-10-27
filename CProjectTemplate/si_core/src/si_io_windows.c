// si_io_windows.c
#include "si_io_windows.h"

#ifdef _WIN32

BOOL path_is_shortcut(const char* const p_path)
{
	BOOL result = FALSE;
	if (NULL == p_path)
	{
		goto END;
	}

	// Uses the single threaded synchronized STA flag.
	HRESULT h_result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(h_result))
	{
		// Failed to initialize COM object
		goto END;
	}

	IShellLinkA* p_shell_link = NULL;
	h_result = CoCreateInstance(
		CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLinkA, (LPVOID*)&p_shell_link
	);
	if (FAILED(h_result))
	{
		// Failed to initialize ShellLink Object
		CoUninitialize();
		goto END;
	}

	IPersistFile* p_persist_f = NULL;
	h_result = p_shell_link->QueryInterface(
		IID_IPersistFile, (LPVOID*)&p_persist_f
	);
	if (FAILED(h_result))
	{
		// Failed to get IPersistFile Interface
		p_shell_link->Release();
		CoUninitialize();
		goto END;
	}

	h_result = p_persist_f->Load((LPCSTR)p_path, STGM_READ);
	result = SUCCEEDED(h_result);
	p_persist_f->Release();
	p_shell_link->Release();
	CoUninitialize();
END:
	return result;
}

char* follow_path(const char* const p_path)
{
	char* p_result = NULL;
	if (NULL == p_path)
	{
		goto END;
	}

	// Uses the single threaded synchronized STA flag.
	HRESULT h_result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (FAILED(h_result))
	{
		// Failed to initialize COM object
		goto END;
	}

	IShellLinkA* p_shell_link = NULL;
	h_result = CoCreateInstance(
		CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLinkA, (LPVOID*)&p_shell_link
	);
	if (FAILED(h_result))
	{
		// Failed to initialize ShellLink Object
		CoUninitialize();
		goto END;
	}

	IPersistFile* p_persist_f = NULL;
	h_result = p_shell_link->QueryInterface(
		IID_IPersistFile, (LPVOID*)&p_persist_f
	);
	if (FAILED(h_result))
	{
		// Failed to get IPersistFile Interface
		p_persist_f = NULL;
		goto ERROR;
	}

	h_result = p_persist_f->Load((LPCSTR)p_path, STGM_READ);
	if (FAILED(h_result))
	{
		// Failed to load file as a shortcut. (Might be fine.)
		// p_result expects caller to use a LocalFree() call.
		p_result = StrDupA(p_path);
		goto ERROR;
	}

	// Is a shortcut so we need to resolve it.
	h_result = p_shell_link->Resolve(NULL, SLR_NO_UI);
	if (FAILED(h_result))
	{
		// Is a shortcut but it can't be resolved.
		goto ERROR;
	}

	TCHAR buffer[MAX_PATH] = {0};
	h_result = p_shell_link->GetPath(buffer, MAX_PATH, NULL, 0);
	if (FAILED(h_result))
	{
		// Failed to get the path of the resolved file.
		goto ERROR;
	}

	// p_result expects caller to use a LocalFree() call.
	p_result = StrDupA(buffer);
ERROR:
	if (NULL != p_persist_f)
	{
		p_persist_f->Release();
		p_persist_f = NULL;
	}
	if (NULL != p_shell_link)
	{
		p_shell_link->Release();
		p_shell_link = NULL;
	}
	CoUninitialize();
END:
	return p_result;
}

HANDLE get_handle_from_file(FILE* const p_file)
{
	HANDLE result = INVALID_HANDLE_VALUE;
	if (NULL == p_file)
	{
		goto END;
	}
	const int file_desc = _fileno(p_file);
	if (0 > file_desc)
	{
		goto END;
	}
	result = (HANDLE)_get_osfhandle(file_desc);
END:
	return result;
}

bool file_clone_data_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
	if (true == follow_links)
	{
		// Resolve any shortcuts
		char* p_follow_path = follow_path(p_source_path);
		if (NULL == p_follow_path)
		{
			goto END;
		}
		char* p_target_path = follow_path(p_sink_path);
		if (NULL == p_target_path)
		{
			// Target path is not a shortcut
			result = (bool)CopyFileA(p_follow_path, p_sink_path);
		}
		else
		{
			result = (bool)CopyFileA(p_follow_path, p_target_path);
			LocalFree(p_target_path);
			p_target_path = NULL;
		}
		LocalFree(p_follow_path);
		p_follow_path = NULL;
	}
	else
	{
		result = (bool)CopyFileA(p_source_path, p_sink_path);
	}
END:
	return result;
}

bool file_clone_meta_np(const char* const p_source_path,
	const char* const p_sink_path, const bool follow_links)
{
	bool result = false;
	if ((NULL == p_source_path) || (NULL == p_sink_path))
	{
		goto END;
	}
	HANDLE source_handle = INVALID_HANDLE_VALUE;
	HANDLE sink_handle   = INVALID_HANDLE_VALUE;

	source_handle = CreateFile(
		p_source_path, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (INVALID_HANDLE_VALUE == source_handle)
	{
		// Failed to open source for reading.
		goto END;
	}

	sink_handle = CreateFile(
		p_sink_path, GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	);
	if (INVALID_HANDLE_VALUE == sink_handle)
	{
		// Failed to open sink for writing.
		CloseHandle(source_handle);
		goto END;
	}

	FILETIME ft_create = {0};
	FILETIME ft_access = {0};
	FILETIME ft_write  = {0};
	const BOOL gft_result = GetFileTime(
		source_handle, &ft_create, &ft_access, &ft_write
	);
	if (TRUE != gft_result)
	{
		// Failed to read file times.
		CloseHandle(sink_handle);
		CloseHandle(source_handle);
		goto END;
	}
	const BOOL sft_result = SetFileTime(
		sink_handle, &ft_create, &ft_access, &ft_write
	);
	CloseHandle(sink_handle);
	CloseHandle(source_handle);
	if (TRUE != sft_result)
	{
		// Failed to set file times.
		goto END;
	}

	DWORD attributes = INVALID_FILE_ATTRIBUTES;
	attributes = GetFileAttributesA(p_source_path);
	if (INVALID_FILE_ATTRIBUTES != attributes)
	{
		const BOOL sfa_result = SetFileAttributesA(p_sink_path, attributes);
		if (TRUE != sfa_result)
		{
			// Failed to set attributed
			goto END;
		}
	}

	result = true;
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
		if (INT_MAX <= name_len)
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
				// Skip directories
				free(p_fullpath);
				p_fullpath = NULL;
				find_next_result = FindNextFile(file_handle, &find_data);
				continue;
			}
		}
		// Handle files/links/ect.
		const bool handler_result = p_handler(p_fullpath, &find_data, p_param);
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

#endif//_WIN32