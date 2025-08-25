#include <stdio.h> //printf()
#include <stdlib.h> // calloc(), free()
#include <string.h> // strcmp(), strncmp()

#include "unity.h"
#include "si_strings.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Tests si_strings functions that join strings on the heap
 */
static void si_strings_test_concats(void)
{
	// Declare test data/variables
	const char* const string_a = "This is an example";
	const char* const string_b = "string used for testing";
	const char* const string_joined_a = "This is an examplestring used for testing";
	const char* const p_strings[] =
	{
		"This", "is", "an", "example", "string", "used", "for", "testing"
	};
	const size_t strings_count = (sizeof(p_strings) / sizeof(const char*));
	const char* const string_joined_b = "This is an example string used for testing";
	const char* const spacer = " ";
	const size_t string_a_len = strlen(string_a);
	const size_t string_b_len = strlen(string_b);
	char* p_value = NULL;
	int s_cmp = 0;

	printf("Testing strn_clone_concat():\n");
	TEST_ASSERT_NULL(strn_clone_concat(NULL, 0u, NULL, 0u));
	TEST_ASSERT_NULL(strn_clone_concat(string_a, 0u, NULL, 0u));
	TEST_ASSERT_NULL(strn_clone_concat(NULL, 0u, string_b, 0u));
	p_value = strn_clone_concat(
		string_a, string_a_len, string_b, string_b_len
	);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(string_joined_a, p_value);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing str_clone_concat():\n");
	TEST_ASSERT_NULL(str_clone_concat(NULL, NULL));
	TEST_ASSERT_NULL(str_clone_concat(string_a, NULL));
	TEST_ASSERT_NULL(str_clone_concat(NULL, string_b));
	p_value = str_clone_concat(string_a, string_b);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, string_joined_a);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing str_clone_join():\n");
	TEST_ASSERT_NULL(str_clone_join(0, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_join(SIZE_MAX, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_join(0, spacer, NULL));
	p_value = str_clone_join(strings_count, spacer, (const char**)p_strings);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, string_joined_b);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing strv_clone_join():\n");
	TEST_ASSERT_NULL(strv_clone_join(0u, NULL));
	TEST_ASSERT_NULL(strv_clone_join(0u, spacer));
	p_value = strv_clone_join(8u, spacer,
		"This", "is", "an", "example", "string", "used", "for", "testing"
	);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, string_joined_b);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;
}

/** Doxygen
 * @brief Tests si_strings functions that modify strings characters inline
 */
static void si_strings_test_formatters(void)
{
	const char* const p_base = "'...there will be cake' ~GLaDOS";
	const char* const p_upper = "'...THERE WILL BE CAKE' ~GLADOS";
	const char* const p_lower = "'...there will be cake' ~glados";
	const size_t base_size = strlen(p_base);
	int s_cmp = 0;
	char* p_buffer = calloc(base_size + 1u, sizeof(char));
	TEST_ASSERT_NOT_NULL(p_buffer);
	memcpy(p_buffer, p_base, base_size);
	p_buffer[base_size] = '\0';
	s_cmp = strncmp(p_buffer, p_base, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	strn_to_uppercase(p_buffer, 0u);
	strn_to_lowercase(p_buffer, 0u);
	s_cmp = strncmp(p_buffer, p_base, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	printf("Testing strn_to_uppercase():\n");
	strn_to_uppercase(NULL, 0u);
	strn_to_uppercase(p_buffer, base_size);
	s_cmp = strncmp(p_buffer, p_upper, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	printf("Testing strn_to_lowercase():\n");
	strn_to_lowercase(NULL, 0u);
	strn_to_lowercase(p_buffer, base_size);
	s_cmp = strncmp(p_buffer, p_lower, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	printf("Testing str_to_uppercase():\n");
	str_to_uppercase(NULL);
	str_to_uppercase(p_buffer);
	s_cmp = strncmp(p_buffer, p_upper, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	printf("Testing str_to_lowercase():\n");
	str_to_lowercase(NULL);
	str_to_lowercase(p_buffer);
	s_cmp = strncmp(p_buffer, p_lower, base_size);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);

	free(p_buffer);
	p_buffer = NULL;
}

/** Doxygen
 * @brief Tests si_strings functions that modify strings length/size
 */
static void si_strings_test_manipulators(void)
{
	const char* const cstr_buffer =  "There\0is\0no\0cow\0level";
	const size_t cstr_size = 22u;
	const char* const cstr_words[] = { "There", "is", "no", "cow", "level" };
	char* p_value = NULL;
	size_t current_size = 0u;
	p_value = calloc(cstr_size, sizeof(char));
	TEST_ASSERT_NOT_NULL(p_value);
	current_size = cstr_size;
	memcpy(p_value, cstr_buffer, cstr_size);

	printf("Testing pop_str_from_heap():\n");
	size_t counter = 0u;
	char* p_next = pop_str_from_heap((uint8_t**)&p_value, &current_size);
	TEST_ASSERT_NOT_NULL(p_next);
	while(NULL != p_next)
	{
		TEST_ASSERT_NOT_NULL(p_next);
		printf("Testing next string: %s\n", p_next);
		TEST_ASSERT_EQUAL_STRING(cstr_words[counter], p_next);
		free(p_next);
		p_next = NULL;
		p_next = pop_str_from_heap((uint8_t**)&p_value, &current_size);
		counter++;
	}
}

static void si_strings_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_strings_test_concats);
	RUN_TEST(si_strings_test_formatters);
	RUN_TEST(si_strings_test_manipulators);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_strings unit test(s).\n");
	si_strings_test_all();
	printf("End of si_strings unit test(s).\n");
	return 0;
}