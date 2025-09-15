//si_double_list.c

#include "si_double_list.h"

si_double_node_t* si_double_node_new()
{
	si_double_node_t* p_new = calloc(1u, sizeof(si_double_node_t));
	if(NULL == p_new)
	{
		goto END;
	}
	p_new->p_data = NULL;
	p_new->p_next = NULL;
	p_new->p_back = NULL;
END:
	return p_new;
}

size_t si_double_node_count(const si_double_node_t* const p_node)
{
	size_t counter = 0u;
	if(NULL == p_node)
	{
		goto END;
	}
	const si_double_node_t* p_iterator = (const si_double_node_t*)p_node;
	while(NULL != p_iterator)
	{
		if(NULL != p_iterator->p_data)
		{
			counter++;
		}
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_node)
		{
			// Circular linked list
			break;
		}
	}
END:
	return counter;
}

size_t si_double_node_capacity(const si_double_node_t* const p_node)
{
	size_t counter = 0u;
	if(NULL == p_node)
	{
		goto END;
	}
	const si_double_node_t* p_iterator = (const si_double_node_t*)p_node;
	while(NULL != p_iterator)
	{
		counter++;
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_node)
		{
			// Circular linked list
			break;
		}
	}
END:
	return counter;
}

/** Doxygen
 * @brief Returns pointer to si_double_node_t index away from current node
 *
 * @param p_list Pointer to current linked_list node to walk index times.
 * @param index Number of nodes away from the current node to walk. Index
 *        values larger than capacity wrap around if list is circular.
 *
 * @return Returns pointer to linked list node by index. Or NULL on error.
 */
static si_double_node_t* si_double_node_at(
	const si_double_node_t* const p_node, const int64_t index)
{
	const si_double_node_t* p_result = NULL;
	if(NULL == p_node)
	{
		goto END;
	}
	p_result = (const si_double_node_t*)p_node;
	for(int64_t iii = 0; iii < index; iii++)
	{
		if(NULL == p_result->p_next)
		{
			break;
		}
		p_result = p_result->p_next;
	}
	for(int64_t iii = 0; iii < (-1 * index); iii++)
	{
		if(NULL == p_result->p_back)
		{
			break;
		}
		p_result = p_result->p_back;
	}
END:
	return (si_double_node_t*)p_result;
}

/** Doxygen
 * @brief Local function that inserts a new node with p_data into
 *        the node's p_next.
 *
 * @param p_list Pointer to a si_double_list struct.
 * @param p_data Pointer to value of unknown size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
static bool si_double_node_insert_next(si_double_node_t* const p_node,
    const void* const p_data)
{
	bool result = false;
	if(NULL == p_node)
	{
		goto END;
	}
	si_double_node_t* p_old = p_node->p_next;
	p_node->p_next = si_double_node_new();
	if(NULL == p_node->p_next)
	{
		p_node->p_next = p_old;
		goto END;
	}
	p_node->p_next->p_next = p_old;
	p_node->p_next->p_back = p_node;
	p_node->p_next->p_data = (void*)p_data;
	if(NULL != p_old)
	{
		p_old->p_back = p_node->p_next;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Local function that inserts a new node with p_data into
 *        the node's p_back.
 *
 * @param p_list Pointer to a si_double_list struct.
 * @param p_data Pointer to value of unknown size to add to list.
 *
 * @return Returns true on success. False otherwise.
 */
