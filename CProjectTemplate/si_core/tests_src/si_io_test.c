// si_io_test.c

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
 * @brief Runs local unit tests for si_io file_clone
 */
static void si_io_test_file_clone(void)
{
	const char* const p_source_path = "./tests_resources/test.txt";
	const char* const p_sink_path   = "./tests_resources/test_copy.txt";

	printf("Testing file_clone_to():\n");
	TEST_ASSERT_FALSE(file_clone_to(NULL, NULL, false));
	TEST_ASSERT_FALSE(file_clone_to(p_source_path, NULL, true));
	TEST_ASSERT_FALSE(file_clone_to(NULL, p_sink_path, false));
	const bool clone_result = file_clone_to(p_source_path, p_sink_path, true);
	TEST_ASSERT_TRUE(clone_result);

	const int remove_result = remove(p_sink_path);
	TEST_ASSERT_EQUAL_INT(0, remove_result);
}

static bool handle_file(const char* const p_fullpath, struct dirent* p_entry)
{
	bool do_next = false;
	if ((NULL == p_fullpath) || (NULL == p_entry))
	{
		goto END;
	}
	printf("%s\n", p_fullpath);
	do_next = true;
END:
	return do_next;
}

/** Doxygen
 * @brief Runs local unit tests for si_io for_each_file
 */
static void si_io_test_for_each_file(void)
{
	const char* const p_list_path = "./tests_resources";

	printf("Testing for_each_file_4():\n");
	TEST_ASSERT_FALSE(for_each_file_4(NULL, NULL, true, false));
	TEST_ASSERT_FALSE(for_each_file_4(p_list_path, NULL, false, true));
	TEST_ASSERT_FALSE(for_each_file_4(NULL, handle_file, false, false));
	TEST_ASSERT_TRUE(for_each_file_4(p_list_path, handle_file, true, false));
}

/** Doxygen
 * @brief Runs all local unit tests for si_io
 */
static void si_io_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_io_test_file_clone);
	RUN_TEST(si_io_test_for_each_file);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_io unit test(s).\n");
	si_io_test_all();
	printf("End of si_io unit test(s).\n");
	return 0;
}
