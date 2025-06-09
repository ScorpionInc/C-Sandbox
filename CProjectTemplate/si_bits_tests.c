#include <stdio.h>

#include "si_bits.h"

int main(int argc, char** pp_argv)
{
	uint32_t data = 0xDEADBEEF;
	fprint_bits(stdout, (uint8_t *)&data, sizeof(uint32_t));
	const char* expected = "11011110 10101101 10111110 11101111";
	char buffer[36];
	buffer[35] = '\0';
	snprint_bits(buffer, 36, &data, sizeof(uint32_t));
	printf("\n");
	return 0;
}
