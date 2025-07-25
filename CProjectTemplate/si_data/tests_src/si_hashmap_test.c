#include <stdio.h>

#include "unity.h"
#include "si_hashmap.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_hashmap_test_init(void)
{
	si_hashmap_t* p_hashmap = si_hashmap_new();
	si_hashmap_free_at(&p_hashmap);
}

void si_hashmap_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_hashmap_test_init);
	UNITY_END();
}

int main(int argc, char** pp_argv)
{
	si_hashmap_test_all();
	return 0;
}
