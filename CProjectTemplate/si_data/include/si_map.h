//si_map.h
// Non-Hashing dictionary container data type

#include "si_realloc_settings.h"
#include "si_linked_list.h"

#ifndef SI_MAP_H
#define SI_MAP_H

typedef struct si_map_pair
{
	si_dynamic key;
	si_dynamic value;
} si_map_pair;

/* Doxygen
 * @brief Initializes a si_map_pair struct with new dynamics read from pointers.
 *
 * @param p_pair Pointer to si_map_pair struct to initialize values of.
 * @param p_key Pointer into memory containing values to be used as a key.
 * @param key_size Number of bytes in the key value.
 * @param p_value Pointer into memory containing value to be stored.
 * @param value_size Number of bytes to be stored from p_value.
 */
void si_map_pair_new(si_map_pair* const p_pair, const void* const p_key,
	const size_t key_size, const void* const p_value, const size_t value_size);

typedef struct si_map
{
	si_linked_list entries;
} si_map;

/* Doxygen
 * @brief Initializes/Allocs a si_map struct at p_map's values to defaults.
 *
 * @param p_map Pointer to si_map struct to be initialized.
 */
void si_map_new(si_map* const p_map);

/* Doxygen
 * @brief Finds the count of key/value pairs stores in map.
 *
 * @param p_map Pointer to si_map struct to be counted.
 *
 * @return Returns the number of elements in the map. Returns SIZE_MAX on error
 */
size_t si_map_count(const si_map* const p_map);

/* Doxygen
 * @brief Finds the index of the entry that contains key/value pair with p_key
 *
 * @param p_map Pointer to the si_map to search for entry from.
 * @param p_key Pointer to bytes to read key value from.
 * @param key_size Number of bytes in the key used.
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_index_of_raw(const si_map* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Finds the index of the entry that contains key/value pair with p_key
 *
 * @param p_map Pointer to the si_map to search for entry from.
 * @param p_key Pointer to si_dynamic struct to read key value from.
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_index_of(const si_map* const p_map, const si_dynamic* const p_key);

/* Doxygen
 * @brief Finds dynamic value by it's raw key value in a si_map.
 *
 * @param p_map Pointer to si_map struct that is to be read from.
 * @param p_key Pointer to memory containing key value bytes.
 * @param key_size Number of byted in the p_key buffer to use.
 *
 * @return Returns pointer to si_dynamic struct on success. Otherwise NULL.
 */
si_dynamic* si_map_at_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Finds dynamic value by it's si_dynamic key value in a si_map.
 *
 * @param p_map Pointer to si_map struct that is to be read from.
 * @param p_key Pointer to memory containing key value bytes.
 * @param key_size Number of byted in the p_key buffer to use.
 *
 * @return Returns pointer to si_dynamic struct on success. Otherwise NULL.
 */
si_dynamic* si_map_at(si_map* const p_map, const si_dynamic* const p_key);

/* Doxygen
 * @brief Tests for the existance of a key within a si_map struct.
 *
 * @param p_map Pointer to si_map struct to be tested for key.
 * @param p_key Pointer to memory containing key to look for.
 * @param key_size Number of bytes composing the key in memory.
 *
 * @return Returns true if key is found within the map. False otherwise.
 */
bool si_map_has_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Tests for the existance of a key within a si_map struct.
 *
 * @param p_map Pointer to si_map struct to be tested for key.
 * @param p_key Pointer to si_dynamic struct containing key to test for.
 *
 * @return Returns true if key is found within the map. False otherwise.
 */
bool si_map_has(si_map* const p_map, const si_dynamic* const p_key);

/* Doxygen
 * @brief Removes key/value pair from si_map where provided key first matches.
 *
 * @param p_map Pointer to si_map struct to have pair removed from.
 * @param p_key Pointer to memory containing key to look for.
 * @param key_size Number of bytes in the key being searched for.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove_raw(si_map* const p_map, const void* const p_key, const size_t key_size);
/* Doxygen
 * @brief Removes key/value pair from si_map where provided key first matches.
 *
 * @param p_map Pointer to si_map struct to have pair removed from.
 * @param p_key Pointer to si_dynamic struct containing the key value to match.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove(si_map* const p_map, const si_dynamic* const p_key);

/* Doxygen
 * @brief Inserts a created new key/value pair into an existing si_map struct.
 *
 * @param p_map Pointer to si_map struct to be inserted into.
 * @param p_key Pointer to value to be used as key.
 * @param key_size Number of bytes in the key value buffer.
 * @param p_value Pointer to the value buffer to be read from.
 * @param value_size Number of bytes in the value buffer to be inserted.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert_raw(si_map* const p_map, const void* const p_key,
	const size_t key_size, const void* p_value, const size_t value_size);
/* Doxygen
 * @brief Inserts a key/value pair into an existing si_map struct.
 *
 * @param p_map Pointer to si_map struct to be inserted into.
 * @param p_pair Pointer to si_map_pair struct to define key and value.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert(si_map* const p_map, const si_map_pair* const p_pair);

/* Doxygen
 * @brief Frees memory allocated within a si_map and its entry pairs.
 *
 * @param p_map Pointer to si_map struct to have it's contents freed.
 */
void si_map_free(si_map* const p_map);

#endif//SI_MAP_H
