#include <stdio.h>

#define SI_TEMPLATE_TYPE char
#include "si_array.template"

int main(int argc, char** pp_argv)
{
	char_array array = {};
	char_array_new_2(&array, 69u);
	char_array_set(&array, 0u, '$');
	printf("Value at 0: %c\n", char_array_get(&array, 0u));
	char_array_free(&array);
	return 0;
}
