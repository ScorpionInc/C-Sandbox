#!/bin/bash
echo "Start unit test build script for si_thread."
gcc ./tests_src/si_tasker_test.c ./src/si_tasker.c ./tests_src/unity.c ../si_data/src/si_map.c -Itests_include -Iinclude -I../si_data/include -o 'a.out'
valgrind -s ./a.out
rm -f ./a.out
echo "End of unit test build script for si_thread."
