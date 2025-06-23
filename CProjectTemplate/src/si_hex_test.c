#include <byteswap.h>

#include <stdio.h>
#include <string.h>

#include "si_hex.h"

int main(int argc, char** pp_argv)
{
	const uint32_t data_le = 0xDEADBEEF;
	const uint32_t data_be = __bswap_32(data_le);
	printf("byte_hex 0xDE: "); fprint_byte_hex(stdout, 0xDE); printf("\n");

	printf("le_hex_5 data: ");
	//for(int i = 0; i < 2; i++)printf(" ");
	//fprint_le_hex_5(stdout, (uint8_t*)&data_le, sizeof(uint32_t), 2u, 4u);
	fprint_le_hex(stdout, (uint8_t*)&data_le, sizeof(uint32_t));
	printf("\n");

	printf("be_hex_5 data: ");
	//for(int i = 0; i < 2; i++)printf(" ");
	//fprint_be_hex_5(stdout, (uint8_t*)&data_be, sizeof(uint32_t), 4u, 4u);
	fprint_be_hex(stdout, (uint8_t*)&data_be, sizeof(uint32_t));
	printf("\n");

	const char* expected = "de ad be ef";
	printf("Expected      : %s\n", expected);
	const size_t buffer_size = 12u;
	char buffer[buffer_size];
	buffer[buffer_size - 1u] = '\0';
	snprint_hex(buffer, buffer_size, (uint8_t*)&data_le, sizeof(uint32_t));
	if(strcmp(buffer, expected) != 0)
	{
		printf("Buffer    : %s\n", buffer);
		printf("Test Failed.\n");
	}
	else
	{
		printf("Test Passed.\n");
	}

	return 0;
}
