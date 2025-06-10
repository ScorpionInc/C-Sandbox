#include <byteswap.h>

#include <stdio.h>
#include <string.h>

#include "si_bits.h"

int main(int argc, char** pp_argv)
{
	const uint32_t data_le = 0xDEADBEEF;
	const uint32_t data_be = __bswap_32(data_le);
	printf("byte_bits 0xDE: "); fprint_byte_bits(stdout, 0xDE); printf("\n");
	
	printf("le_bits_5 data: ");
	for(int i = 0; i < 2; i++)printf(" ");
	fprint_le_bits_5(stdout, (uint8_t*)&data_le, sizeof(uint32_t), 2u, 28u);
	printf("\n");
	
	printf("bits          : ");
	fprint_grouped_bits(stdout, (uint8_t *)&data_le, sizeof(uint32_t)); printf("\n");
	
	printf("be_bits_5 data: ");
	for(int i = 0; i < 2; i++)printf(" ");
	fprint_be_bits_5(stdout, (uint8_t*)&data_be, sizeof(uint32_t), 2u, 28u);
	printf("\n");
	
	const char* expected = "11011110 10101101 10111110 11101111";
	printf("Expected      : %s\n", expected);
	char buffer[36];
	buffer[35] = '\0';
	snprint_bits(buffer, 36, (uint8_t*)&data_le, sizeof(uint32_t));
	if(strcmp(buffer, expected) != 0)
	{
		printf("Test Failed.\n");
	} else {
		printf("Test Passed.\n");
	}
	return 0;
}
