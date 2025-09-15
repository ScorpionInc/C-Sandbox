//si_signals.c
#include "si_signals.h"

void fprint_signal_debug(FILE* const p_file)
{
	if(NULL == p_file)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < SI_SIGNALS_MAX; iii++)
	{
		struct sigaction query = {0};
		const int query_result = sigaction(iii, NULL, &query);
		if(0 > query_result)
		{
			continue;
		}
		const char* p_sig_desc = strsignal(iii);
		if(SIG_DFL == query.sa_handler)
		{
			fprintf(p_file, "[%24s][%2lu] Default signal handler\n",
				p_sig_desc, iii
			);
		}
		else if (SIG_IGN == query.sa_handler)
		{
			fprintf(p_file, "[%24s][%2lu] Ignored signal\n", p_sig_desc, iii);
		}
		else
		{
			fprintf(p_file, "[%24s][%2lu] Custom handler: %p\n",
				p_sig_desc, iii, query.sa_handler
			);
		}
	}
END:
	return;
}

bool set_exit_signal_handler(__sighandler_t p_handler)
{
	bool result = false;
	if(NULL == p_handler)
	{
		goto END;
	}
	const int exit_signals[] = {
		SIGTERM,
		//SIGKILL,// Cannot be caught
		SIGINT,
		SIGQUIT,
		SIGABRT,
		SIGHUP
	};
	const size_t exit_count = sizeof(exit_signals) / sizeof(int);

	struct sigaction ssa = {0};
    ssa.sa_handler = p_handler;
    sigemptyset(&ssa.sa_mask);
	ssa.sa_flags = 0;

	result = true;
	for(size_t iii = 0u; iii < exit_count; iii++)
	{
		const int next_sig = exit_signals[iii];
		const int register_result = sigaction(next_sig, &ssa, NULL);
		if(0 != register_result)
		{
			fprintf(
				stderr,
				"Failed to set_exit_signal_handler() on: %s.\n",
				strsignal(next_sig)
			);
			result = false;
			break;
		}
	}
END:
	return result;
}
