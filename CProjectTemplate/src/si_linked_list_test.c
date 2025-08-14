#include <stdio.h>

#include "si_singular_list.h"

void test00(void)
{
	const size_t initial_capacity = 69u;
	si_singular_list_t list = (si_singular_list_t){0};
	si_singular_list_init_2(&list, initial_capacity);

	for(int i = 0; i < 2; i++)
	{
		short next_short = i * 21;
		printf("Next Short: %hd\n", next_short);

		//si_singular_list_append_raw(&list, &next_short, sizeof(short));
	}
	printf("test00 initial values: ");
	//si_singular_list_fprint(&list, stdout);
	fprintf(stdout, "\n");

	const size_t r_index = si_singular_list_count(&list) - 1u;
	si_singular_list_remove_at(&list, r_index);

	printf("test00 after remove: ");
	//si_singular_list_fprint(&list, stdout);
	fprintf(stdout, "\n");

	si_singular_list_free(&list);
}

int main(int argc, char** pp_argv)
{
	test00();
}
