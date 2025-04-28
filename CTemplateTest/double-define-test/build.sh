#!/bin/bash
echo Clean-up last build
2>/dev/null 1>&2 rm -f -- ./main.elf
echo Building and testing
gcc -Wall ./main.c -lm -o ./main.elf && valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./main.elf
echo End of script
