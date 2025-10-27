// si_strings_test.c

// Needed for the 'n' version of strndup()
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#endif//__STDC_ALLOC_LIB__

#ifdef __linux__
#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif//_POSIX_C_SOURCE
#endif

#include <stdio.h>  // printf()
#include <stdlib.h> // calloc(), free()
#include <string.h> // strcmp(), strncmp(), strndup()

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
 * @brief Runs unit test of string generator functions from si_strings.
 */
static void si_strings_test_generators(void)
{
	const char* const p_pattern  = "abc";
	const char* const p_expected = "abcabcabcabcab";
	const size_t expected_len = 14u;
	int cmp_result = 0;
	char* p_data = NULL;

	TEST_ASSERT_NULL(str_new(0u, p_pattern));
	TEST_ASSERT_NULL(str_new(0u, NULL));

	p_data = str_new(expected_len, NULL);
	TEST_ASSERT_NOT_NULL(p_data);
	free(p_data);
	p_data = NULL;

	p_data = str_new(expected_len, p_pattern);
	TEST_ASSERT_NOT_NULL(p_data);
	cmp_result = strncmp(p_data, p_expected, expected_len);
	TEST_ASSERT_EQUAL_INT(0, cmp_result);
	printf("Generated pattern heap string: '%s'\n", p_data);
	free(p_data);
	p_data = NULL;
}

/** Doxygen
 * @brief Runs unit test of string padding functions from si_strings.
 */
static void si_strings_test_padding(void)
{
	const char* const p_base = "It's a feature";
	const size_t base_len = 14u;
	const int abs_pad = (int)base_len + 12;
	const char* const p_left_expected = "            It's a feature";
	const char* const p_right_expected = "It's a feature            ";
	int cmp_result = 0;
	char* p_data = NULL;
	printf("Non-padded string: '%s'\n", p_base);

	TEST_ASSERT_NULL(strn_clone_pad(NULL, 0u, 0));
	TEST_ASSERT_NULL(strn_clone_pad(NULL, base_len, 0));
	TEST_ASSERT_NULL(strn_clone_pad(NULL, base_len, abs_pad));
	TEST_ASSERT_NULL(strn_clone_pad(p_base, 0u, 0));
	TEST_ASSERT_NULL(strn_clone_pad(p_base, 0u, abs_pad));

	p_data = strn_clone_pad(p_base, base_len, abs_pad);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Left padded string: '%s'\n", p_data);
	cmp_result = strncmp(p_left_expected, p_data, (size_t)abs_pad);
	TEST_ASSERT_EQUAL_INT(0, cmp_result);
	free(p_data);
	p_data = NULL;

	p_data = strn_clone_pad(p_base, base_len, -1 * abs_pad);
	TEST_ASSERT_NOT_NULL(p_data);
	printf("Right padded string: '%s'\n", p_data);
	cmp_result = strncmp(p_right_expected, p_data, (size_t)abs_pad);
	TEST_ASSERT_EQUAL_INT(0, cmp_result);
	free(p_data);
	p_data = NULL;
}

/** Doxygen
 * @brief Tests si_strings functions that join strings on the heap
 */
