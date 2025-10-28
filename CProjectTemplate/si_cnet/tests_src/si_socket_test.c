// si_socket_test.c
#include <stdio.h> // printf()

#include "unity.h"
#include "si_socket.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

void si_socket_test_main(void)
{
	si_socket_t socket = si_socket_invalid;
	TEST_ASSERT_FALSE(si_socket_is_valid(&socket));
	TEST_ASSERT_TRUE(si_socket_is_invalid(&socket));
	si_socket_init_4(&socket, AF_INET, SOCK_STREAM, 0);
	TEST_ASSERT_FALSE(si_socket_is_invalid(&socket));
	TEST_ASSERT_TRUE(si_socket_is_valid(&socket));

	si_socket_set_blocking(&socket, true);
	TEST_ASSERT_TRUE(si_socket_is_blocking(&socket));
	si_socket_set_blocking(&socket, false);
	TEST_ASSERT_FALSE(si_socket_is_blocking(&socket));

	si_socket_set_keepalive(&socket, true);
	TEST_ASSERT_TRUE(si_socket_is_keepalive(&socket));
	si_socket_set_keepalive(&socket, false);
	TEST_ASSERT_FALSE(si_socket_is_keepalive(&socket));

	si_socket_close(&socket);
	TEST_ASSERT_FALSE(si_socket_is_valid(&socket));
	TEST_ASSERT_TRUE(si_socket_is_invalid(&socket));
}

/** Doxygen
 * @brief Runs all locally defined unit tests.
 */
static void si_socket_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_socket_test_main);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_socket unit test.\n");
	si_socket_test_all();
	(void)printf("End of si_socket unit test.\n");
	return 0;
}