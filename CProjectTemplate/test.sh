#!/bin/bash
gcc -Wall ./test.c -o test.elf -lm
valgrind ./test.elf
