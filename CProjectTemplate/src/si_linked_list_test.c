#include <stdio.h>

#include "si_linked_list.h"

void test00(void)
{
	const size_t initial_capacity = 69u;
	si_linked_list_t list = (si_linked_list_t){};
	si_linked_list_new_2(&list, initial_capacity);

	for(int i = 0; i < 2; i++)
	{
		short next_short = i * 21;
		printf("Next Short: %hd\n", next_short);

		si_linked_list_append_raw(&list, &next_short, sizeof(short));
	}
	printf("test00 initial values: ");
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");

	const size_t r_index = si_linked_list_count(&list) - 1u;
	si_linked_list_remove_at(&list, r_index);

	printf("test00 after remove: ");
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");

	si_linked_list_free(&list);
}

#define SI_TEMPLATE_TYPE int
#include "si_linked_list.template"

void test01(void)
{
	int_linked_list list = {};
	int_linked_list_new(&list);

	for(int i = 0; i < 5; i++)
	{
		int next_int = 389 * (i + 3);
		printf("Next int: %d\n", next_int);

		int_linked_list_append_raw(&list, next_int);
	}

	printf("test01 initial values: ");
	int_linked_list_fprint(&list, stdout);
	printf("\n");

	for(int i = 0; i < 2; i++)
	{
		int_linked_list_remove_at(&list, 0u);
	}

	printf("test01 after remove: ");
	int_linked_list_fprint(&list, stdout);
	printf("\n");

	int_linked_list_free(&list);
}

int main(int argc, char** pp_argv)
{
	test00();
	//test01();
}