static bool si_double_node_insert_back(si_double_node_t* const p_node,
    const void* const p_data)
{
	bool result = false;
	if(NULL == p_node)
	{
		goto END;
	}
	si_double_node_t* p_old = p_node->p_back;
	p_node->p_back = si_double_node_new();
	if(NULL == p_node->p_back)
	{
		p_node->p_back = p_old;
		goto END;
	}
	p_node->p_back->p_next = p_node;
	p_node->p_back->p_back = p_old;
	p_node->p_back->p_data = (void*)p_data;
	if(NULL != p_old)
	{
		p_old->p_next = p_node->p_back;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Removes double linked list node after the node specified by p_node.
 *
 * @param p_node Pointer to a si_double_node_t struct to remove it's p_next.
 *
 * @return Returns True on success. Returns false otherwise.
 */
static bool si_double_node_remove_next(si_double_node_t* const p_node)
{
	bool result = false;
	if(NULL == p_node)
	{
		goto END;
	}
	if(NULL == p_node->p_next)
	{
		// Already removed
		goto END;
	}
	if(p_node == p_node->p_next)
	{
		// Last node circular
		// Call si_double_node_remove() instead.
		goto END;
	}
	si_double_node_t* p_after = p_node->p_next->p_next;
	free(p_node->p_next);
	p_node->p_next = p_after;
	if(NULL != p_after)
	{
		p_after->p_back = p_node;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Removes double linked list node before the node specified by p_node.
 *
 * @param p_node Pointer to a si_double_node_t struct to remove it's p_back.
 *
 * @return Returns True on success. Returns false otherwise.
 */
static bool si_double_node_remove_back(si_double_node_t* const p_node)
{
	bool result = false;
	if(NULL == p_node)
	{
		goto END;
	}
	if(NULL == p_node->p_back)
	{
		// Already removed
		goto END;
	}
	if(p_node == p_node->p_back)
	{
		// Last node circular
		// Call si_double_node_remove() instead.
		goto END;
	}
	si_double_node_t* p_before = p_node->p_back->p_back;
	free(p_node->p_back);
	p_node->p_back = p_before;
	if(NULL != p_before)
	{
		p_before->p_next = p_node;
	}
	result = true;
END:
	return result;
}

/** Doxygen
 * @brief Removes double linked list node specified by p_node.
 *
 * @param p_node Pointer to the si_double_node_t struct to remove.
 *
 * @return Returns True on success. Returns false otherwise.
 */
static bool si_double_node_remove(si_double_node_t* p_node)
{
	bool result = false;
	if(NULL == p_node)
	{
		// Already freed
		goto END;
	}
	result = si_double_node_remove_back(p_node->p_next);
	if(true != result)
	{
		result = si_double_node_remove_next(p_node->p_back);
	}
	if(true != result)
	{
		free(p_node);
		result = true;
	}
END:
	return result;
}

/** Doxygen
 * @brief Frees the contents of a si_double_node_t recursively.
 */
static void si_double_node_free(si_double_node_t* p_node)
{
	if(NULL == p_node)
	{
		goto END;
	}
	p_node->p_data = NULL;

	// If circular break the circle to prevent infinite loops
	if((NULL != p_node->p_back) && (p_node->p_back != p_node))
	{
		p_node->p_back->p_next = NULL;
	}
	p_node->p_back = NULL;

	if((NULL != p_node->p_next) && (p_node->p_next != p_node))
	{
		si_double_node_t* p_next = p_node->p_next;
		si_double_node_free(p_next);
		free(p_next);
	}
	p_node->p_next = NULL;
END:
	return;
}


void si_double_list_init_4(si_double_list_t* const p_list,
	const bool is_circular, const size_t initial_capacity,
	int (*p_cmp_f)(const void* const, const void* const))
{
	if(NULL == p_list)
	{
		goto END;
	}
	p_list->is_circular = is_circular;
	p_list->count = 0u;
	p_list->capacity = 0u;
	p_list->p_head = NULL;
	p_list->p_tail = NULL;
	p_list->p_cmp_f = p_cmp_f;
	si_double_list_grow_to(p_list, initial_capacity);
	si_double_list_set_circular_2(p_list, p_list->is_circular);
END:
	return;
}
inline void si_double_list_init_3(si_double_list_t* const p_list,
	const bool is_circular, const size_t initial_capacity)
{
	// Default value of p_cmp_f = NULL
	si_double_list_init_4(p_list, is_circular, initial_capacity, NULL);
}
inline void si_double_list_init_2(si_double_list_t* const p_list,
	const bool is_circular)
{
	// Default value of initial_capacity
	si_double_list_init_3(p_list, is_circular,
		SI_DOUBLE_LIST_DEFAULT_CAPACITY);
}
inline void si_double_list_init(si_double_list_t* const p_list)
{
	// Default value of is_circular
	si_double_list_init_2(p_list, SI_DOUBLE_LIST_IS_CIRCULAR);
}

si_double_list_t* si_double_list_new_3(const bool is_circular,
    const size_t initial_capacity,
    int (*p_cmp_f)(const void* const, const void* const))
{
	si_double_list_t* p_new = NULL;
	p_new = calloc(1u, sizeof(si_double_list_t));
	if(NULL == p_new)
	{
		goto END;
	}
	si_double_list_init_4(p_new, is_circular, initial_capacity, p_cmp_f);
END:
	return p_new;
}
inline si_double_list_t* si_double_list_new_2(const bool is_circular,
    const size_t initial_capacity)
{
	// Default value for p_cmp_f = NULL
	return si_double_list_new_3(is_circular, initial_capacity, NULL);
}
inline si_double_list_t* si_double_list_new_1(const bool is_circular)
{
	// Default value for initial_capacity
	return si_double_list_new_2(is_circular, SI_DOUBLE_LIST_DEFAULT_CAPACITY);
}
inline si_double_list_t* si_double_list_new()
{
	// Default value for is_circular
	return si_double_list_new_1(SI_DOUBLE_LIST_IS_CIRCULAR);
}

/** Doxygen
 * @brief Finds the node at index going shortest path possible.
 *
 * @param p_list Pointer to double linked list to be walked.
 * @param index Offset into list to get node from.
 *
 * @return Returns si_double_node_t pointer to node at index.
 */
static si_double_node_t* si_double_list_node_at(
	const si_double_list_t* const p_list, const size_t index)
{
	si_double_node_t* p_result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	if(0u == p_list->capacity)
	{
		goto END;
	}
	const size_t HALF_CAP = p_list->capacity / 2;
	size_t mod_index = index;
	if(p_list->is_circular)
	{
		// If list is circular, wrap the index around.
		mod_index = mod_index % p_list->capacity;
	}
	if(mod_index >= p_list->capacity)
	{
		goto END;
	}
	if(mod_index < HALF_CAP)
	{
		// This is safe to cast as SIZE_MAX / 2 <= INT64_MAX
		p_result = si_double_node_at(p_list->p_head, (int64_t)mod_index);
	}
	else
	{
		// Because int division rounds down, if index == HALF_CAP going
		// backward from the tail node is one step faster.
		int64_t new_index = ((int64_t)p_list->capacity) - 1;// Last Valid Index
		new_index -= mod_index; // Distance backwards to walk
		p_result = si_double_node_at(p_list->p_tail, -new_index);
	}
END:
	return p_result;
}

void* si_double_list_at(const si_double_list_t* const p_list,
	const size_t index)
{
	void* p_result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	si_double_node_t* p_node = si_double_list_node_at(p_list, index);
	if(NULL == p_node)
	{
		goto END;
	}
	p_result = p_node->p_data;
END:
	return p_result;
}

bool si_double_list_is_empty(si_double_list_t* const p_list)
{
	bool result = true;
	if(NULL == p_list)
	{
		goto END;
	}
	result = (0u == p_list->count);
END:
	return result;
}

bool si_double_list_is_full(si_double_list_t* const p_list)
{
	bool result = true;
	if(NULL == p_list)
	{
		goto END;
	}
	result = (p_list->count == p_list->capacity);
END:
	return result;
}

size_t si_double_list_find_3(const si_double_list_t* const p_list,
    const void* const p_data, const size_t start_index)
{
	size_t result = SIZE_MAX;
	if(NULL == p_list)
	{
		goto END;
	}
	if((NULL == p_list->p_cmp_f) || (0u >= p_list->capacity))
	{
		goto END;
	}
	size_t index = start_index;
	if(p_list->is_circular)
	{
		index = (index % p_list->capacity);
	}
	si_double_node_t* p_start = si_double_list_node_at(p_list, index);
	si_double_node_t* p_iterator = p_start;
	while(NULL != p_iterator)
	{
		const int cmp_result = p_list->p_cmp_f(p_iterator->p_data, p_data);
		if(0 == cmp_result)
		{
			// Match found!
			if(p_list->is_circular)
			{
				result = (index % p_list->capacity);
			}
			else
			{
				result = index;
			}
			break;
		}
		index++;
		p_iterator = p_iterator->p_next;
	}
END:
	return result;
}
inline size_t si_double_list_find(const si_double_list_t* const p_list,
    const void* const p_data)
{
	// Default value for start_index is 0u(Start from begining).
	return si_double_list_find_3(p_list, p_data, 0u);
}

bool si_double_list_sort(si_double_list_t* const p_list)
{
	// TODO Implement a faster sort
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	if(NULL == p_list->p_cmp_f)
	{
		goto END;
	}
	si_double_node_t* p_to_set = p_list->p_head;
	si_double_node_t* p_walker = NULL;
	for(size_t iii = 0u; iii < p_list->capacity; iii++)
	{
		if(NULL == p_to_set)
		{
			break;
		}
		p_walker = p_to_set;
		for(size_t jjj = iii + 1; jjj < p_list->capacity; jjj++)
		{
			p_walker = p_walker->p_next;
			if(NULL == p_walker)
			{
				break;
			}
			// Ascending order
			int cmp_result = p_list->p_cmp_f(
				p_to_set->p_data, p_walker->p_data
			);
			if(0 > cmp_result)
			{
				// Swap
				void* p_tmp = p_to_set->p_data;
				p_to_set->p_data = p_walker->p_data;
				p_walker->p_data = p_tmp;
			}
		}
		p_to_set = p_to_set->p_next;
	}
	result = true;
END:
	return result;
}

bool si_double_list_insert(si_double_list_t* const p_list,
	const void* const p_data, const size_t index)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	size_t new_index = index;
	if((p_list->is_circular) && (0u < p_list->capacity))
	{
		new_index = index % (p_list->capacity + 1u);
	}
	if(new_index > p_list->capacity)
	{
		goto END;
	}
	// Handle initial insert edge case
	if((NULL == p_list->p_head) && (0u == new_index))
	{
		p_list->p_head = si_double_node_new();
		p_list->p_tail = p_list->p_head;
		if(NULL != p_list->p_head)
		{
			if(p_list->is_circular)
			{
				p_list->p_head->p_next = p_list->p_head;
				p_list->p_head->p_back = p_list->p_head;
			}
			p_list->p_head->p_data = (void*)p_data;
			result = true;
		}
		goto END;
	}
	// Handle append
	if(new_index == p_list->capacity)
	{
		result = si_double_node_insert_next(p_list->p_tail, p_data);
		goto END;
	}
	// Find parent node.
	const size_t HALF_CAP = (p_list->capacity / 2);
	if((0u < new_index) && (new_index < HALF_CAP))
	{
		// Walk from head to parent
		si_double_node_t* const p_parent =
			si_double_list_node_at(p_list, new_index - 1u);
		result = si_double_node_insert_next(p_parent, p_data);
	}
	else
	{
		// Walk from tail to parent
		si_double_node_t* const p_parent =
			si_double_list_node_at(p_list, new_index);
		result = si_double_node_insert_back(p_parent, p_data);
	}
END:
	// If the insert happened, update the values in p_list.
	if(true == result)
	{
		if(p_list->capacity == new_index)
		{
			p_list->p_tail = p_list->p_tail->p_next;
		}
		p_list->capacity++;
		if(NULL != p_data)
		{
			p_list->count++;
		}
		if(0u == new_index)
		{
			p_list->p_head = p_list->p_head->p_back;
		}
	}
	return result;
}

void si_double_list_set_circular_2(si_double_list_t* const p_list,
	const bool is_circular)
{
	if(NULL == p_list)
	{
		goto END;
	}
	if((NULL == p_list->p_head) || (NULL == p_list->p_tail))
	{
		goto END;
	}
	if(true == is_circular)
	{
		// Make circular
		p_list->p_head->p_back = p_list->p_tail;
		p_list->p_tail->p_next = p_list->p_head;
	}
	else
	{
		// Make Non-circular
		p_list->p_head->p_back = NULL;
		p_list->p_tail->p_next = NULL;
	}
	p_list->is_circular = is_circular;
END:
	return;
}
inline void si_double_list_set_circular(si_double_list_t* const p_list)
{
	// Default value for is_circular is SI_DOUBLE_LIST_IS_CIRCULAR(true)
	si_double_list_set_circular_2(p_list, SI_DOUBLE_LIST_IS_CIRCULAR);
}

bool si_double_list_is_circular(si_double_list_t* const p_list)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = p_list->is_circular;
END:
	return result;
}

size_t si_double_list_grow_by(si_double_list_t* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < amount; iii++)
	{
		bool app_result = si_double_list_append(p_list, NULL);
		if(true != app_result)
		{
			break;
		}
		result++;
	}
END:
	return result;
}

size_t si_double_list_grow_to(si_double_list_t* const p_list,
	const size_t capacity)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	result = p_list->capacity;
	if(result >= capacity)
	{
		// Would have to shrink to hit capacity
		// Call si_double_list_shrink_to instead
		goto END;
	}
	const size_t grow_count = (capacity - result);
	result += si_double_list_grow_by(p_list, grow_count);
END:
	return result;
}

