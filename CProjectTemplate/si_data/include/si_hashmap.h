//si_hashmap.h

#include <stdbool.h>
#include <stdlib.h>

#include "si_adler.h"
#include "si_map.h"
#include "si_realloc_settings.h"

#ifndef SI_HASHMAP_H
#define SI_HASHMAP_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// Each map in the si_hashmap_t->maps array associates a hash value
// with it's associated data pointer.
typedef struct si_hashmap_t
{
	si_array_t maps;
	si_realloc_settings_t* p_settings;
	size_t (*p_hash_f)(const void* const, const size_t);
} si_hashmap_t;

/** Doxygen
 * @brief Initializes values within an already existing si_hashmap_t struct.
 * 
 * @param p_hashmap Pointer to the hashmap struct to be initilized.
 * @param capacity Capacity of the hashmap. Not be be changed after insert.
*/
void si_hashmap_init(si_hashmap_t* const p_hashmap, const size_t capacity);

/** Doxygen
 * @brief Allocates, initializes and then returns a new si_hashmap_t struct.
 * 
 * @param capacity Capacity of the hashmap. Not to be changed after insert.
 * 
 * @return Returns pointer to new struct on success. Returns NULL otherwise.
*/
si_hashmap_t* si_hashmap_new(const size_t capacity);

/** Doxygen
 * @brief Update child data structures with updated reallocation settings
 *        pointer address. Note: Not needed for changes inside settings struct.
 * 
 * @param p_hashmap Pointer to the hashmap to have children settings updated.
 */
void si_hashmap_update_settings(si_hashmap_t* const p_hashmap);

/** Doxygen
 * @brief Walks the structure to determine the # of hash,value pairs stored.
 * 
 * @param p_hashmap Pointer to the si_hashmap_t to be walked.
 * 
 * @return Returns size_t count of items in hashmap.
 */
size_t si_hashmap_count(const si_hashmap_t* const p_hashmap);

/** Doxygen
 * @brief Determines if hash,value pairs have been added or not.
 * 
 * @param p_hashmap Pointer to the hashmap to walk.
 */
bool si_hashmap_is_empty(const si_hashmap_t* const p_hashmap);

/** Doxygen
 * @brief Returns a size_t hash of a data buffer of size for hashmap.
 * 
 * @param p_hashmap Pointer to haskmap to get hash function from.
 * @param p_key Pointer to data buffer to read hash input from.
 * @param key_size Number of bytes in the key buffer to read.
 * 
 * @return Returns size_t hash of input buffer on success. SIZE_MAX otherwise.
 */
size_t si_hashmap_hash(const si_hashmap_t* const p_hashmap,
	const void* const p_key, const size_t key_size);

/** Doxygen
 * @brief Determines the data in the hash map from provided hash input data.
 * 
 * @param p_hashmap Pointer to the hashmap to be read from.
 * @param p_key Pointer to data buffer to generate hash from.
 * @param key_size Number of bytes in the key buffer to be used for hashing.
 * 
 * @return Returns pointer to value on success. Returns NULL otherwise.
 */
void* si_hashmap_at(const si_hashmap_t* p_hashmap, const void* const p_key,
	const size_t key_size);

/** Doxygen
 * @brief Determines the data in the hash map from the provided hash.
 * 
 * @param p_hashmap Pointer to the hashmap to be read from.
 * @param hash Hash value to id the data value.
 * 
 * @return Returns pointer to value on success. Returns NULL otherwise.
 */
void* si_hashmap_at_hash(const si_hashmap_t* p_hashmap, const size_t hash);

/** Doxygen
 * @brief Determines if a key value was used to add a value to the hashmap.
 * 
 * @param p_hashmap Pointer to the hashmap to check for the key value.
 * @param p_key Pointer to the buffer to read the key value from.
 * @param key_size Number of bytes to read from the buffer for the hash(size_t)
 * 
 * @return Returns stdbool true if exists. Returns false otherwise.
 */
bool si_hashmap_has(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size);

/** Doxygen
 * @brief Determines if a hash exists within the pointed at hashmap.
 * 
 * @param p_hashmap Pointer to the hashmap to check.
 * @param hash Hash to be checked for.
 * 
 * @return Returns true if hash was found. Returns false otherwise.
 */
bool si_hashmap_has_hash(si_hashmap_t* const p_hashmap, const size_t hash);

/** Doxygen
 * @brief Finds the hash used for data value, add sets a pointer to it.
 * 
 * @param p_hashmap Pointer to the hashmap to search.
 * @param p_value Data pointer to be searched for in the hashmap.
 * @param pp_hash Pointer to pointer to be set to the hash used for value.
 * 
 * @return Returns stdbool true on success. False otherwise. Sets pp_hash.
 */
bool si_hashmap_find(si_hashmap_t* const p_hashmap, const void* const p_value,
	const size_t** pp_hash);

/** Doxygen
 * @brief Adds a new key/value pair into the pointed at hashmap.
 * 
 * @param p_hashmap Pointer to hashmap to add new pair into.
 * @param p_key Pointer to data to be used to geneterate hash.
 * @param key_size Number of bytes in data buffer to read to hash.
 * @param p_value Data pointer value to be inserted.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_insert(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value);

/** Doxygen
 * @brief Adds a new hash/value pair into the pointed at hashmap.
 * 
 * @param p_hashmap Pointer to hashmap to add new pair into.
 * @param hash Hash value to be inserted. (size_t)
 * @param p_value Data pointer value to be inserted.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_insert_hash(si_hashmap_t* const p_hashmap, const size_t hash,
	const void* const p_value);

/** Doxygen
 * @brief Sets a new value at key in the pointed at hashmap.
 * 
 * @param p_hashmap Pointer to hashmap to add new pair into.
 * @param p_key Pointer to data to be used to geneterate hash.
 * @param key_size Number of bytes in data buffer to read to hash.
 * @param p_value Data pointer value to be set to.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_assign(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size, const void* const p_value);

/** Doxygen
 * @brief Sets a new value at hash in the pointed at hashmap.
 * 
 * @param p_hashmap Pointer to hashmap to add new pair into.
 * @param hash Hash value to be identify pair to change. (size_t)
 * @param p_value Data pointer value to be set to.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_assign_hash(si_hashmap_t* const p_hashmap,
	const size_t hash, const void* const p_value);

/** Doxygen
 * @brief Remove a key/value pair from a hashmap.
 * 
 * @param p_hashmap Pointer to si_hashmap_t struct to remove from.
 * @param p_key Pointer to data to be used to geneterate hash.
 * @param key_size Number of bytes in data buffer to read to hash.
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_remove(si_hashmap_t* const p_hashmap, const void* const p_key,
	const size_t key_size);

/** Doxygen
 * @brief Remove a key/value pair from a hashmap.
 * 
 * @param p_hashmap Pointer to si_hashmap_t struct to remove from.
 * @param hash Hash value to be identify pair to remove. (size_t)
 * 
 * @return Returns stdbool true on success. Returns false otherwise.
 */
bool si_hashmap_remove_hash(si_hashmap_t* const p_hashmap, const size_t hash);

/** Doxygen
 * @brief Free allocated data inside already existing si_hashmap_t struct.
 * 
 * @param p_hashmap Pointer to hashmap to have it data freed from.
 */
void si_hashmap_free(si_hashmap_t* p_hashmap);

/** Doxygen
 * @brief Frees si_hashmap_t struct at pointer and all of it's allocated data.
 * 
 * @param pp_hashmap Pointer to hashmap pointer to have it data freed.
 */
void si_hashmap_free_at(si_hashmap_t** pp_hashmap);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_HASHMAP_H
