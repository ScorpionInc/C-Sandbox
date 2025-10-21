// si_paddr_test.c
#include <stdio.h> // fprintf(), printf()

#include "unity.h"
#include "si_paddr.h"

/* Is run before every test, put unit init calls here. */
void setUp (void)
{
}
/* Is run after every test, put unit clean-up calls here. */
void tearDown (void)
{
}

/** Doxygen
 * @brief Runs local unit test for ipv4 functionality.
 */
static void si_paddr_test_ipv4(void)
{
	// 127.0.0.1 /8
	const uint8_t p_address[INET_ADDRESS_SIZE] = {0x7f, 0x00, 0x00, 0x01};
	const uint8_t p_sub_msk[INET_ADDRESS_SIZE] = {0xFF, 0x00, 0x00, 0x00};
	sin_addr_fprint(stdout, p_address);
	fprintf(stdout, "\n");

	uint8_t p_network_address[INET_ADDRESS_SIZE] = {0};
	memcpy(p_network_address, p_address, INET_ADDRESS_SIZE);
	convert_ipv4_addr_to_network(p_network_address, p_sub_msk);
	TEST_ASSERT_EQUAL(0x7f, p_network_address[0]);
	TEST_ASSERT_EQUAL(0x00, p_network_address[1]);
	TEST_ASSERT_EQUAL(0x00, p_network_address[2]);
	TEST_ASSERT_EQUAL(0x00, p_network_address[3]);
	TEST_ASSERT_TRUE(is_within_ipv4_network(
		p_network_address, p_sub_msk, p_address
	));

	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = 8080;
	memcpy(&(addr.sin_addr), p_address, INET_ADDRESS_SIZE);

	TEST_ASSERT_TRUE(is_localhost_address_v4(&addr));
	TEST_ASSERT_TRUE(sockaddr_in_is_valid(&addr));

	char* p_address_str = sockaddr_as_str((struct sockaddr*)&addr);
	TEST_ASSERT_NOT_NULL(p_address_str);
	fprintf(stdout, "%s\n", p_address_str);
	free(p_address_str);
	p_address_str = NULL;
}

/** Doxygen
 * @brief Runs local unit test for ipv6 functionality.
 */
static void si_paddr_test_ipv6(void)
{
	//!TODO
	const uint8_t p_address[INET6_ADDRESS_SIZE] = {0};
	sin6_addr_fprint(stdout, p_address);
	fprintf(stdout, "\n");
}

/** Doxygen
 * @brief Runs all locally defined unit tests.
 */
static void si_paddr_test_all(void)
{
	UNITY_BEGIN();
	RUN_TEST(si_paddr_test_ipv4);
	RUN_TEST(si_paddr_test_ipv6);
	UNITY_END();
}

int main(void)
{
	(void)printf("Start of si_ unit test.\n");
	si_paddr_test_all();
	(void)printf("End of si_ unit test.\n");
	return 0;
}
