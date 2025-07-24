/* si_map.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define structs with functions for managing data in
 *          a non-hashing dictionary container.
 * Created: 20250711
 * Updated: 20250721
//*/

#include "si_linked_list.h"

#ifndef SI_MAP_H
#define SI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_map_pair_t si_map_pair_t;

typedef struct si_map_t
{
	si_linked_list_t entries;
	int  (*p_cmp_key_f)(const void* const, const void* const);
	int  (*p_cmp_value_f)(const void* const, const void* const);
	void (*p_free_key_f)(void* const);
	void (*p_free_value_f)(void* const);
} si_map_t;

/* Doxygen
 * @brief Initializes/Allocs a si_map_t struct at p_map's values to defaults.
 *
 * @param p_map Pointer to si_map_t struct to be initialized.
 */
void si_map_init(si_map_t* const p_map);

/* Doxygen
 * @brief Initializes/Allocs a si_map_t struct at p_map's values to defaults.
 *
 * @param p_map Pointer to si_map_t struct to be initialized.
 */
si_map_t* si_map_new();

/* Doxygen
 * @brief Finds the count of key/value pairs stores in map.
 *
 * @param p_map Pointer to si_map_t struct to be counted.
 *
 * @return Returns the number of elements in the map. Returns SIZE_MAX on error
 */
size_t si_map_count(const si_map_t* const p_map);

/* Doxygen
 * @brief Finds the index of the entry that contains key/value pair with p_key
 *
 * @param p_map Pointer to the si_map_t to search for entry from.
 * @param p_key Pointer to bytes to read key value from.
 * @param key_size Number of bytes in the key used.
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_index_of_raw(const si_map_t* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Finds the index of the entry that contains key/value pair with p_key
 *
 * @param p_map Pointer to the si_map_t to search for entry from.
 * @param p_key Pointer to si_dynamic struct to read key value from.
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_index_of(const si_map_t* const p_map, const si_dynamic_t* const p_key);

/* Doxygen
 * @brief Finds dynamic value by it's raw key value in a si_map_t.
 *
 * @param p_map Pointer to si_map_t struct that is to be read from.
 * @param p_key Pointer to memory containing key value bytes.
 * @param key_size Number of byted in the p_key buffer to use.
 *
 * @return Returns pointer to si_dynamic struct on success. Otherwise NULL.
 */
si_dynamic_t* si_map_at_raw(si_map_t* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Finds dynamic value by it's si_dynamic key value in a si_map_t.
 *
 * @param p_map Pointer to si_map_t struct that is to be read from.
 * @param p_key Pointer to memory containing key value bytes.
 * @param key_size Number of byted in the p_key buffer to use.
 *
 * @return Returns pointer to si_dynamic struct on success. Otherwise NULL.
 */
si_dynamic_t* si_map_at(si_map_t* const p_map, const si_dynamic_t* const p_key);

/* Doxygen
 * @brief Tests for the existance of a key within a si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be tested for key.
 * @param p_key Pointer to memory containing key to look for.
 * @param key_size Number of bytes composing the key in memory.
 *
 * @return Returns true if key is found within the map. False otherwise.
 */
bool si_map_has_raw(si_map_t* const p_map, const void* const p_key,
	const size_t key_size);
/* Doxygen
 * @brief Tests for the existance of a key within a si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be tested for key.
 * @param p_key Pointer to si_dynamic struct containing key to test for.
 *
 * @return Returns true if key is found within the map. False otherwise.
 */
bool si_map_has(si_map_t* const p_map, const si_dynamic_t* const p_key);

/* Doxygen
 * @brief Removes key/value pair from si_map_t where provided key first matches.
 *
 * @param p_map Pointer to si_map_t struct to have pair removed from.
 * @param p_key Pointer to memory containing key to look for.
 * @param key_size Number of bytes in the key being searched for.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove_raw(si_map_t* const p_map, const void* const p_key, const size_t key_size);
/* Doxygen
 * @brief Removes key/value pair from si_map_t where provided key first matches.
 *
 * @param p_map Pointer to si_map_t struct to have pair removed from.
 * @param p_key Pointer to si_dynamic struct containing the key value to match.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove(si_map_t* const p_map, const si_dynamic_t* const p_key);

/* Doxygen
 * @brief Inserts a created new key/value pair into an existing si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be inserted into.
 * @param p_key Pointer to value to be used as key.
 * @param key_size Number of bytes in the key value buffer.
 * @param p_value Pointer to the value buffer to be read from.
 * @param value_size Number of bytes in the value buffer to be inserted.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert_raw(si_map_t* const p_map, const void* const p_key,
	const size_t key_size, const void* p_value, const size_t value_size);
/* Doxygen
 * @brief Inserts a key/value pair into an existing si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be inserted into.
 * @param p_pair Pointer to si_map_pair struct to define key and value.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert(si_map_t* const p_map, const si_map_pair_t* const p_pair);

/* Doxygen
 * @brief Frees memory allocated within a si_map_t and its entry pairs.
 *
 * @param p_map Pointer to si_map_t struct to have it's contents freed.
 */
void si_map_free(si_map_t* const p_map);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_MAP_H
