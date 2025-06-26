#include <stdio.h> //FILE printf()

#include "si_adler.h"

int main(int argc, char** pp_argv)
{
	const char* const inputs[] = {
		"",
		"~~~",
		"abc",
		"abd",
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
	};
	/*
	const char* const expected[] = {
		"0x0001",
		"0x0680",
		"0x572C",
		"0x582D",
		"0xA68B"
	};//*/
	const size_t inputs_count = 5u;
	si_adler hash = {};
	for(size_t i = 0u; i < inputs_count; i++)
	{
		si_adler_new(&hash, 2u);
		size_t input_length = strlen(inputs[i]);
		si_adler_update(&hash, (uint8_t*)inputs[i], input_length);
		printf("'%48s':", inputs[i]);
		si_adler_fprint(&hash, stdout); printf("\n");
		si_adler_free(&hash);
	}
	return 0;
}
