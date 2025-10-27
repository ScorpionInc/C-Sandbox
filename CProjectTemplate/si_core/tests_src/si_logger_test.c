// si_logger_test.c

#include <stdio.h>

#include "unity.h"
#include "si_logger.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

struct exampleObject
{
	int whole;
	char letter;
	float real;
};

/** Doxygen
 * @brief Prints an example object struct's values to a FILE stream.
 * 
 * @param p_file Pointer to FILE to print to.
 * @param p_object Pointer to example object struct to be printed.
 */
static void example_object_fprint(FILE* p_file, struct exampleObject* p_object)
{
	if ((NULL == p_file) || (NULL == p_object))
	{
		goto END;
	}
	fprintf(p_file, "{w: %d; l: %c; r: %f}",
		p_object->whole, p_object->letter, p_object->real
	);
END:
	return;
}

/** Doxygen
 * @brief Tests si_logger
 */
static void si_logger_test_main(void)
{
	struct exampleObject example = {0};
	example.whole = 42;
	example.letter = '*';
	example.real = 3.1415926f;

	si_logger_t* p_logger = si_logger_new();
	p_logger->logging_level = SI_LOGGER_ALL;
	si_logger_critical(p_logger, "Program has encountered a critical error.");
	si_logger_error(p_logger, "Program has encountered an error.");
	si_logger_warning(p_logger, "Program is still running.");
	si_logger_info(p_logger, "Testing output to stdout.");
	si_logger_debug(p_logger, "Logger is at address: %p.", p_logger);
	si_logger_custom(
		p_logger, SI_LOGGER_DEBUG,
		"Example object value: ", &example, " with a custom print function.\n",
		(void(*)(FILE* const, const void* const))example_object_fprint
	);
	si_logger_log(p_logger, "Critical custom log level message.", 1234567890u);
	si_logger_destroy(&p_logger);
}

/** Doxygen
 * @brief Runs all local unit tests for si_logger
 */
static void si_logger_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_logger_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_logger unit test(s).\n");
	si_logger_test_all();
	(void)printf("End of si_logger unit test(s).\n");
	return 0;
}
