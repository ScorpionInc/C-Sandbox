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
	for (int iii = 0; iii < 6; iii++)
	{
		printf("Pushed onto the stack: %i\n", iii);
		si_stack_push(&stack, &iii);
	}
	int buffer = 9;
	for (int iii = 0; iii < 3; iii++)
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
	for (int iii = 0; iii < 6; iii++)
	{
		printf("Pushed onto the stack: %i\n", iii);
		int_stack_push(&stack, iii);
	}
	for (int iii = 0; iii < 3; iii++)
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

int main(void)
{
	printf("Start of si_stack unit test.\n");
	si_stack_test_all();
	printf("End of si_stack unit test.\n");
	return 0;
}
