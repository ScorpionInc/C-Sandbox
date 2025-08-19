/* si_signals.h
//*/

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

#endif//SI_SIGNALS_H
