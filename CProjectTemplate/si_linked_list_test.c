#include <stdio.h>

#include "si_linked_list.h"

int main(int argc, char** pp_argv)
{
	short initial_value = 42;
	si_linked_list list = (si_linked_list){};
	si_linked_list_new(&list, sizeof(short), &initial_value);
	short next_short = 21;
	for(int i = 0; i < 5; i++)
	{
		si_linked_list_append(&list, &next_short);
		next_short += i + 1;
	}
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");
	si_linked_list_remove_at(&list, 3u);
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");
	si_linked_list_free(&list);
	return 0;
}
