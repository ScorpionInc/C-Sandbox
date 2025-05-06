/* SiArray.h
 * Language: C
 * Authors: ScorpionInc
 * Purpose: Defines a struct with functions for managing dynamically allocated contigious array of fixed type size values.
 * Created: 20141012
 * Updated: 20250501
//*/

#include <math.h> //round, pow
#include <stdbool.h> //true, false
#include <stdlib.h> //malloc, free, size_t
#include <string.h> //memcpy

#ifndef SIARRAY_H_
#define SIARRAY_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

struct SiArray{
	size_t elementSize;
	size_t capacity;
	size_t count;
	enum SiArrayGrowMethod {
		SiArrayGrowNever,
		SiArrayGrowLinear,
		SiArrayGrowScalar,
		SiArrayGrowExponential
	} growMethod;
	float growRate;
	void* data;
};

const size_t SIARRAY_DEFAULT_CAPACITY = 32u;
const enum SiArrayGrowMethod SIARRAY_DEFAULT_GROW_METHOD = SiArrayGrowExponential;
const float SIARRAY_DEFAULT_GROW_RATE = 2.0f;

// Free dynamic array in struct pointed at by address.
void SiArray_freeAt(struct SiArray* address){
	if(address == NULL)
		// Invalid struct pointer
		return;
	if(address->data == NULL)
		// Struct's array is already freed (or never initialized)
		return;
	address->count = 0u;
	address->capacity = 0u;
	free(address->data);
	address->data = NULL;
}

// Initialize SiArray Struct at address with initial value(s).
void SiArray_new5(struct SiArray* address, const size_t _elementSize, const size_t _capacity, const enum SiArrayGrowMethod _growMethod, const float _growRate){
	if(address == NULL)
		// Invalid struct pointer
		return;
	// Free if it's already defined.
	SiArray_freeAt(address);
	address->data = malloc(_capacity * _elementSize);
	if(address->data == NULL)
		// Failed to allocate memory
		return;
	if(_capacity > 0u && _elementSize > 0u)
		// Just in case add a NULL-terminator at the end.
		((char*)address->data)[(_capacity * _elementSize) - 1u] = '\0';
	address->elementSize = _elementSize;
	address->capacity = _capacity;
	address->growMethod = _growMethod;
	address->growRate = _growRate;
}
// Helper Initializer for default parameter values
static inline void SiArray_new4(struct SiArray* address, const size_t _elementSize, const size_t _capacity, const enum SiArrayGrowMethod _growMethod){
	SiArray_new5(address, _elementSize, _capacity, _growMethod, SIARRAY_DEFAULT_GROW_RATE);
}
// Helper Initializer for default parameter values
static inline void SiArray_new3(struct SiArray* address, const size_t _elementSize, const size_t _capacity){
	SiArray_new4(address, _elementSize, _capacity, SIARRAY_DEFAULT_GROW_METHOD);
}
// Helper Initializer for default parameter values
static inline void SiArray_new(struct SiArray* address, const size_t _elementSize){
	SiArray_new3(address, _elementSize, SIARRAY_DEFAULT_CAPACITY);
}

// Returns true if provided pointer lies somewhere within allocated mem
// Returns false otherwise.
bool SiArray_pointsToData(const struct SiArray* address, const void* pointer) {
	if(address == NULL || pointer == NULL)
		return false;
	if(address->data == NULL)
		return false;
	return pointer >= address->data && pointer <= address->data + (address->elementSize * address->capacity);
}
// Returns true if provided pointer points at where the start of an element within allocated mem would be.
// Returns false otherwise.
bool SiArray_pointsToDataElement(const struct SiArray* address, const void* pointer) {
	if(!SiArray_pointsToData(address, pointer))
		return false;
	return (((pointer - address->data) % address->elementSize) == 0u);
}
// Returns true if provided pointer points at where the start of an allocated element within allocated mem.
// Returns false otherwise.
bool SiArray_isValidPointer(const struct SiArray* address, const void* pointer) {
	if(!SiArray_pointsToDataElement(address, pointer))
		return false;
	return pointer < address->data + (address->elementSize * address->count);
}

