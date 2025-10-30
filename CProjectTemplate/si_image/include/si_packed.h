/** si_packed.h
 * Language: C
 * Purpose : Macros for managing packing structs and testing for support.
 * Created : 20251030
 * Updated : 20251030
 */

// Trys supporting packed structs if possible
#if defined(__GNUC__) || defined(__clang__)
#	define SI_PACKED_SUPPORT
#	define SI_PACKED_IS_SUPPORTED (1)
// Uses an inline attribute
#	define SI_PACKED_PRE
#	define SI_PACKED_INLINE __attribute__((packed))
#	define SI_PACKED_POST
#elif defined(_MSC_VER)
#	define SI_PACKED_SUPPORT
#	define SI_PACKED_IS_SUPPORTED (1)
// Uses a settings stack
#	define SI_PACKED_PRE #pragma pack(push, 1)
#	define SI_PACKED_INLINE
#	define SI_PACKED_POST #pragma pack(pop)
#else
#	ifdef SI_PACKED_SUPPORT
#		undef SI_PACKED_SUPPORT
#	endif// SI_PACKED_SUPPORT
#	define SI_PACKED_IS_SUPPORTED (0)
// All are blank meaning no struct packing supported
#	define SI_PACKED_PRE
#	define SI_PACKED_INLINE
#	define SI_PACKED_POST
#endif// End of compiler check(s)

/* Example Code:

SI_PACKED_PRE
typedef struct SI_PACKED_INLINE example_t
{
	int32_t number;
	uint8_t bit_flag : 1;
} example_t;
SI_PACKED_POST

 */
