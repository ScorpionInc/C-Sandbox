#include "unity.h"
#include "si_stack.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_stack_test_modify(void)
{
	// TODO Use unity TEST macros
	si_stack_t stack = {0};
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

void si_stack_test_template(void)
{
	// TODO Use unity TEST macros
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

void si_stack_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_stack_test_modify);
	RUN_TEST(si_stack_test_template);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	si_stack_test_all();
	return 0;
}
