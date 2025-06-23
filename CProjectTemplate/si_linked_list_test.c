#include <stdio.h>

#include "si_linked_list.h"

int main(int argc, char** pp_argv)
{
	short initial_value = 42;
	si_linked_list list = (si_linked_list){};
	si_linked_list_new(&list, sizeof(short), &initial_value);
	short next_short = 21;
	si_linked_list_append(&list, &next_short);
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");
	si_linked_list_free(&list);
	return 0;
}
