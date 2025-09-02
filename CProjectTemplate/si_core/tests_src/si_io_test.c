//si_io_test.c

#include <stdbool.h> // bool, false, true
#include <stdio.h> // remove()

#include "unity.h"
#include "si_io.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs all local unit tests for si_logger
 */
static void si_io_test_file_clone(void)
{
	const char* const p_source_path = "./tests_resources/test.txt";
	const char* const p_sink_path   = "./tests_resources/test_copy.txt";

	TEST_ASSERT_FALSE(file_clone_to(NULL, NULL, false));
	TEST_ASSERT_FALSE(file_clone_to(p_source_path, NULL, true));
	TEST_ASSERT_FALSE(file_clone_to(NULL, p_sink_path, false));
	const bool clone_result = file_clone_to(p_source_path, p_sink_path, true);
	TEST_ASSERT_TRUE(clone_result);

	const int remove_result = remove(p_sink_path);
	TEST_ASSERT_EQUAL_INT(0, remove_result);
}

/** Doxygen
 * @brief Runs all local unit tests for si_logger
 */
static void si_io_test_all(void)
{
    UNITY_BEGIN();
    RUN_TEST(si_io_test_file_clone);
    UNITY_END();
}

int main(void)
{
    printf("Start of si_io unit test(s).\n");
    si_io_test_all();
    printf("End of si_io unit test(s).\n");
    return 0;
}
