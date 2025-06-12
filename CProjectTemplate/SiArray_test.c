#include <stdio.h>

#define SI_TEMPLATE_TYPE char
#include "SiArray.template"

void runTest() {
	charArray cArray = {};
	charArray_new(&cArray);
	printf("Array Initialized: Element Size: %li\t Capacity: %li\n", cArray.elementSize, cArray.capacity);
	//*
	for(unsigned int i = 0u; i < 95u; i++)
		charArray_append(&cArray, (' ' + i));
	charArray_append(&cArray, '\0');
	//*/
	printf("Append: '%s':%li\n", (char*)cArray.data, cArray.count);
	charArray_removeBackBy(&cArray, 25u);
	printf("Remove: '%s':%li\n", (char*)cArray.data, cArray.count);
	charArray_clear(&cArray);
	printf("Clear: '%s':%li\n", (char*)cArray.data, cArray.count);
	charArray_freeAt(&cArray);
}

int main(int argc, char** argv){
	runTest();
	return 0;
}