static void si_strings_test_concats(void)
{
	// Declare test data/variables
	const char* const p_string_a = "This is an example";
	const char* const p_string_b = "string used for testing";
	const char* const p_string_joined_a =
		"This is an examplestring used for testing";
	const char* const p_strings[] =
	{
		"This", "is", "an", "example", "string", "used", "for", "testing"
	};
	const size_t strings_count = (sizeof(p_strings) / sizeof(const char*));
	const char* const p_string_joined_b =
		"This is an example string used for testing";
	const char* const p_spacer = " ";
	const size_t p_string_a_len = strnlen(p_string_a, INT64_MAX);
	const size_t p_string_b_len = strnlen(p_string_b, INT64_MAX);
	TEST_ASSERT_LESS_THAN_size_t(INT64_MAX, p_string_a_len);
	TEST_ASSERT_LESS_THAN_size_t(INT64_MAX, p_string_b_len);
	char* p_value = NULL;
	int s_cmp = 0;
	size_t new_size = 0u;

	printf("Testing strn_lgrow_concat():\n");
	p_value = strndup(p_string_a, p_string_a_len);

	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, strn_lgrow_concat(NULL, 0u, NULL, 0u));
	TEST_ASSERT_EQUAL_size_t(
		SIZE_MAX, strn_lgrow_concat(
			&p_value, p_string_a_len, NULL, p_string_b_len
		)
	);
	TEST_ASSERT_EQUAL_size_t(
		SIZE_MAX, strn_lgrow_concat(
			NULL, p_string_a_len, p_string_b, p_string_b_len
		)
	);
	new_size = strn_lgrow_concat(
		&p_value, p_string_a_len, p_string_b, p_string_b_len
	);
	TEST_ASSERT_NOT_NULL(p_value);
	TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, new_size);
	TEST_ASSERT_EQUAL_size_t((p_string_a_len + p_string_b_len), new_size);
	printf("%s\n", p_value);
	free(p_value);
	p_value = NULL;

	printf("Testing str_lgrow_concat():\n");
	p_value = strndup(p_string_a, p_string_a_len);
	TEST_ASSERT_NOT_NULL(p_value);
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, str_lgrow_concat(NULL, NULL));
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, str_lgrow_concat(NULL, p_string_b));
	new_size = str_lgrow_concat(&p_value, p_string_b);
	TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, new_size);
	TEST_ASSERT_EQUAL_size_t((p_string_a_len + p_string_b_len), new_size);
	printf("%s\n", p_value);
	free(p_value);
	p_value = NULL;

	printf("Testing strn_rgrow_concat():\n");
	p_value = strndup(p_string_b, p_string_b_len);
	TEST_ASSERT_NOT_NULL(p_value);
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, strn_rgrow_concat(NULL, 0u, NULL, 0u));
	TEST_ASSERT_EQUAL_size_t(
		SIZE_MAX, strn_rgrow_concat(
			p_string_a, p_string_a_len, NULL, p_string_b_len
		)
	);
	TEST_ASSERT_EQUAL_size_t(
		SIZE_MAX, strn_rgrow_concat(
			NULL, p_string_a_len, &p_value, p_string_b_len
		)
	);
	new_size = strn_rgrow_concat(
		p_string_a, p_string_a_len, &p_value, p_string_b_len
	);
	TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, new_size);
	TEST_ASSERT_EQUAL_size_t((p_string_a_len + p_string_b_len), new_size);
	printf("%s\n", p_value);
	free(p_value);
	p_value = NULL;

	printf("Testing str_rgrow_concat():\n");
	p_value = strndup(p_string_b, p_string_b_len);
	TEST_ASSERT_NOT_NULL(p_value);
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, str_rgrow_concat(NULL, NULL));
	TEST_ASSERT_EQUAL_size_t(SIZE_MAX, str_rgrow_concat(p_string_a, NULL));
	new_size = str_rgrow_concat(p_string_a, &p_value);
	TEST_ASSERT_NOT_EQUAL_size_t(SIZE_MAX, new_size);
	TEST_ASSERT_EQUAL_size_t((p_string_a_len + p_string_b_len), new_size);
	printf("%s\n", p_value);
	free(p_value);
	p_value = NULL;

	printf("Testing strn_clone_concat():\n");
	TEST_ASSERT_NULL(strn_clone_concat(NULL, 0u, NULL, 0u));
	TEST_ASSERT_NULL(strn_clone_concat(p_string_a, 0u, NULL, 0u));
	TEST_ASSERT_NULL(strn_clone_concat(NULL, 0u, p_string_b, 0u));
	p_value = strn_clone_concat(
		p_string_a, p_string_a_len, p_string_b, p_string_b_len
	);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_string_joined_a, p_value);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing str_clone_concat():\n");
	TEST_ASSERT_NULL(str_clone_concat(NULL, NULL));
	TEST_ASSERT_NULL(str_clone_concat(p_string_a, NULL));
	TEST_ASSERT_NULL(str_clone_concat(NULL, p_string_b));
	p_value = str_clone_concat(p_string_a, p_string_b);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, p_string_joined_a);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing strv_clone_concat():\n");
	TEST_ASSERT_NULL(strv_clone_concat(0u));
	TEST_ASSERT_NULL(strv_clone_concat(0u, NULL));
	TEST_ASSERT_NULL(strv_clone_concat(0u, p_string_a, p_string_b));
	p_value = strv_clone_concat(2u, p_string_a, p_string_b);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, p_string_joined_a);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing str_clone_join():\n");
	TEST_ASSERT_NULL(str_clone_join(0, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_join(SIZE_MAX, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_join(0, p_spacer, NULL));
	p_value = str_clone_join(strings_count, p_spacer, (const char**)p_strings);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, p_string_joined_b);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	printf("Testing strv_clone_join():\n");
	TEST_ASSERT_NULL(strv_clone_join(0u, NULL));
	TEST_ASSERT_NULL(strv_clone_join(0u, p_spacer));
	p_value = strv_clone_join(8u, p_spacer,
		"This", "is", "an", "example", "string", "used", "for", "testing"
	);
	TEST_ASSERT_NOT_NULL(p_value);
	s_cmp = strcmp(p_value, p_string_joined_b);
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

static void print_string_array(char** const pp_array, const size_t count)
{
	if (NULL == pp_array)
	{
		goto END;
	}
	printf("{");
	for (size_t iii = 0u; iii < count; iii++)
	{
		printf("%p:'%s'", (void*)&(pp_array[iii]), pp_array[iii]);
		if (count - 1u > iii)
		{
			printf(", ");
		}
	}
	printf("}:%lu\n", count);
END:
	return;
}

static void si_strings_test_split(void)
{
	const char* const p_haystack = "This is a test string.";
	const char* const p_needle = " ";
	char** pp_result = NULL;
	size_t items = 0u;
	const size_t expected_items = 5u;
	const char* p_expected[5u] = {"This", "is", "a", "test", "string."};

	TEST_ASSERT_NULL(str_split(NULL, NULL, NULL));
	TEST_ASSERT_NULL(str_split(NULL, p_needle, &items));
	TEST_ASSERT_NULL(str_split(p_haystack, NULL, &items));
	TEST_ASSERT_NULL(str_split(NULL, p_needle, NULL));
	TEST_ASSERT_NULL(str_split(p_haystack, p_needle, NULL));
	TEST_ASSERT_NULL(str_split(p_haystack, NULL, NULL));
	TEST_ASSERT_NULL(str_split(NULL, NULL, &items));
	pp_result = str_split(p_haystack, p_needle, &items);
	TEST_ASSERT_NOT_NULL(pp_result);
	print_string_array(pp_result, items);
	TEST_ASSERT_EQUAL_size_t(expected_items, items);
	for (size_t iii = 0u; iii < expected_items; iii++)
	{
		printf(
			"Comparing split string vs expected: '%s' == '%s'\n",
			pp_result[iii], p_expected[iii]
		);
		const int cmp_result = strncmp(
			pp_result[iii], p_expected[iii], CHAR_MAX
		);
		TEST_ASSERT_EQUAL_INT(0, cmp_result);
	}
	str_split_destroy(&pp_result, items);
	TEST_ASSERT_NULL(pp_result);
}

/** Doxygen
 * @brief Tests si_strings functions that modify strings length/size
 */
static void si_strings_test_manipulators(void)
{
	char* p_value = NULL;
	int s_cmp = 0;
	const char* const p_string_a =
		"I am the night! I am vengence! I am Bruce Wayne!";
	const char* const p_string_b = "Bruce Wayne";
	const char* const p_string_c = "Batman";
	const char* const p_string_d =
		"I am the night! I am vengence! I am Batman!";

	printf("Testing str_clone_substitute():\n");
	TEST_ASSERT_NULL(str_clone_substitute(NULL, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_substitute(NULL, NULL, p_string_c));
	TEST_ASSERT_NULL(str_clone_substitute(NULL, p_string_b, NULL));
	TEST_ASSERT_NULL(str_clone_substitute(NULL, p_string_b, p_string_c));
	TEST_ASSERT_NULL(str_clone_substitute(p_string_a, NULL, NULL));
	TEST_ASSERT_NULL(str_clone_substitute(p_string_a, NULL, p_string_c));
	TEST_ASSERT_NULL(str_clone_substitute(p_string_a, p_string_b, NULL));
	TEST_ASSERT_NULL(p_value);
	p_value = str_clone_substitute(p_string_a, p_string_b, p_string_c);
	TEST_ASSERT_NOT_NULL(p_value);
	printf("Substitution result(s): '%s'.\n", p_value);
	s_cmp = strcmp(p_string_d, p_value);
	TEST_ASSERT_EQUAL_INT(0, s_cmp);
	free(p_value);
	p_value = NULL;

	const char* const p_cstr_buffer =  "There\0is\0no\0cow\0level";
	const size_t cstr_size = 22u;
	const char* const p_cstr_words[] = { "There", "is", "no", "cow", "level" };
	size_t current_size = 0u;
	char* p_next = NULL;

	TEST_ASSERT_NULL(p_value);
	p_value = calloc(cstr_size, sizeof(char));
	TEST_ASSERT_NOT_NULL(p_value);
	current_size = cstr_size;
	memcpy(p_value, p_cstr_buffer, cstr_size);

	printf("Testing pop_str_from_heap():\n");
	size_t counter = 0u;
	TEST_ASSERT_NULL(p_next);
	p_next = pop_str_from_heap((uint8_t**)&p_value, &current_size);
	TEST_ASSERT_NOT_NULL(p_next);
	while (NULL != p_next)
	{
		TEST_ASSERT_NOT_NULL(p_next);
		printf("Testing next string: %s\n", p_next);
		TEST_ASSERT_EQUAL_STRING(p_cstr_words[counter], p_next);
		free(p_next);
		p_next = NULL;
		p_next = pop_str_from_heap((uint8_t**)&p_value, &current_size);
		counter++;
	}
}

typedef struct example_t
{
	uint32_t height_inches;
	uint32_t age;
} example_t;

/** Doxygen
 * @brief Demonstrates usage of the str_from_fprint() with a custom function.
 * 
 * @param p_file FILE pointer to be written to.
 * @param p_example Pointer to an example_t struct to print values from.
 * 
 * @return Returns fprintf results on success. Returns -1 otherwise.
 */
static int custom_example_fprint(FILE* const p_file,
	const struct example_t* const p_example)
{
	int result = -1;
	if ((NULL == p_file) || (NULL == p_example))
	{
		goto END;
	}
	const int fprintf_result = fprintf(
		p_file, "%u years old, %u inches tall",
		p_example->age, p_example->height_inches
	);
	result = fprintf_result;
END:
	return result;
}

/** Doxygen
 * @brief Tests si_strings function str_from_fprint()
 */
static void si_strings_test_from_fprint(void)
{
	const uint32_t age = 42u;
	const uint32_t height = 69u;
	const char* const P_DATA = "Hello World!";

	char* p_str = NULL;

	example_t example = {0};
	example.age = age;
	example.height_inches = height;

	TEST_ASSERT_NULL(str_from_fprint(NULL, NULL));
	TEST_ASSERT_NULL(str_from_fprint((str_fprint_f)fprintf, NULL));
	TEST_ASSERT_NULL(str_from_fprint(NULL, &P_DATA));

	p_str = str_from_fprint((str_fprint_f)fprintf, P_DATA);
	TEST_ASSERT_NOT_NULL(p_str);
	fprintf(stdout, "Got string value of: '%s'.\n", p_str);
	free(p_str);
	p_str = NULL;

	p_str = str_from_fprint((str_fprint_f)custom_example_fprint, &example);
	TEST_ASSERT_NOT_NULL(p_str);
	fprintf(stdout, "Got string value of: '%s'.\n", p_str);
	free(p_str);
	p_str = NULL;
}

/** Doxygen
 * @brief Runs all local unit tests for si_strings
 */
static void si_strings_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_strings_test_generators);
	RUN_TEST(si_strings_test_padding);
	RUN_TEST(si_strings_test_concats);
	RUN_TEST(si_strings_test_formatters);
	RUN_TEST(si_strings_test_split);
	RUN_TEST(si_strings_test_manipulators);
	RUN_TEST(si_strings_test_from_fprint);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_strings unit test(s).\n");
	si_strings_test_all();
	printf("End of si_strings unit test(s).\n");
	return 0;
}
