#!/bin/bash
echo 'Testing test.c'
gcc -Wall ./test.c -o test.elf -lm &&
valgrind ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_bits_test.c'
gcc -Wall ./si_bits.c ./si_bits_tests.c -o test.elf &&
valgrind ./test.elf &&
rm -f ./test.elf
echo
