//si_adler.h

#include <endian.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "si_uint_utils.h"

#ifndef SI_ADLER_H
#define SI_ADLER_H

typedef struct si_adler_t
{
	uint8_t* p_hash;
	size_t block_size;
} si_adler_t;


// Largest prime below 2^8
#define ADLER_16_PRIME 251ULL
// Largest prime below 2^16
#define ADLER_32_PRIME 65521ULL
// Largest prime below 2^32
#define ADLER_64_PRIME 4294967291ULL
// Largest prime below 2^64
#define ADLER_128_PRIME 18446744073709551557ULL

/* Doxygen
 * @brief Based upon the target block size of the hash returns prime number.
 *
 * @param block_size Size in bytes of the block size being used.
 *
 * @return Returns ULL int of the largest prime number for half the block size.
 */
unsigned long long si_adler_select_prime(const size_t block_size);

/* Doxygen
 * @brief Initializes le byte array p_bytes to ADLER prime based on block_size
 *
 * @param p_bytes Pointer to byte buffer of half block_size to hold prime value
 * @param block_size Size of the target hash(Not size of p_bytes).
 */
void si_adler_init_prime_le(uint8_t* const p_bytes, const size_t block_size);
/* Doxygen
 * @brief Initializes be byte array p_bytes to ADLER prime based on block_size
 *
 * @param p_bytes Pointer to byte buffer of half block_size to hold prime value
 * @param block_size Size of the target hash(Not size of p_bytes).
 */
void si_adler_init_prime_be(uint8_t* const p_bytes, const size_t block_size);
/* Doxygen
 * @brief Initializes ho byte array p_bytes to ADLER prime based on block_size
 *
 * @param p_bytes Pointer to byte buffer of half block_size to hold prime value
 * @param block_size Size of the target hash(Not size of p_bytes).
 */
void si_adler_init_prime(uint8_t* const p_bytes, const size_t block_size);


/* Doxygen
 * @brief Initializes existing si_adler struct values. (Not new)
 *
 * @param p_bytes Pointer to the si_adler struct to be initialized.
 */
void si_adler_init(si_adler_t* const p_hash);

/* Doxygen
 * @brief Creates and initializes a new heap alder struct.
 *
 * @param p_bytes Pointer to the si_adler struct to be initialized.
 * @param block_size Size in bytes of the resulting hash.
 */
void si_adler_new(si_adler_t* const p_hash, const size_t block_size);

/* Doxygen
 * @brief Handles input_buffer_size of input updating si_adler struct values.
 *
 * @param p_hash Pointer to si_adler struct to hold results.
 * @param p_buffer Pointer to input buffer of input_buffer_size bytes.
 * @param input_buffer_size Number of bytes in the input buffer to be used.
 */
void si_adler_update(si_adler_t* const p_hash, const uint8_t* const p_buffer,
	const size_t input_buffer_size);

/* Doxygen
 * @brief Frees buffers allocated by the new function inside si_adler struct.
 *
 * @param p_hash Pointer to the si_adler struct to have it's values freed.
 */
void si_adler_free(si_adler_t* const p_hash);

/* Doxygen
 * @brief Simple file print of si_adler values used mostly for debugging.
 *
 * @param p_hash Pointer to si_adler struct to read values from.
 * @param p_file Pointer to FILE to write the information to.
 */
void si_adler_fprint(const si_adler_t* const p_hash, FILE* const p_file);

#endif//SI_ADLER_H
