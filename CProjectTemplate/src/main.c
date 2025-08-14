#include <stdio.h>

#include "si_array.h"

int main(int argc, char** pp_argv)
{
	si_array_t array = {0};
	si_array_init(&array, 0u);
	si_array_free(&array);
	// As is tradition:
	printf("Hello World!\n");
	return 0;
}
