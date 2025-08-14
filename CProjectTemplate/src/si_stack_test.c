#include "si_stack.h"

// TODO Test templates

void test00()
{
	si_stack_t stack = {};
	si_stack_new(&stack, sizeof(int));
	for(int i = 0; i < 6; i++)
	{
		printf("Pushed onto the stack: %i\n", i);
		si_stack_push(&stack, &i);
	}
	int buffer = 9;
	for(int i = 0; i < 3; i++)
	{
		si_stack_pop(&stack, &buffer);
		printf("Popped off the stack: %i\n", buffer);
	}
	si_stack_free(&stack);
}


#define SI_TEMPLATE_TYPE int
#include "si_stack.template"

void test01()
{
	int_stack_t stack = {0};
	int_stack_new(&stack);
	for(int i = 0; i < 6; i++)
	{
		printf("Pushed onto the stack: %i\n", i);
		int_stack_push(&stack, i);
	}
	for(int i = 0; i < 3; i++)
	{
		printf("Popped off the stack: %i\n", int_stack_pop(&stack));
	}
	int_stack_free(&stack);
}

int main(int argc, char** pp_argv)
{
	test00();
	test01();
	return 0;
}
