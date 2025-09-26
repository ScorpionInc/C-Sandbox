// si_argparse_test.c

#include <stdbool.h> // bool, false, true

#include "unity.h"
#include "si_argparse.h"
#include "si_arg_parsers.h"
#include "si_arg_validators.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs local unit tests for validators
 */
static void si_argparse_test_validators(void)
{
	const uint16_t valid_port   = 3389u;
	const uint16_t invalid_port = 0u;
	const char* const p_valid_dir = "./";
	const char* const p_invalid_dir = "\0/";

	TEST_ASSERT_FALSE(port_validator(NULL));
	TEST_ASSERT_TRUE(port_validator(&valid_port));
	TEST_ASSERT_FALSE(port_validator(&invalid_port));

	TEST_ASSERT_FALSE(dir_validator(NULL));
	TEST_ASSERT_FALSE(dir_validator(""));
	TEST_ASSERT_TRUE(dir_validator(p_valid_dir));
	TEST_ASSERT_FALSE(dir_validator(p_invalid_dir));
}

/** Doxygen
 * @brief Runs local unit tests for parsers
 */
static void si_argparse_test_parsers(void)
{
	const char* const p_valid_port = "3389";
	const char* const p_invalid_port = "-3389";
	const char* const p_valid_path = ".";

	TEST_ASSERT_NULL(port_parser(NULL));
	TEST_ASSERT_NULL(port_parser(p_invalid_port));
	const uint16_t* p_parsed_port = port_parser(p_valid_port);
	TEST_ASSERT_NOT_NULL(p_parsed_port);
	printf("Parsed port: '%hu'\n", *p_parsed_port);
	free((void*)p_parsed_port);
	p_parsed_port = NULL;
	TEST_ASSERT_NULL(p_parsed_port);

	TEST_ASSERT_NULL(dir_parser(NULL));
	TEST_ASSERT_NULL(dir_parser(""));
	const char* p_parsed_path = dir_parser(p_valid_path);
	TEST_ASSERT_NOT_NULL(p_valid_path);
	printf("Parsed directory: '%s'\n", p_parsed_path);
	free((void*)p_parsed_path);
	p_parsed_path = NULL;
	TEST_ASSERT_NULL(p_parsed_path);
}

/** Doxygen
 * @brief Sets up arguments and values for testing argparse from test_main().
 * 
 * @param p_argpase Pointer to the argparse to be setup.
 */
static void setup_main_parser(si_argparse_t* const p_argparse)
{
	if(NULL == p_argparse)
	{
		goto END;
	}

	si_arg_t help_arg = {0};
	si_arg_init(&help_arg);
	help_arg.p_full = "help";
	help_arg.flag = '?';
	help_arg.p_help = "Print this help statement and exit.";
	help_arg.is_optional = true;

	si_arg_t version_arg = {0};
	si_arg_init(&version_arg);
	version_arg.p_full = "version";
	version_arg.flag = 'V';
	version_arg.p_help = "Print the software version and exits.";
	version_arg.is_optional = true;

	si_arg_t port_arg = {0};
	si_arg_init(&port_arg);
	port_arg.p_full = "port";
	port_arg.flag = 'p';
	port_arg.p_help = "Specifies a required port number.";
	port_arg.maximum_values = 1u;
	port_arg.minimum_values = 1u;
	port_arg.p_parser = port_parser;
	port_arg.p_validate = port_validator;
	port_arg.p_free_value = free;

	si_arg_t dir_arg = {0};
	si_arg_init(&dir_arg);
	dir_arg.p_full = "dir";
	dir_arg.flag = 'd';
	dir_arg.p_help = "Specifies a required directory path.";
	dir_arg.maximum_values = 1u;
	dir_arg.minimum_values = 1u;
	dir_arg.p_parser = dir_parser;
	dir_arg.p_validate = dir_validator;
	dir_arg.p_free_value = free;

	p_argparse->p_description = "Unit Test Program";
	si_argparse_add_argument(p_argparse, &help_arg);
	si_argparse_add_argument(p_argparse, &version_arg);
	si_argparse_add_argument(p_argparse, &port_arg);
	si_argparse_add_argument(p_argparse, &dir_arg);
END:
	return;
}

/** Doxygen
 * @brief Runs the setup main parser on provided values.
 * 
 * @param p_argparse Pointer to the argparse to be used.
 * @param argc Number of parameters being passed/parsed.
 * @param pp_argv Pointer to C string array of parameters.
 */
static void run_main_parser(si_argparse_t* const p_argparse,
	const int argc, const char** const pp_argv)
{
	if((NULL == p_argparse) || (NULL == pp_argv))
	{
		goto END;
	}
	const bool was_parsed = si_argparse_parse(p_argparse, argc, pp_argv);
	TEST_ASSERT_TRUE(was_parsed);
	const bool is_parser_valid = si_argparse_is_valid_values(p_argparse);
	if(true != is_parser_valid)
	{
		si_argparse_fprint_error(p_argparse, stderr);
		si_argparse_fprint_help(p_argparse, stdout);
		TEST_ASSERT_TRUE(is_parser_valid);
		goto END;
	}
	const bool is_help = si_argparse_is_set(p_argparse, "?");
	if(true == is_help)
	{
		si_argparse_fprint_help(p_argparse, stdout);
		goto END;
	}
	const bool is_version = si_argparse_is_set(p_argparse, "version");
	if (true == is_version)
    {
        printf("Version 0.0.1b\n");
        goto END;
    }
	const uint16_t* p_port = si_argparse_value_of(p_argparse, "port");
	TEST_ASSERT_NOT_NULL(p_port);
	printf("Value of port from run_main_parser: %hu\n", *p_port);
	const char* p_dir = si_argparse_value_of(p_argparse, "dir");
	printf("Value of dir from run_main_parser: %s\n", p_dir);
	TEST_ASSERT_NOT_NULL(p_dir);
END:
	return;
}

/** Doxygen
 * @brief Runs local unit tests for argparse
 */
static void si_argparse_test_main(void)
{
	const char* const pp_argv[] =
	{
		"si_argparse_test.out", "-p", "3389", "-d", "."
	};
	const int argc = sizeof(pp_argv) / sizeof(pp_argv[0]);

	si_argparse_t parser = {0};
	si_argparse_init(&parser);

	setup_main_parser(&parser);
	run_main_parser(&parser, argc, (const char**)pp_argv);

	si_argparse_free(&parser);
}

/** Doxygen
 * @brief Runs all local unit tests for si_argparse
 */
static void si_argparse_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_argparse_test_validators);
	RUN_TEST(si_argparse_test_parsers);
	RUN_TEST(si_argparse_test_main);
	UNITY_END();
}

int main(void)
{
	printf("Start of si_argparse unit test(s).\n");
	si_argparse_test_all();
	printf("End of si_argparse unit test(s).\n");
	return 0;
}