// Returns pointer to element at index on success.
// Returns NULL on failure/error.
void* SiArray_getElementAt(struct SiArray* address, const size_t index){
	if(address == NULL)
		return address;
	if(address->data == NULL)
		return address->data;
	if(index >= address->count)
		return NULL;
	return &((char*)address->data)[index * address->elementSize];
}
static inline void* SiArray_first(struct SiArray* address){
	// Helper Function
	return SiArray_getElementAt(address, 0u);
}
static inline void* SiArray_last(struct SiArray* address){
	// Helper Function
	return address->count > 0u ? SiArray_getElementAt(address, address->count - 1u) : NULL;
}

// Returns the positive change in capacity of address array based upon it's grow rate and grow method on success.
// Returns 0u on failure/error.
size_t SiArray_nextGrowAmount(const struct SiArray* address){
	if(address == NULL)
		return 0u;
	size_t targetSize = 0u;
	switch(address->growMethod) {
	case(SiArrayGrowNever):
		targetSize = address->capacity;
		break;
	case(SiArrayGrowLinear):
		targetSize = (((size_t)-1) - address->growRate < address->capacity) ? ((size_t)-1) : address->capacity + address->growRate;
		break;
	case(SiArrayGrowScalar):
		targetSize = (size_t)round((float)address->capacity * address->growRate);
		break;
	case(SiArrayGrowExponential):
		targetSize = (size_t)round(pow((float)address->capacity, address->growRate));
		break;
	default:
		// Unknown Grow Method
		return 0u;
		break;
	}
	return targetSize - address->capacity;
}
// Returns the negative change in capacity of address array based upon it's grow rate and grow method on success.
// Returns 0u on failure/error.
size_t SiArray_nextShrinkAmount(const struct SiArray* address){
	if(address == NULL)
		return 0u;
	size_t targetSize = 0u;
	switch(address->growMethod) {
	case(SiArrayGrowNever):
		targetSize = address->capacity;
		break;
	case(SiArrayGrowLinear):
		if(address->growRate <= address->capacity)
			targetSize = address->capacity - address->growRate;
		break;
	case(SiArrayGrowScalar):
		targetSize = (size_t)round((float)address->capacity / address->growRate);
		break;
	case(SiArrayGrowExponential):
		targetSize = (size_t)round(pow((float)address->capacity, 1.0f / address->growRate));
		break;
	default:
		// Unknown Method
		return 0u;
		break;
	}
	return address->capacity - targetSize;
}

// Reference: https://stackoverflow.com/a/39562813 Uses realloc for performance.
// Grows Dynamic array capacity by amount regardless of grow method or grow rate.
// Returns true on success.
// Returns false otheriwse.
bool SiArray_growBy(struct SiArray* address, const size_t growAmount){
	if(address == NULL)
		// Invalid struct pointer
		return false;
	if(growAmount <= 0u && address->data != NULL)
		// Nothing to do here
		return true;
	size_t newCapacity = (((size_t)-1) - growAmount < address->capacity) ? ((size_t)-1) : address->capacity + growAmount;
	if(address->data == NULL){
		address->data = malloc(newCapacity * address->elementSize);
		if(address->data == NULL)
			// Failed to allocate required new memory block.
			return false;
	} else {
		void* tempArray = realloc(address->data, newCapacity * address->elementSize);
		if(tempArray == NULL)
			// Failed to reallocate required memory block.
			return false;
		if(address->elementSize > 0u)
			//Just to be safe we will set the last byte in the new buffer to NULL.
			*((char*)tempArray + ((newCapacity * address->elementSize) - 1u)) = '\0';
		address->data = tempArray;
	}
	address->capacity = newCapacity;
	return true;
}
static inline bool SiArray_growTo(struct SiArray* address, const size_t newLargerCapacity){
	// Helper Function
	// Calculates the difference in size from newLargerCapacity and grows array by that much returnung true on success.
	// Returns false if newLargerCapacity < currentCapacity or on failure to allocate.
	return (newLargerCapacity >= address->capacity) ? SiArray_growBy(address, newLargerCapacity - address->capacity) : false;
}
// Increases the size of the dynamic array in the struct pointed
// at by address by its growth rate according to its grow method.
// Returns true on success.
// Returns false otherwise.
bool SiArray_grow(struct SiArray* address){
	if(address == NULL)
		// Invalid struct pointer
		return false;
	if(address->growMethod == SiArrayGrowNever)
		return false;
	return SiArray_growBy(address, SiArray_nextGrowAmount(address));
}

