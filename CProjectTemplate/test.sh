#!/bin/bash

echo 'Testing si_bits_test.c'
gcc -Wall ./si_bits.c ./si_bits_tests.c -o test.elf &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_hex_test.c'
gcc -Wall ./si_hex.c ./si_hex_tests.c -o test.elf &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_array_test.c'
gcc -Wall ./si_realloc_settings.c ./si_dynamic.c ./si_array_test.c -o test.elf -lm &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_queue_test.c'
gcc -Wall ./si_realloc_settings.c ./si_dynamic.c ./si_queue.c ./si_queue_test.c -o test.elf -lm &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_stack_test.c'
gcc -Wall ./si_realloc_settings.c ./si_dynamic.c ./si_stack.c ./si_stack_test.c -o test.elf -lm &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo

echo 'Testing si_linked_list_test.c'
gcc -Wall ./si_realloc_settings.c ./si_dynamic.c ./si_linked_list.c ./si_linked_list_test.c -o test.elf -lm &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./test.elf &&
rm -f ./test.elf
echo