size_t si_double_list_shrink_by(si_double_list_t* const p_list,
	const size_t amount)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	for(size_t iii = 0u; iii < amount; iii++)
	{
		const bool rm_result = si_double_list_remove_at(
			p_list, p_list->capacity - 1u);
		if(true != rm_result)
		{
			break;
		}
		result++;
	}
END:
	return result;
}

size_t si_double_list_shrink_to(si_double_list_t* const p_list,
	const size_t capacity)
{
	size_t result = 0u;
	if(NULL == p_list)
	{
		goto END;
	}
	result = p_list->capacity;
	if(result <= capacity)
	{
		// Would have to grow to hit capacity
		// Call si_double_list_grow_to instead
		goto END;
	}
	const size_t remove_count = (result - capacity);
	result -= si_double_list_shrink_by(p_list, remove_count);
END:
	return result;
}

bool si_double_list_resize(si_double_list_t* const p_list,
	const size_t capacity)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	if(p_list->capacity <= capacity)
	{
		result = (capacity == si_double_list_grow_to(p_list, capacity));
		goto END;
	}
	result = (capacity == si_double_list_shrink_to(p_list, capacity));
END:
	return result;
}

bool si_double_list_append(si_double_list_t* const p_list,
	const void* const p_data)
{
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	result = si_double_list_insert(p_list, p_data, p_list->capacity);
END:
	return result;
}

