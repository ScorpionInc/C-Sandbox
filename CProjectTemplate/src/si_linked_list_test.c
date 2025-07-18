#include <stdio.h>

#include "si_linked_list.h"

void test00(void)
{
	short initial_short = 42;
	si_dynamic initial_value = {};
	si_dynamic_new_3(&initial_value, sizeof(short), 1u);
	si_dynamic_set(&initial_value, 0u, &initial_short);
	fprint_si_dynamic(stdout, &initial_value);
	printf("\n");

	si_linked_list list = (si_linked_list){};
	si_linked_list_new(&list, &initial_value);
	si_dynamic_free(&initial_value);

	si_dynamic next_value = {};
	for(int i = 0; i < 5; i++)
	{
		short next_short = i * 21;
		si_dynamic_new_3(&next_value, sizeof(short), 1u);
		si_dynamic_set(&next_value, 0u, &next_short);
		fprint_si_dynamic(stdout, &next_value);
		printf("\n");

		si_linked_list_append(&list, &next_value);
		si_dynamic_free(&next_value);
	}
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");
	si_linked_list_remove_at(&list, 3u);
	si_linked_list_fprint(&list, stdout);
	fprintf(stdout, "\n");
	si_linked_list_free(&list);
}

#define SI_TEMPLATE_TYPE int
#include "si_linked_list.template"

void test01(void)
{
	int_linked_list list = {};
	int_linked_list_new(&list, 42);
	for(int i = 0; i < 3; i++)
	{
		int_linked_list_append(&list, 389 * (i + 3));
	}
	int_linked_list_fprint(&list, stdout);
	printf("\n");
	for(int i = 0; i < 2; i++)
	{
		int_linked_list_remove_at(&list, 0u);
	}
	int_linked_list_fprint(&list, stdout);
	printf("\n");
	int_linked_list_free(&list);
}

int main(int argc, char** pp_argv)
{
	test00();
	test01();
}
