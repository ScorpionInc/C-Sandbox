#include <pthread.h>
#include <stdio.h>

#include "si_tasker.h"

void test01(void)
{
	//!TODO
	si_tasker tasker = {};
	si_tasker_new(&tasker);
	si_tasker_free(&tasker);
}

int main(int argc, char** pp_argv)
{
	printf("\nRunning si_thread_test.c\n");
	//test00();
	test01();
	printf("\n");
	return 0;
}