bool si_double_list_remove_at(si_double_list_t* p_list,
    const size_t index)
{
	bool had_data = false;
	bool result = false;
	if(NULL == p_list)
	{
		goto END;
	}
	if((NULL == p_list->p_head) || (NULL == p_list->p_tail))
	{
		goto END;
	}
	// Handle 0 index edge case
	if(0u == index)
	{
		had_data = (NULL != p_list->p_head->p_data);
		if(p_list->p_tail == p_list->p_head)
		{
			// Handle last node case
			p_list->p_tail = NULL;
		}
		si_double_node_t* p_old = p_list->p_head;
		if(NULL == p_list->p_tail)
		{
			p_list->p_head = NULL;
		}
		else
		{
			p_list->p_head = p_old->p_next;
		}
		result = si_double_node_remove(p_old);
		p_old = NULL;
		goto END;
	}
	// Walk shortest path to parent node
	const size_t HALF_CAP = p_list->capacity / 2;
	if(HALF_CAP <= index)
	{
		// Remove back parent
		si_double_node_t* p_after = si_double_list_node_at(p_list, index + 1u);
		if(NULL != p_after->p_back)
		{
			had_data = (NULL != p_after->p_back->p_data);
		}
		result = si_double_node_remove_back(p_after);
	}
	else
	{
		// Remove next parent
		si_double_node_t* p_back = si_double_list_node_at(p_list, index - 1u);
		if(NULL != p_back->p_next)
		{
			had_data = (NULL != p_back->p_next->p_data);
		}
		result = si_double_node_remove_next(p_back);
	}
END:
	if(true == result)
	{
		p_list->capacity--;
		if(true == had_data)
		{
			p_list->count--;
		}
	}
	return result;
}

