// si_logger_level.c
#include "si_logger_level.h"

#ifdef __linux__
void fprint_stacktrace_l(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix)
{
	if (NULL == p_file)
	{
		goto END;
	}
	const char* p_mut_prefix = "";
	if (NULL != p_prefix)
	{
		p_mut_prefix = p_prefix;
	}
	if (__INT_MAX__ <= skip_count)
	{
		goto END;
	}
	const size_t STACK_BUFFER_SIZE = (
		(size_t)MAX_CALLSTACK_DEPTH * sizeof(void*)
	);
	void* p_addresses = NULL;
	p_addresses = calloc(1u, STACK_BUFFER_SIZE);
	if (NULL == p_addresses)
	{
		goto END;
	}
	const int bt_result = backtrace(p_addresses, MAX_CALLSTACK_DEPTH);
	if (0 > bt_result)
	{
		free(p_addresses);
		p_addresses = NULL;
		goto END;
	}
	char** p_stack_strs = backtrace_symbols(p_addresses, bt_result);
	if (NULL == p_stack_strs)
	{
		free(p_addresses);
		p_addresses = NULL;
		goto END;
	}
	for (int iii = (int)skip_count; iii < bt_result; iii++)
	{
		fprintf_exclusive(p_file, "%s%s\n", p_mut_prefix, p_stack_strs[iii]);
	}
	if (bt_result >= MAX_CALLSTACK_DEPTH)
	{
		fprintf_exclusive(p_file, "%s(callstack truncated)\n", p_mut_prefix);
	}
	free(p_stack_strs);
	p_stack_strs = NULL;
	free(p_addresses);
	p_addresses = NULL;
END:
	return;
}
#elif defined(_WIN32)
void fprint_stacktrace_w(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix)
{
	if ((NULL == p_file) || (INT_MAX <= skip_count))
	{
		goto END;
	}
	const char* p_mut_prefix = "";
	if (NULL != p_prefix)
	{
		p_mut_prefix = p_prefix;
	}

	HANDLE current_process = GetCurrentProcess();
	HANDLE current_thread = GetCurrentThread();
	if ((INVALID_HANDLE_VALUE == current_process) ||
	    (INVALID_HANDLE_VALID == current_thread))
	{
		goto END;
	}
	SymInitialize(current_process, NULL, TRUE);
	SymSetOptions(SYMOPT_LOAD_LINES);
	CONTEXT context = {0};
	RtlCaptureContext(&context);

	STACKFRAME64 stack_frame = {0};
	stack_frame.AddrPC.Offset = context.Rip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context.Rsp;
	stack_frame.AddrStack.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context.Rbp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;

	bool stack_walked = false;
	for (int iii = 0; iii < skip_count; iii++)
	{
		stack_walked = StackWalk64(
			IMAGE_FILE_MACHINE_AMD64, process, thread, &stack_frame, &context,
			NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL
		);
		if ((true != stack_walked) || (0 == stack_frame.AddrPC.Offset))
		{
			goto CLEAN;
		}
	}
	for (int iii = 0; iii < MAX_CALLSTACK_DEPTH; iii++)
	{
		stack_walked = StackWalk64(
			IMAGE_FILE_MACHINE_AMD64, process, thread, &stack_frame, &context,
			NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL
		);
		if ((true != stack_walked) || (0 == stack_frame.AddrPC.Offset))
		{
			goto CLEAN;
		}
		{
		const size_t symbol_buffer_size = (
			sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)
		);
		char symbol_buffer[symbol_buffer_size];
		memset(symbol_buffer, 0x00 symbol_buffer_size);

		PSYMBOL_INFO p_symbol = (PSYMBOL_INFO)symbol_buffer;
		p_symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		p_symbol->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0L;
		const bool got_symbol = SymFromAddr(
			current_process, stack_frame.AddrPC.Offset, &displacement, p_symbol
		);
		if (true == got_symbol)
		{
			// Print call stack with symbol + offset
			fprintf_exclusive(
				p_file, "[%d] %s + 0x%llx\n",
				iii, p_symbol->Name, displacement
			);
		}
		else
		{
			// Print call stack with unknown/unnamed symbol.
			fprintf_exclusive(
				p_file, "[%d] (Unknown Symbol) + 0x%llx\n",
				iii, stack_frame.AddrPC.Offset
			);
		}
		}
	}
CLEAN:
	SymCleanup(current_process);
END:
	return;
}
#endif// OS Specific implementations

inline void fprint_stacktrace_3(FILE* const p_file,
	const size_t skip_count, const char* const p_prefix)
{
	// Calls implementation based upon OS
#ifdef __linux__
	fprint_stacktrace_l(p_file, skip_count, p_prefix);
#elif defined(_WIN32)
	fprint_stacktrace_w(p_file, skip_count, p_prefix);
#endif
}
void fprint_stacktrace_2(FILE* const p_file,
	const size_t skip_count)
{
	// Default value of p_prefix is NULL.
	fprint_stacktrace_3(p_file, skip_count, NULL);
}
inline void fprint_stacktrace(FILE* const p_file)
{
	// Default value of skip_count is (1u)
	fprint_stacktrace_2(p_file, 1u);
}

char* si_logger_level_generate_header(si_logger_level_t* const p_level,
	const int padding, const size_t msg_level)
{
	char* p_result = NULL;
	if (NULL == p_level)
	{
		goto END;
	}
	if (NULL == p_level->p_file)
	{
		goto END;
	}
	size_t result_size = 0u;
	p_result = char_to_string('[');
	if (NULL == p_result)
	{
		goto END;
	}
	result_size++;
	const bool is_ansi = si_tui_is_ansi(p_level->p_file);
	if ((true == is_ansi) && (NULL != p_level->p_header_ansi))
	{
		result_size = str_lgrow_concat(&p_result, p_level->p_header_ansi);
		if (SIZE_MAX == result_size)
		{
			free(p_result);
			p_result = NULL;
			goto END;
		}
	}
	char* p_header = NULL;
	const bool is_level = (p_level->log_level == msg_level);
	if ((true == is_level) && (NULL != p_level->p_header))
	{
		// Use p_level->p_header as header value.
		p_header = str_clone_pad(p_level->p_header, padding);
	}
	else
	{
		// Use numerical value of custom msg_level as header.
		p_header = unsigned_to_string_3(
			&msg_level, sizeof(msg_level), padding
		);
	}
	if (NULL == p_header)
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
	result_size = str_lgrow_concat(&p_result, p_header);
	free(p_header);
	p_header = NULL;
	if (SIZE_MAX == result_size)
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
	if ((true == is_ansi) && (NULL != p_level->p_header_ansi))
	{
		result_size = str_lgrow_concat(&p_result, "\x1b[0m");
		if (SIZE_MAX == result_size)
		{
			free(p_result);
			p_result = NULL;
			goto END;
		}
	}
	result_size = str_lgrow_concat(&p_result, "]: ");
	if (SIZE_MAX == result_size)
	{
		free(p_result);
		p_result = NULL;
		goto END;
	}
END:
	return p_result;
}

void si_logger_level_fprint_header(si_logger_level_t* const p_level,
	const int padding, const size_t msg_level)
{
	if (NULL == p_level)
	{
		goto END;
	}
	if (NULL == p_level->p_file)
	{
		goto END;
	}

	char* p_header = si_logger_level_generate_header(
		p_level, padding, msg_level
	);
	if (NULL == p_header)
	{
		goto END;
	}
	fprintf(p_level->p_file, "%s", p_header);
	free(p_header);
	p_header = NULL;
END:
	return;
}