// Adds new element of size elementSize to array.
// Calls grow if appending would exceed current capacity.
// Returns true on success.
// Returns false otherwise.
bool SiArray_appendAt(struct SiArray* address, const void* value){
	// Grow array as/if needed.
	if(address->count == ((size_t)-1))
		// Already at maximum value of index
		return false;
	if(address->count + 1u > address->capacity)
		if(!SiArray_grow(address))
			return false;
	memcpy((address->data + (address->count * address->elementSize)), value, address->elementSize);
	// Step Counter
	address->count += 1u;
	return true;
}

// *WARNING* All shrink commands(except shrinkToFit) risk data loss if shrinking capacity smaller than element count.
// Reference: https://stackoverflow.com/a/39562813 Uses realloc for performance.
// Reduces the capacity of the SiArray at address by shrinkAmount on success returning true ignores grow method and grow rate.
// Otherwise on failure/error returns false.
bool SiArray_shrinkBy(struct SiArray* address, const size_t shrinkAmount){
	if(address == NULL)
		// Invalid array pointer
		return false;
	if(shrinkAmount <= 0u)
		// NOP
		return false;
	if(address->data == NULL || address->capacity <= 0u)
		// Can't shrink from nothing.
		return false;
	size_t newCapacity = 0u;
	if(shrinkAmount < address->capacity)
		newCapacity = address->capacity - shrinkAmount;
	if(newCapacity <= 0u) {
		SiArray_freeAt(address);
		return true;
	}
	size_t newSize = (newCapacity * address->elementSize);
	void* tempArray = realloc(address->data, newSize);
	if(tempArray == NULL)
		// Failed to reallocate memory
		return false;
	// Assign new capacity and the pointer to array.
	address->data = tempArray;
	address->capacity = newCapacity;
	return true;
}
static inline bool SiArray_shrinkTo(struct SiArray* address, const size_t newSmallerCapacity){
	// Helper Function
	return (newSmallerCapacity < address->capacity) ? SiArray_shrinkBy(address, address->capacity - newSmallerCapacity) : false;
}
// Does the opposite of the grow function, reducing the capacity of the SiArray once by it's growth method on success.
// Unlike the grow function this function is not called automatically when removing elements (yet(?)).
bool SiArray_shrink(struct SiArray* address){
	if(address == NULL)
		return false;
	if(address->growMethod == SiArrayGrowNever)
		return false;
	return SiArray_shrinkBy(address, SiArray_nextShrinkAmount(address));
}

// Reduces the capacity to match the current count.
// Returns amount of capacity SiArray reduced by.
size_t SiArray_shrinkToFit(struct SiArray* address){
	size_t originalCapacity = address->capacity;
	SiArray_shrinkTo(address, address->count);
	return originalCapacity - address->capacity;
}

// Either grows or shrinks capacity to reach the target capacity. Ignores grow method.
bool SiArray_setCapacityTo(struct SiArray* address, const size_t targetCount) {
	if(targetCount < address->capacity)
		return SiArray_shrinkTo(address, targetCount);
	else
		return SiArray_growTo(address, targetCount);
}

// Removes amount elements from the end of the data at address on success returning true.
// Returns false on failure.
bool SiArray_removeBackBy(struct SiArray* address, const size_t amount){
	// Validate array is initialized with at least amount elements with a size of at least 1
	// While this is slower than just doing it, it's safer since this function may be called externally.
	if(address == NULL)
		return false;
	if(address->data == NULL)
		return false;
	if(address->elementSize <= 0u)
		return false;
	if(amount == 0u)
		// NOP
		return true;
	if(address->count < amount)
		return false;
	address->count -= amount;
	// No needs to bounds check this byte set as array had to shrink thus count smaller than capacity.
	((char*)address->data)[address->count * address->elementSize] = '\0';
	// TODO Worth it? Best implementation?
	// Auto shrink array as needed
	size_t shrinkAmount = SiArray_nextShrinkAmount(address);
	if(address->capacity - shrinkAmount > address->count)
		return SiArray_shrink(address);
	return true;
}

// Removes last element from array at address. Setting value of first byte to /0 (just in case).
// Returns true on success.
// Returns false otherwise.
static inline bool SiArray_removeBack(struct SiArray* address){
	// Helper Function
	return SiArray_removeBackBy(address, 1u);
}

// Removes all elements of array.
static inline bool SiArray_clear(struct SiArray* address){
	// Helper Function
	return SiArray_removeBackBy(address, address->count);
}
// Alias Function
static inline bool SiArray_empty(struct SiArray* address){
	return SiArray_clear(address);
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif//SIARRAY_H_
