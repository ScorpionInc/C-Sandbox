#!/bin/bash
echo "Start unit test build script for si_thread."
gcc -ggdb ./tests_src/si_tasker_test.c ./src/* ./tests_src/unity.c ../si_data/src/* -Itests_include -Iinclude -I../si_data/include -o 'a.out' -lm
valgrind -s ./a.out
rm -f ./a.out
echo "End of unit test build script for si_thread."
