/* si_signals.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines function(s) for working with signals.
 * Created: 20250819
 * Updated: 20250820
//*/

#ifdef __linux__

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif//_XOPEN_SOURCE

#endif//__linux__

#include <signal.h> // signal()
#include <stdbool.h> // bool, true, false
#include <stdio.h> // FILE, fprintf()
#include <string.h> // For strsignal()

#define SI_SIGNALS_MAX (31)

#ifndef SI_SIGNALS_H
#define SI_SIGNALS_H

/** Doxygen
 * @brief Prints some debug information about the current signal state to FILE.
 *
 * @param p_file FILE pointer to be written to.
 */
void fprint_signal_debug(FILE* const p_file);

/** Doxygen
 * @brief Sets multiple exit signal handlers to a provided custom handler.
 * 
 * @param p_handler Pointer to signal handler function to call.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool set_exit_signal_handler(__sighandler_t p_handler);

#endif//SI_SIGNALS_H