size_t si_double_list_push(si_double_list_t* const p_list,
	const void* const p_data)
{
	size_t result = SIZE_MAX;
	if(NULL == p_list)
	{
		goto END;
	}
	if(p_list->count >= p_list->capacity)
	{
		// List is full, insert instead of assign.
		const bool app_result = si_double_list_append(p_list, p_data);
		if(true == app_result)
		{
			result = p_list->capacity - 1u;
		}
		goto END;
	}
	si_double_node_t* p_iterator = p_list->p_head;
	while(NULL != p_iterator)
	{
		result++;
		if(NULL == p_iterator->p_data)
		{
			p_iterator->p_data = (void*)p_data;
			break;
		}
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_list->p_head)
		{
			// Circular
			break;
		}
	}
	if(NULL != p_iterator)
	{
		if(p_data != p_iterator->p_data)
		{
			result = SIZE_MAX;
		}
	}
	if(SIZE_MAX != result)
	{
		p_list->count++;
	}
END:
	return result;
}

void* si_double_list_pop(si_double_list_t* const p_list)
{
	void* p_result = NULL;
	if(NULL == p_list)
	{
		goto END;
	}
	si_double_node_t* p_iterator = p_list->p_head;
	while(NULL != p_iterator)
	{
		if(NULL != p_iterator->p_data)
		{
			p_result = p_iterator->p_data;
			p_iterator->p_data = NULL;
			p_list->count--;
			break;
		}
		p_iterator = p_iterator->p_next;
		if(p_iterator == p_list->p_head)
		{
			// Circular
			break;
		}
	}
END:
	return p_result;
}

void si_double_list_free(si_double_list_t* p_list)
{
	if(NULL == p_list)
	{
		goto END;
	}
	p_list->capacity = 0u;
	p_list->count = 0u;
	p_list->p_cmp_f = NULL;
	if(NULL != p_list->p_head)
	{
		si_double_node_free(p_list->p_head);
		free(p_list->p_head);
	}
	p_list->p_head = NULL;
	p_list->p_tail = NULL;
END:
	return;
}

void si_double_list_free_at(si_double_list_t** const pp_list)
{
	if(NULL == pp_list)
	{
		goto END;
	}
	if(NULL == *pp_list)
	{
		// Already freed
		goto END;
	}
	si_double_list_free(*pp_list);
	free(*pp_list);
	*pp_list = NULL;
END:
	return;
}
