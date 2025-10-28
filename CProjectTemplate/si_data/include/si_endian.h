/* si_endian.h
 * Language: C
 * Purpose: Cross-platform support for endianess function(s)/type(s)
 * Created: 20151027
 * Updated: 20251027
//*/

#ifdef __linux__

#include <endian.h>

#elif defined(_WIN32)

#include <winsock2.h>

// I couldn't find a Windows equivalent to endian.h
// ABCD
#define LITTLE_ENDIAN 0x41424344UL
// DCBA
#define BIG_ENDIAN 0x44434241UL
// BADC
#define PDP_ENDIAN 0x42414443UL
#define BYTE_ORDER ('ABCD')

#else
#warning Unknown/Unsupported OS
#endif// OS Specific includes/defines

#ifndef SI_ENDIAN_H
#define SI_ENDIAN_H

#endif//SI_ENDIAN_H
