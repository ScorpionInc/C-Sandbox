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
