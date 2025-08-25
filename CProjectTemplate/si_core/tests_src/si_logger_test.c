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

/** Doxygen
 * @brief Tests si_logger
 */
static void si_logger_test_main(void)
{
	si_logger_t* p_logger = si_logger_new();
	p_logger->p_file = stdout;
	p_logger->logging_level = SI_LOGGER_ALL;
	si_logger_critical(p_logger, "Program has encountered a critical error.");
	si_logger_error(p_logger, "Program has encountered an error.");
	si_logger_warning(p_logger, "Program is still running.");
	si_logger_info(p_logger, "Testing output to stdout.");
	si_logger_debug(p_logger, "Logger is at address: %p.", p_logger);
	free(p_logger);
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
	printf("Start of si_logger unit test(s).\n");
	si_logger_test_all();
	printf("End of si_logger unit test(s).\n");
	return 0;
}
