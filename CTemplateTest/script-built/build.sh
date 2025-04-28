#!/bin/bash
echo Build Script Starting.
echo Generating C Tempalates
/bin/bash ./generate_templates.sh
echo Clean-up last build.
rm -f -- ./main.elf
echo Building main.c
gcc -Wall ./main.c -o main.elf
valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./main.elf
echo Script Stopping.
