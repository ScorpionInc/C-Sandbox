/* si_map.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Define structs with functions for managing data in
 *          a non-hashing dictionary container.
 * Created: 20250711
 * Updated: 20250724
//*/

#include "si_array.h"
#include "si_realloc_settings.h"

#ifndef SI_MAP_H
#define SI_MAP_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

typedef struct si_map_pair_t
{
	void* p_key;
	void* p_value;
} si_map_pair_t;

/* Doxygen
 * @brief Initializes a new si_map_pair_t struct with values of pointers.
 *
 * @param p_key Pointer into memory to be used as a key.
 * @param p_value Pointer into memory to be stored.
 * 
 * @return Returns pointer to new struct on success. Returns NULL otherwise.
 */
si_map_pair_t* si_map_pair_new(const void* const p_key,
	const void* const p_value);


typedef struct si_map_t
{
	si_array_t entries;
	si_realloc_settings_t* p_settings;
	int  (*p_cmp_key_f)(const void* const, const void* const);
	int  (*p_cmp_value_f)(const void* const, const void* const);
	void (*p_free_key_f)(void* const);
	void (*p_free_value_f)(void* const);
} si_map_t;

/* Doxygen
 * @brief Initializes a si_map_t struct at p_map's values to their defaults.
 *
 * @param p_map Pointer to si_map_t struct to be initialized.
 */
void si_map_init(si_map_t* const p_map);

/* Doxygen
 * @brief Initializes/Allocs a si_map_t struct at p_map's values to their defaults.
 * 
 * @return Returns pointer to new si_map_t on success. Returns NULL otherwise.
 */
si_map_t* si_map_new();

/* Doxygen
 * @brief Finds the count of key/value pairs stored in p_map.
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
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_index_of(const si_map_t* const p_map, const void* const p_key);

/* Doxygen
 * @brief Finds the index of a entry that contains key/value pair with p_value
 *
 * @param p_map Pointer to the si_map_t to search for entry from.
 * @param p_value Pointer to value to search for.
 *
 * @return Returns entry index on success. SIZE_MAX otherwise.
 */
size_t si_map_find(const si_map_t* const p_map, const void* const p_value);

/* Doxygen
 * @brief Finds dynamic value by it's raw key value in a si_map_t.
 *
 * @param p_map Pointer to si_map_t struct that is to be read from.
 * @param p_key Pointer to memory containing key value bytes.
 *
 * @return Returns pointer to si_dynamic struct on success. Otherwise NULL.
 */
void* si_map_at(si_map_t* const p_map, const void* const p_key);

/* Doxygen
 * @brief Tests for the existance of a key within a si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be tested for key.
 * @param p_key Pointer to memory containing key to look for.
 *
 * @return Returns true if key is found within the map. False otherwise.
 */
bool si_map_has(si_map_t* const p_map, const void* const p_key);

/* Doxygen
 * @brief Removes key/value pair from si_map_t at index.
 *
 * @param p_map Pointer to si_map_t struct to have pair removed from.
 * @param index Index to pair to be removed from dynamic array.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove_at(si_map_t* const p_map, const size_t index);

/* Doxygen
 * @brief Removes key/value pair from si_map_t where provided key first matches.
 *
 * @param p_map Pointer to si_map_t struct to have pair removed from.
 * @param p_key Pointer to memory containing key to look for.
 *
 * @return Returns true on removal of key/value pair. Returns false otherwise.
 */
bool si_map_remove(si_map_t* const p_map, const void* const p_key);

/* Doxygen
 * @brief Inserts a created new key/value pair into an existing si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be inserted into.
 * @param p_pair Pointer to key value pair to be added to map.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert_pair(si_map_t* const p_map,
	const si_map_pair_t* const p_pair);

/* Doxygen
 * @brief Inserts a created new key/value pair into an existing si_map_t struct.
 *
 * @param p_map Pointer to si_map_t struct to be inserted into.
 * @param p_key Pointer to value to be used as key.
 * @param p_value Pointer to the value buffer to be read from.
 *
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_insert(si_map_t* const p_map, const void* const p_key,
	const void* const p_value);

/* Doxygen
 * @brief In p_map, sets first key match's value to p_value.
 *
 * @param p_map Pointer to the si_map_t to have a value set in.
 * @param p_key Pointer to the key value to identify the pair.
 * @param p_value New pointer value to be assigned to key in map.
 * 
 * @return Returns true on success. Returns false otherwise.
 */
bool si_map_assign(si_map_t* const p_map, const void* const p_key,
	const void* const p_value);

/* Doxygen
 * @brief Frees memory allocated within a si_map_t and its entry pairs.
 *
 * @param p_map Pointer to si_map_t struct to have it's contents freed.
 */
void si_map_free(si_map_t* const p_map);

/* Doxygen
 * @brief Frees memory allocated within a si_map_t and its entry pairs.
 *
 * @param p_map Pointer to si_map_t struct to have it's contents freed.
 */
void si_map_free_at(si_map_t** pp_map);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SI_MAP_H
